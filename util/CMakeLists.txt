if (NOT PICO_NO_HARDWARE)
    add_subdirectory(motor_controller)
    add_subdirectory(wheel_encoder)
    add_subdirectory(ultrasonic)
    add_subdirectory(magnetometer)
    add_subdirectory(irsensor)
    add_subdirectory(map)
    add_subdirectory(wifi)
    add_subdirectory(pid_controller)
endif ()
