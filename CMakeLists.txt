include(FreeRTOS_Kernel_import.cmake)


if (NOT PICO_NO_HARDWARE)
    add_subdirectory(driver)
endif ()
