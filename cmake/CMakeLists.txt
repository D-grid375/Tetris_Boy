cmake_minimum_required(VERSION 3.12)

# ★ これを先に置くと、CMakeの試しビルドが「静的ライブラリ」になり、クロス環境でも安全
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ---- Pico (組み込み) ビルド ----
include(pico_sdk_import.cmake)  # ★ project() より前に読む
project(my_project C CXX ASM)
pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_executable(my_project
    ../src/main.c
    ../src/app/config/config.c
    ../src/app/tetris/tetris_main.c
    ../src/app/tetris/tetris_input_ctrl.c
    ../src/app/tetris/tetris_data_compute.c
    ../src/app/tetris/tetris_display_ctrl.c
    ../src/app/tetris/tetris_const_bitmap.c
    ../src/app/tetris/tetris_debug_cmd_def.c
    ../src/app/tetris/tetris_debug_ctrl.c
    ../src/mid/analogStick/analogStick_ops.c
    ../src/mid/analogStick/analogStick_init.c
    ../src/mid/button/button_ops.c
    ../src/mid/button/button_init.c
    ../src/mid/debug_com/debug_com_init.c
    ../src/mid/debug_com/debug_com_ops.c
    ../src/mid/SH1107/SH1107_ops.c
    ../src/mid/SH1107/SH1107_init.c
    ../src/mid/SH1107/SH1107_ctrl.c
    ../src/drv/adc/adc_ops.c
    ../src/drv/adc/adc_ctrl.c
    ../src/drv/adc/adc_init.c
    ../src/drv/gpio/gpio_ops.c
    ../src/drv/gpio/gpio_init.c
    ../src/drv/interrupt/interrupt_ctrl.c
    ../src/drv/interrupt/interrupt_init.c
    ../src/drv/timer/timer_ops.c
    ../src/drv/timer/timer_init.c
    ../src/drv/I2C/I2C_ctrl.c
    ../src/drv/I2C/I2C_ops.c
    ../src/drv/I2C/I2C_init.c
    ../src/common/lib/bitmap/bitmap_lib.c
    ../src/common/lib/math/math_lib.c
)

target_include_directories(my_project PRIVATE
    ../src/app
    ../src/app/config
    ../src/app/tetris
    ../src/mid
    ../src/mid/analogStick
    ../src/mid/button
    ../src/mid/debug_com
    ../src/mid/SH1107
    ../src/drv
    ../src/drv/adc
    ../src/drv/gpio
    ../src/drv/timer
    ../src/drv/interrupt
    ../src/drv/I2C
    ../src/drv/include
    ../src/common
    ../src/common/lib
    ../src/common/lib/bitmap
    ../src/common/lib/math
    ../src/common/include
)

target_link_libraries(my_project pico_stdlib)
target_compile_options(my_project PRIVATE -save-temps -fverbose-asm)
pico_add_extra_outputs(my_project)