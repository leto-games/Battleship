add_compile_definitions(${PRESET_DEFINE_MACRO})

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(
	GLOB C_CPP_COURCES 
	"Src/*/*.cpp" "Src/*/*.c"
	"Src/*/*/*.cpp" "Src/*/*/*/*.c"
)

if("${PRESET_DEFINE_MACRO}" STREQUAL "__STM32__")
	project("Battleship" C CXX ASM)
	file(GLOB HW_COURCES "Src/*.cpp" "Src/*.c" )
	add_executable(${PROJECT_NAME} "${C_CPP_COURCES}" "${HW_COURCES}")
	set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")
else()
	project("Battleship" C CXX)
	add_library("${PROJECT_NAME}" SHARED "${C_CPP_COURCES}")
	if(WIN32 AND DEFINED ENV{LETO_PATH})
		target_compile_definitions(${PROJECT_NAME} PRIVATE LETOCORE_SHARED)
	endif()
endif()

# glibc never unmaps a DSO that exports STB_GNU_UNIQUE symbols, and GCC emits them
# for local statics in inline and template code. AppLoader closes game libraries
# with dlclose, so the game may not carry such symbols. GNU only: AppleClang and
# MSVC reject the flag, and Mach-O has no STB_GNU_UNIQUE.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE -fno-gnu-unique)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Src>
    $<INSTALL_INTERFACE:Src>
)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin     # Для исполняемых файлов
    ARCHIVE DESTINATION lib     # Для статических библиотек
    LIBRARY DESTINATION lib     # Для динамических библиотек
)
target_compile_definitions(${PROJECT_NAME} PRIVATE LETOGAMES_EXPORT_LIBRARY)

if(${PRESET_DEFINE_MACRO} STREQUAL "__STM32__")
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		USE_HAL_DRIVER
		${MCU_CHIP}
	)
	
	# ------------------------------------------------------------------------------
	# Настройки архитектуры из .cproject (STM32F401CCUx)
	# ------------------------------------------------------------------------------
	set(OBJECT_GEN_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb")
	set(COMPILER_FLAGS "-ffunction-sections -fdata-sections -Wall -fstack-usage -flto")
	# -fcyclomatic-complexity - Используется только в STM32CubeIDE

	# Флаги компиляции для C / C++ / ASM
	set(CMAKE_C_FLAGS   "${OBJECT_GEN_FLAGS} ${COMPILER_FLAGS}" CACHE INTERNAL "C Compiler Flags")
	set(CMAKE_CXX_FLAGS "${OBJECT_GEN_FLAGS} ${COMPILER_FLAGS} -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics" CACHE INTERNAL "C++ Compiler Flags")
	set(CMAKE_ASM_FLAGS "${OBJECT_GEN_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "ASM Compiler Flags")

	set(CMAKE_EXE_LINKER_FLAGS 
		"-T${CMAKE_CURRENT_LIST_DIR}/devices/${LINKER_SCRIPT} --specs=nano.specs --specs=nosys.specs -flto -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections -mabi=aapcs -Wl,--print-memory-usage" 
		CACHE INTERNAL "Linker options")

	# 5. Команда для автоматической генерации файлов прошивки (.bin и .hex) после сборки ELF
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.hex
		COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.bin
		COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
        COMMENT "Generating .hex, .bin and printing memory size..." 
		VERBATIM
    )

	install(FILES 
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.bin"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.hex"
		DESTINATION bin
		COMPONENT app
	)

elseif (${PRESET_DEFINE_MACRO} STREQUAL "__WIN__")

	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup") # /SUBSYSTEM:WINDOWS")
	#add_library("${PROJECT_NAME}" SHARED "${CPP_COURCES}")

	#target_link_libraries("${PROJECT_NAME}" PRIVATE STM32F4_Libraries)

endif ()

if(DEFINED ENV{LETO_PATH})
    message(STATUS "[${PROJECT_NAME}] Найдена переменная окружения LETO_PATH. Подключаем готовые артефакты...")

	if("${PRESET_DEFINE_MACRO}" STREQUAL "__STM32__")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
			"$ENV{LETO_PATH}/Apps/${CMAKE_PRESET_NAME}/${PROJECT_NAME}/"
		)
	else()
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory
			"$ENV{LETO_PATH}/Apps/${CMAKE_PRESET_NAME}/${PROJECT_NAME}/"
			COMMAND ${CMAKE_COMMAND} -E copy
			"$<TARGET_FILE:${PROJECT_NAME}>"
			"$ENV{LETO_PATH}/Apps/${CMAKE_PRESET_NAME}/${PROJECT_NAME}/"
		)
	endif()

	set(LIB_DIR "$ENV{LETO_PATH}/Common/${CORE_BUILD_PRESET}/lib")
	set(BIN_DIR "$ENV{LETO_PATH}/Common/${CORE_BUILD_PRESET}/bin")

    if(WIN32)
		set(LIB_PREFIX "")
        set(LIB_EXT "lib")

		add_library(LetoAPI SHARED IMPORTED GLOBAL)
		set_target_properties(LetoAPI PROPERTIES
			IMPORTED_IMPLIB "${LIB_DIR}/${LIB_PREFIX}LetoAPI.${LIB_EXT}"
			IMPORTED_LOCATION "${BIN_DIR}/LetoAPI.dll"
			INTERFACE_INCLUDE_DIRECTORIES "$ENV{LETO_PATH}/LetoAPI/include"
		)

		add_library(LetoCore SHARED IMPORTED GLOBAL)
		set_target_properties(LetoCore PROPERTIES
			IMPORTED_IMPLIB "${LIB_DIR}/${LIB_PREFIX}LetoCore.${LIB_EXT}"
			IMPORTED_LOCATION "${BIN_DIR}/LetoCore.dll"
			INTERFACE_INCLUDE_DIRECTORIES "$ENV{LETO_PATH}/LetoCore/include"
			INTERFACE_COMPILE_DEFINITIONS "LETOCORE_SHARED" # макрос привязан к библиотеке!
		)
    else()
        set(LIB_PREFIX "lib")
        set(LIB_EXT "a")

		add_library(LetoAPI STATIC IMPORTED GLOBAL)
		set_target_properties(LetoAPI PROPERTIES
			IMPORTED_LOCATION "${LIB_DIR}/${LIB_PREFIX}LetoAPI.${LIB_EXT}"
			INTERFACE_INCLUDE_DIRECTORIES "$ENV{LETO_PATH}/LetoAPI/include"
		)

		add_library(LetoCore STATIC IMPORTED GLOBAL)
		set_target_properties(LetoCore PROPERTIES
			IMPORTED_LOCATION "${LIB_DIR}/${LIB_PREFIX}LetoCore.${LIB_EXT}"
			INTERFACE_INCLUDE_DIRECTORIES "$ENV{LETO_PATH}/LetoCore/include"
		)
    endif()
else()
    message(STATUS "[${PROJECT_NAME}] Переменная окружения LETO_PATH не найдена. Серверная сборка через FetchContent...")

	include(FetchContent)
    FetchContent_Declare(
        LetoCore
        GIT_REPOSITORY https://github.com/leto-console/LetoCore
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(LetoCore)
endif()

target_link_libraries("${PROJECT_NAME}" PUBLIC LetoAPI LetoCore)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
	COMPONENT app
    LIBRARY DESTINATION lib
	COMPONENT app
)
