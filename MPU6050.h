#ifndef MPU6050_H
#define MPU6050_H

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "I2C.h"

#define MPU6050_ADDR_W 0xD0
#define MPU6050_ADDR_R 0xD1
#define PWR_MGMT_1     0x6B
#define ACCEL_XOUT_H   0x3B

// Inicializa el MPU6050 (despierta del modo sleep)
void MPU6050(void);

// Lee aceleración en eje X (raw, ±2g → 16384 LSB/g)
int16_t MPU6050_ReadX(void);

// Lee aceleración en eje Y
int16_t MPU6050_ReadY(void);

// Lee aceleración en eje Z
int16_t MPU6050_ReadZ(void);

// Lee los tres ejes de una sola vez (más eficiente)
void MPU6050_ReadAll(int16_t *ax, int16_t *ay, int16_t *az);

#endif