add_subdirectory(magnometer)
add_subdirectory(wifi_client)
add_subdirectory(ultrasonic)
add_subdirectory(irline)
add_subdirectory(motor)
add_subdirectory(encoder)
add_subdirectory(barcode)
add_subdirectory(motor_encoder_barcode)



set(WIFI_SSID "Samuel" CACHE INTERNAL "WiFi SSID for examples")
set(WIFI_PASSWORD "100011001" CACHE INTERNAL "WiFi password for examples")
set(TEST_TCP_SERVER_IP "172.20.10.2" CACHE INTERNAL "Ip address")
include(FreeRTOS_Kernel_import.cmake)