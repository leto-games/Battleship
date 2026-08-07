# ==============================================================================
# CMake Toolchain файл для STM32 (Cortex-M4F)
# ==============================================================================

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# ------------------------------------------------------------------------------
# ПОИСК ТУЛЧЕЙНА В СИСТЕМЕ (PATH)
# ------------------------------------------------------------------------------
# Передаем CMake только имена файлов. Он сам найдет их в PATH и подставит .exe на Windows.
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR           arm-none-eabi-gcc-ar)

# Для утилит используем поиск find_program, чтобы CMake зафиксировал их полные пути
find_program(CMAKE_OBJCOPY NAMES arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_OBJDUMP NAMES arm-none-eabi-objdump REQUIRED)
find_program(CMAKE_SIZE    NAMES arm-none-eabi-size    REQUIRED)
find_program(CMAKE_READELF NAMES arm-none-eabi-readelf REQUIRED)

# Отключаем проверку компилятора сборкой бинарника
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ------------------------------------------------------------------------------
# Конфигурация типов сборки (Debug / Release / MinSizeRel)
# ------------------------------------------------------------------------------
# (Ниже оставляем всё без изменений, как в вашем файле...)
set(CMAKE_C_FLAGS_DEBUG "-g3 -Os" CACHE INTERNAL "C Debug Flags")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -Os" CACHE INTERNAL "C++ Debug Flags")

set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG" CACHE INTERNAL "C Release Flags")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG" CACHE INTERNAL "C++ Release Flags")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
