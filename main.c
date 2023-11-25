/*
Team 74 P7
 */


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "main_lib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware/pwm.h"
#include "math.h"

#include "util/irsensor/sensor.h"
#include "util/map/map.h"
#include "util/wifi/wifi.h"
#include "util/magnetometer/magnetometer.h"
#include "util/ultrasonic/ultrasonic.h"
#include "util/motor_controller/motor_controller.h"
#include "util/wheel_encoder/wheel_encoder.h"
#include "util/pid_controller/pid_controller.h"

// Callback function for GPIO events
void main_callback(unsigned int gpio, long unsigned int events) {
    printf("Callback GPIO: %d\n", gpio);

    // Handling events based on the GPIO
    if (gpio == L_WHEEL_ENCODER) {
        // Left Wheel Encoder Event
        l_triggered += 1;

        if (l_prev_time) {
            l_start_time = time_us_64();
            l_speed = get_dst(l_start_time, l_prev_time, l_triggered);
            printf("Left Wheel Speed: %.2f/s\n", l_speed);
        }
        l_prev_time = time_us_64();
    } else if (gpio == R_WHEEL_ENCODER) {
        // Right Wheel Encoder Event
        r_triggered += 1;

        if (r_prev_time) {
            r_start_time = time_us_64();
            r_speed = get_dst(r_start_time, r_prev_time, r_triggered);
            printf("Right Wheel Speed: %.2f/s\n", r_speed);
        }
        r_prev_time = time_us_64();
    } else if (gpio == ULTRASONIC_ECHO_PIN) {
        // Ultrasonic Sensor Event
        if (gpio_get(ULTRASONIC_ECHO_PIN) == 1) {
            startTime_ultra = get_absolute_time();
        } else {
            endTime_ultra = get_absolute_time();
            echoReceived = true;
        }
    }
}

// Initialization function for various components
void initAll() {
    currMode = DEFAULT;
    currMoveState = STATIONARY;
    leftSensor = rightSensor = false;

    // Initialize sensors
    initSensor(&leftSensor, &rightSensor);
    initMotorController(&leftSliceNum, &rightSliceNum, &direction);
    printf("Left Slice Num: %d\n", leftSliceNum);
    printf("Right Slice Num: %d\n", rightSliceNum);
    initWheelEncoder();
}

// Function to traverse the map
void traverseMap() {
    // To be implemented
    // Traverse the map using DFS, prioritize going left
}

// Default behavior function
void defaultBehaviour() {
    // Read sensor values
    getLeftSensor(&leftSensor);
    getRightSensor(&rightSensor);

    // Decide behavior based on sensor readings
    if (leftSensor && rightSensor) {
        // Obstacle in front, turn left
        currMoveState = TURN_LEFT;
        currMode = PERFORMING_TASK;
    } else if (leftSensor) {
        // Obstacle on the left, turn right
        currMoveState = TURN_RIGHT;
        currMode = PERFORMING_TASK;
    } else if (rightSensor) {
        // Obstacle on the right, turn left
        currMoveState = TURN_LEFT;
        currMode = PERFORMING_TASK;
    } else {
        // No obstacle, move forward
        currMoveState = FORWARD;
        currMode = PERFORMING_TASK;
    }
}

// Function to update the behavior based on the current mode
void updateBehaviour() {
    switch (currMode) {
        case DEFAULT:
            defaultBehaviour();
            break;
        case TRAVERSE_MAP:
            traverseMap();
            break;
        case PERFORMING_TASK:
            defaultBehaviour(); // DEBUG LINE
            break;
        default:
            break;
    }
}

// Function to update the movement based on the current move state
void updateMovement() {
    switch (currMoveState) {
        case STATIONARY:
            // No movement
            break;
        case FORWARD:
            // Move forward
            move_forward();
            break;
        case BACKWARD:
            // Move backward
            reverse();
            break;
        case TURN_LEFT:
            // Turn left
            turn_left(leftSliceNum, rightSliceNum, direction);
            break;
        case TURN_RIGHT:
            // Turn right
            turn_right(leftSliceNum, rightSliceNum, direction);
            break;
        default:
            break;
    }
}

// Task launching function
void vLaunch(void) {
    TaskHandle_t sensor_task;
    xTaskCreate(sensorTask, "TestMainThread", configMINIMAL_STACK_SIZE, NULL, 5, &sensor_task);

    vTaskStartScheduler();
}

int main() {
    // Initialize standard I/O
    stdio_init_all();

    // Initial delay for system startup
    sleep_ms(5000);
    printf("Starting...\n");

    // Initialize components
    initAll();

    // Delay after initialization
    sleep_ms(2000);
    printf("Init Passed...\n");

    // Enable interrupts for wheel encoders and ultrasonic sensor
    gpio_set_irq_enabled_with_callback(L_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE, true, &main_callback);
    gpio_set_irq_enabled_with_callback(R_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE, true, &main_callback);
    gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &main_callback);

    // Start moving forward
    move_forward();

    // Timing variables for frame rate control
    absolute_time_t frame_start;
    absolute_time_t frame_end;
    uint16_t frame_duration;
    float fps = 10;
    float frame_time = 1000 / fps;
    float dt = frame_time / 1000;
    printf("Frame Time: %f\n", frame_time);

    // Short delay before launching tasks
    sleep_ms(100);
    vLaunch();

    // Main loop
    while (true) {
        // Record start time of the frame
        frame_start = get_absolute_time();

        // Update behavior and movement based on sensor inputs
        updateBehaviour();
        updateMovement();

        // Additional logic...

        // Record end time of the frame
        frame_end = get_absolute_time();
        frame_duration = absolute_time_diff_us(frame_start, frame_end);

        // Sleep to control frame rate
        printf("Sleep Duration: %f\n", frame_time - frame_duration / 1000);
        sleep_ms(frame_time - frame_duration / 1000);
    }
}
