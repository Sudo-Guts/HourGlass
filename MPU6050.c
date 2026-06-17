
#include "MPU6050.h"

void MPU6050(void) {
    I2C_Start();
    I2C_Write(MPU6050_ADDR_W);
    I2C_Write(PWR_MGMT_1);
    I2C_Write(0x00);      // despertar
    I2C_Stop();
    _delay_ms(10);
}

int16_t MPU6050_ReadX(void) {
    uint8_t high, low;
    I2C_Start();
    I2C_Write(MPU6050_ADDR_W);
    I2C_Write(ACCEL_XOUT_H);
    I2C_Start();
    I2C_Write(MPU6050_ADDR_R);
    I2C_Read(&high, 1);
    I2C_Read(&low, 0);
    I2C_Stop();
    return (int16_t)((high << 8) | low);
}

int16_t MPU6050_ReadY(void) {
    uint8_t high, low;
    I2C_Start();
    I2C_Write(MPU6050_ADDR_W);
    I2C_Write(ACCEL_XOUT_H + 2);
    I2C_Start();
    I2C_Write(MPU6050_ADDR_R);
    I2C_Read(&high, 1);
    I2C_Read(&low, 0);
    I2C_Stop();
    return (int16_t)((high << 8) | low);
}

int16_t MPU6050_ReadZ(void) {
    uint8_t high, low;
    I2C_Start();
    I2C_Write(MPU6050_ADDR_W);
    I2C_Write(ACCEL_XOUT_H + 4);
    I2C_Start();
    I2C_Write(MPU6050_ADDR_R);
    I2C_Read(&high, 1);
    I2C_Read(&low, 0);
    I2C_Stop();
    return (int16_t)((high << 8) | low);
}

void MPU6050_ReadAll(int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t buf[6];
    I2C_Start();
    I2C_Write(MPU6050_ADDR_W);
    I2C_Write(ACCEL_XOUT_H);
    I2C_Start();
    I2C_Write(MPU6050_ADDR_R);
    for (uint8_t i = 0; i < 5; i++)
        I2C_Read(&buf[i], 1);
    I2C_Read(&buf[5], 0);
    I2C_Stop();
    *ax = (int16_t)((buf[0] << 8) | buf[1]);
    *ay = (int16_t)((buf[2] << 8) | buf[3]);
    *az = (int16_t)((buf[4] << 8) | buf[5]);
}