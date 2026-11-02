cmake_minimum_required(VERSION 3.22)

# CMAKE_SYSTEM_NAME is supplied by the CLion CMake profile before this file is loaded.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

project(ThreeLedFreeRTOS C ASM)
set(CMAKE_C_STANDARD 11)

add_compile_options(-mcpu=cortex-m3 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Ofast)
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-Ofast -g)
elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_compile_options(-Os)
else ()
    add_compile_options(-Og -g)
endif ()

set(MCU_DEFINE STM32F103xB)
set(HAL Drivers/STM32F1xx_HAL_Driver)
set(CMSIS Drivers/CMSIS)
set(FREERTOS Middlewares/Third_Party/FreeRTOS/Source)

add_executable(${PROJECT_NAME}.elf
    Core/Src/main.c
    Core/Src/freertos_app.c
    Core/Src/stm32f1xx_it.c
    Core/Src/system_stm32f1xx.c
    Core/Src/startup_stm32f103xb.s
    ${HAL}/Src/stm32f1xx_hal.c
    ${HAL}/Src/stm32f1xx_hal_cortex.c
    ${HAL}/Src/stm32f1xx_hal_dma.c
    ${HAL}/Src/stm32f1xx_hal_flash.c
    ${HAL}/Src/stm32f1xx_hal_gpio.c
    ${HAL}/Src/stm32f1xx_hal_rcc.c
    ${HAL}/Src/stm32f1xx_hal_tim.c
    ${HAL}/Src/stm32f1xx_hal_tim_ex.c
    ${FREERTOS}/tasks.c
    ${FREERTOS}/queue.c
    ${FREERTOS}/list.c
    ${FREERTOS}/portable/GCC/port.c
    ${FREERTOS}/portable/MemMang/heap_4.c
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE DEBUG USE_HAL_DRIVER ${MCU_DEFINE})
target_include_directories(${PROJECT_NAME}.elf PRIVATE
    Core/Inc
    ${HAL}/Inc/Legacy
    ${HAL}/Inc
    ${CMSIS}/Device/ST/STM32F1xx/Include
    ${CMSIS}/Include
    ${FREERTOS}/include
    ${FREERTOS}/portable/GCC
)
target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -ffunction-sections -fdata-sections -Wall -Wextra
    -Wno-unused-parameter -Wno-missing-field-initializers
)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F103C8Tx_FLASH.ld)
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
    -mcpu=cortex-m3 -mthumb -T${LINKER_SCRIPT}
)
target_link_libraries(${PROJECT_NAME}.elf PRIVATE c m)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
    COMMENT "Building ${HEX_FILE} and ${BIN_FILE}"
)
