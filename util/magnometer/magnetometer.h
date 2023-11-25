#pragma once

#include <stdio.h>
#include <math.h>
#include "common.h"
#include <stdint.h>

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

// Function declarations
void readAccelerometerData(int16_t* x, int16_t* y, int16_t* z);
void readMagnetometerData(int16_t* x, int16_t* y, int16_t* z);
void calculateAcceleration(int16_t x, int16_t y, int16_t z);
double getCompassBearing(int16_t x, int16_t y);
uint8_t readRegister(uint8_t address, uint8_t reg);

void initMagnetometer();
void getMagnetometerTimeout(bool *result);
void setMagnetometerTimeout(bool value);
