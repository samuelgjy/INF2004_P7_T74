#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <math.h>

// Define the I2C pins (GP0 and GP8 on a Raspberry Pi Pico)
#define SDA_PIN 0
#define SCL_PIN 1
#define GY_511_ADDRESS 0x1E // Default address for GY-511 magnetometer module

// Register addresses for MPU6050
#define ACC_CTRL_REG1_A 0x19
#define ACCEL_XOUT_H 0x29
#define ACCEL_XOUT_L 0x28
#define ACCEL_YOUT_H 0x2B
#define ACCEL_YOUT_L 0x2A
#define ACCEL_ZOUT_H 0x2D
#define ACCEL_ZOUT_L 0x2C


#define MAG_XOUT_H 0x03
#define MAG_XOUT_L 0x04
#define MAG_YOUT_H 0x05
#define MAG_YOUT_L 0x06
#define MAG_ZOUT_H 0x07
#define MAG_ZOUT_L 0x08
#define I2C_ADDRESS 0x1E
#define I2C_ADDRESS_ACCEL 0x68
#define I2C_ADDRESS_MAG 0x3C

int16_t read_accel(int16_t *data, uint8_t reg) {
  uint8_t buffer[2];

  i2c_write_blocking(i2c0, 0x19, &reg, 1, true);
  i2c_read_blocking(i2c0, 0x19, buffer, 2, false);

  *data = (int16_t)((buffer[1] << 8) | buffer[0]);

  return 0;
}

float calculate_heading(int16_t mag_x, int16_t mag_y) {
  float heading = atan2(mag_y, mag_x) * 180.0 / M_PI;
  if (heading < 0) {
    heading += 360.0;
  }
  return heading;
}

int16_t read_mag(int16_t *data, uint8_t reg) {
    uint8_t buffer[2];
    i2c_write_blocking(i2c0, I2C_ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c0, I2C_ADDRESS, buffer, 2, false);
    *data = (int16_t)((buffer[1] << 8) | buffer[0]);
    return 0;
}

int main() {
  stdio_init_all();
  i2c_init(i2c0, 115200); // Initialize I2C at 100kHz
  gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
  i2c_set_baudrate(i2c0, 115200);

  // Enable continuous measurement mode for the accelerometer
  uint8_t accelerometer_config_data[] = {0x20, 0x27};
  i2c_write_blocking(i2c0, 0x19, accelerometer_config_data, sizeof(accelerometer_config_data), false);

  // Data to configure the GY-511 module for continuous measurement
  uint8_t magnetometer_config_data[] = {0x02, 0x00};
  i2c_write_blocking(i2c0, GY_511_ADDRESS, magnetometer_config_data, sizeof(magnetometer_config_data), false);

  int16_t accel_x, accel_y, accel_z;
  int16_t mag_x, mag_y, mag_z;

  while (1) {
    read_accel(&accel_x, ACCEL_XOUT_H);
    read_accel(&accel_y, ACCEL_YOUT_H);
    read_accel(&accel_z, ACCEL_ZOUT_H);

    read_mag(&mag_x, MAG_XOUT_L);
    read_mag(&mag_y, MAG_YOUT_L);
    read_mag(&mag_z, MAG_ZOUT_L);

    printf("Accel: X=%-8d, Y=%-8d, Z=%-8d\t", accel_x, accel_y, accel_z);
    // printf("Magnetometer: X=%-8d, Y=%-8d, Z=%-8d\n", mag_x, mag_y, mag_z);
    float heading = calculate_heading(mag_x, mag_y);

    printf("Magnetometer - X: %d uT, Y: %d uT, Z: %d uT, Heading: %.2f°\n", mag_x, mag_y, mag_z, heading);

    // Add a delay or sleep if necessary
    sleep_ms(500); // Sleep for 1 second before the next reading
  }

  return 0;
}