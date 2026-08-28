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
	if(DEFINED ENV{LETO_PATH})
		target_compile_definitions(${PROJECT_NAME} PRIVATE LETOCORE_SHARED)
	endif()
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
	set(COMPILER_FLAGS "-ffunction-sections -fdata-sections -Wall -fstack-usage")
	# -fcyclomatic-complexity - Используется только в STM32CubeIDE

	# Флаги компиляции для C / C++ / ASM
	set(CMAKE_C_FLAGS   "${OBJECT_GEN_FLAGS} ${COMPILER_FLAGS}" CACHE INTERNAL "C Compiler Flags")
	set(CMAKE_CXX_FLAGS "${OBJECT_GEN_FLAGS} ${COMPILER_FLAGS} -fno-exceptions -fno-rtti -fno-use-cxa-atexit" CACHE INTERNAL "C++ Compiler Flags")
	set(CMAKE_ASM_FLAGS "${OBJECT_GEN_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "ASM Compiler Flags")

	set(CMAKE_EXE_LINKER_FLAGS 
		"-T${CMAKE_CURRENT_LIST_DIR}/devices/${LINKER_SCRIPT} --specs=nano.specs --specs=nosys.specs -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections -mabi=aapcs -Wl,--print-memory-usage" 
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

elseif (${PRESET_DEFINE_MACRO} STREQUAL "__LINUX__")

	#find_package(Qt6 COMPONENTS Widgets REQUIRED Core)
	#qt_standard_project_setup()
	#add_executable ("${PROJECT_NAME}" "Main/App.cpp" "${CPP_COURCES}")
	#target_link_libraries("${PROJECT_NAME}" PRIVATE Qt6::Core Qt6::Widgets)

elseif (${PRESET_DEFINE_MACRO} STREQUAL "__TERMUX__")

	#add_library("${PROJECT_NAME}" SHARED "${CPP_COURCES}")

	#target_link_libraries("${PROJECT_NAME}" PRIVATE STM32F4_Libraries)
	set_target_properties("${PROJECT_NAME}" PROPERTIES POSITION_INDEPENDENT_CODE ON)

endif ()

if(DEFINED ENV{LETO_PATH})
    message(STATUS "[${PROJECT_NAME}] Найдена переменная окружения LETO_PATH. Подключаем готовые артефакты...")

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
		"$ENV{LETO_PATH}/leto-console/apps/${CMAKE_PRESET_NAME}/${PROJECT_NAME}/"
	)

	target_link_directories(${PROJECT_NAME} PUBLIC "$ENV{LETO_PATH}/LetoAPI/lib/${CORE_BUILD_PRESET}")
	target_link_directories(${PROJECT_NAME} PUBLIC "$ENV{LETO_PATH}/LetoCore/lib/${CORE_BUILD_PRESET}")

	target_include_directories(${PROJECT_NAME} PUBLIC "$ENV{LETO_PATH}/LetoAPI/include")
	target_include_directories(${PROJECT_NAME} PUBLIC "$ENV{LETO_PATH}/LetoCore/include")
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

target_link_libraries("${PROJECT_NAME}" PUBLIC LetoAPI)
target_link_libraries("${PROJECT_NAME}" PUBLIC LetoCore)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
	COMPONENT app
)
