//This file contains all the common variables and 
//constants used in the project
#ifndef COMMON_H   
#define COMMON_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

// Pin Definitions for Ultrasonic Sensor (ultrasonic.h)
#define ULTRASONIC_ECHO_PIN 0
#define ULTRASONIC_TRIG_PIN 1

// Pin Definitions for Infrared Sensors (sensor.h)
#define BARCODE_SENSOR 26  // Barcode sensor (UNUSED)
#define LEFT_IR_SENSOR 12  // Left infrared sensor
#define RIGHT_IR_SENSOR 13 // Right infrared sensor

// Pin Definitions for Motor Controller (motor_controller.h)
#define CLK_CYCLE_NO 12500  // Clock cycle number

// Motor Control Pin Configuration
#define LEFT_MOTOR 6          // Blue wire, ENA
#define L_REVERSE_PIN 4       // Orange wire, IN2
#define L_CLOCKWISE_PIN 5     // Yellow wire, IN1

#define RIGHT_MOTOR 7         // White wire, ENB
#define R_REVERSE_PIN 2       // Grey wire, IN3
#define R_CLOCKWISE_PIN 3     // Black wire -> Purple, IN4 

#define LEFT_CHANNEL PWM_CHAN_A // PWM Channel for the left motor

// Pin Definitions for Wheel Encoders (wheel_encoder.h)
#define WHEEL_CIRCUM 20.4    // Wheel circumference
#define DISTANCE_STATE 1.02   // Distance state for wheel encoder

#define L_WHEEL_ENCODER 18    // Left wheel encoder
#define R_WHEEL_ENCODER 19    // Right wheel encoder

// Pin Definitions for Magnetometer (magnetometer.h)
#define I2C_PORT i2c1

#define ACC_ADDRESS 0x19
#define MAG_ADDRESS 0x1E

#define CTRL_REG1_A 0x20

#define OUT_X_L_A 0x28
#define OUT_X_H_A 0x29
#define OUT_Y_L_A 0x2A
#define OUT_Y_H_A 0x2B
#define OUT_Z_L_A 0x2C
#define OUT_Z_H_A 0x2D

#define CRA_REG_M 0x00
#define MR_REG_M 0x02
#define OUT_X_H_M 0x03
#define OUT_X_L_M 0x04
#define OUT_Z_H_M 0x05
#define OUT_Z_L_M 0x06
#define OUT_Y_H_M 0x07
#define OUT_Y_L_M 0x08

#define PIN_SDA 14
#define PIN_SCL 15

#endif // COMMON_H
