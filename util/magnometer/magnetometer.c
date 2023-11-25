#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "magnetometer.h"

// Global variables for accelerometer bias and timeout status
int16_t accelerometerBiasX = 0;
int16_t accelerometerBiasY = 0;
int16_t accelerometerBiasZ = 0;
volatile bool magnetometerTimeoutReceived = false;

// Function to initialize I2C communication
void initializeI2C() {
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    i2c_set_slave_mode(I2C_PORT, false, 0);
}

// Function to write a value to a register on the I2C device
void writeI2CRegister(uint8_t address, uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_write_blocking(I2C_PORT, address, data, 2, true);
}

// Function to read a value from a register on the I2C device
uint8_t readI2CRegister(uint8_t address, uint8_t reg) {
    uint8_t data;
    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, address, &data, 1, false);
    return data;
}

// Function to configure the magnetometer
void configureMagnetometer() {
    writeI2CRegister(MAG_ADDRESS, MR_REG_M, CRA_REG_M);
}

// Function to read magnetometer data
void readMagnetometerData(int16_t* x, int16_t* y, int16_t* z) {
    *x = (int16_t)((readI2CRegister(MAG_ADDRESS, OUT_X_H_M) << 8) | readI2CRegister(MAG_ADDRESS, OUT_X_L_M));
    *y = (int16_t)((readI2CRegister(MAG_ADDRESS, OUT_Y_H_M) << 8) | readI2CRegister(MAG_ADDRESS, OUT_Y_L_M));
    *z = (int16_t)((readI2CRegister(MAG_ADDRESS, OUT_Z_H_M) << 8) | readI2CRegister(MAG_ADDRESS, OUT_Z_L_M));

    // Check for timeout conditions
    if ((*x == -16192 && *y == -16192 && *z == -16192) || 
        (*x == 16448 && *y == 16448 && *z == 16448) || 
        (*x == -32640 && *y == -32640 && *z == -32640) || 
        (*x == 0 && *y == 0 && *z == 0)) {
        magnetometerTimeoutReceived = true;
    }
}

// Function to read accelerometer data
void readAccelerometerData(int16_t* x, int16_t* y, int16_t* z) {
    *x = (int16_t)((readI2CRegister(ACC_ADDRESS, OUT_X_H_A) << 8) | readI2CRegister(ACC_ADDRESS, OUT_X_L_A)) - accelerometerBiasX;
    *y = (int16_t)((readI2CRegister(ACC_ADDRESS, OUT_Y_H_A) << 8) | readI2CRegister(ACC_ADDRESS, OUT_Y_L_A)) - accelerometerBiasY;
    *z = (int16_t)((readI2CRegister(ACC_ADDRESS, OUT_Z_H_A) << 8) | readI2CRegister(ACC_ADDRESS, OUT_Z_L_A)) - accelerometerBiasZ;
}

// Function to configure the accelerometer
void configureAccelerometer() {
    writeI2CRegister(ACC_ADDRESS, CTRL_REG1_A, 0x47);
}

// Function to calibrate the accelerometer
void calibrateAccelerometer() {
    const int numSamples = 100;  
    int16_t xAccum = 0;
    int16_t yAccum = 0;
    int16_t zAccum = 0;

    // Accumulate accelerometer readings over multiple samples
    for (int i = 0; i < numSamples; i++) {
        int16_t x, y, z;
        readAccelerometerData(&x, &y, &z);
        xAccum += x;
        yAccum += y;
        zAccum += z;
        sleep_ms(10);  
    }

    // Calculate average bias values
    accelerometerBiasX = xAccum / numSamples;
    accelerometerBiasY = yAccum / numSamples;
    accelerometerBiasZ = zAccum / numSamples;
}

// Function to calculate acceleration from accelerometer readings
void calculateAcceleration(int16_t x, int16_t y, int16_t z) {
    double gravity = 9.81; // Standard gravity in m/s^2
    double accX = (double)x * (gravity / 16384.0);
    double accY = (double)y * (gravity / 16384.0);
    double accZ = (double)z * (gravity / 16384.0);

    printf("Acceleration: (X = %.2f m/s^2, Y = %.2f m/s^2, Z = %.2f m/s^2)\n", accX, accY, accZ);
}

// Function to get compass bearing from magnetometer readings
double getCompassBearing(int16_t x, int16_t y) {
    double angle = atan2((double)y, (double)x);
    if (angle < 0) {
        angle += 2 * M_PI;
    }
    return (angle * 180.0) / M_PI;
}

// Function to initialize the magnetometer
void initializeMagnetometer() {
    initializeI2C();
    configureAccelerometer();
    configureMagnetometer();
    calibrateAccelerometer();
}

// Function to get the magnetometer timeout status
void getMagnetometerTimeout(bool *result) {
    *result = magnetometerTimeoutReceived;
}

// Function to set the magnetometer timeout status
void setMagnetometerTimeout(bool value) {
    magnetometerTimeoutReceived = value;
}

int main() {
    stdio_init_all();
    initializeMagnetometer();

    while (1) {
        int16_t xAcc, yAcc, zAcc;
        int16_t xMag, yMag, zMag;

        // Read accelerometer and magnetometer data if no timeout occurred
        if (!magnetometerTimeoutReceived) {
            readAccelerometerData(&xAcc, &yAcc, &zAcc);
            readMagnetometerData(&xMag, &yMag, &zMag);
            calculateAcceleration(xAcc, yAcc, zAcc);
            printf("Magnetometer Data: (X = %d, Y = %d, Z = %d)\n", xMag, yMag, zMag);

            // Calculate and print compass bearing
            double compassBearing = getCompassBearing(xMag, yMag);
            printf("Compass Bearing: %.2f degrees\n", compassBearing);
        } else {
            printf("Magnetometer read timed out.\n");
            magnetometerTimeoutReceived = false;
        }

        sleep_ms(500);
    }

    return 0;
}
