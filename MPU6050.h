/*  
 * ------------------------------------------------------------
 *  Autor:          Gustavo Ruiz Luis
 *  Archivo:        MPU6050.h
 *  Descripcion:    Libreria para el MPU6050
 * ------------------------------------------------------------
 */

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

/*
 *  @brief         Inicializa el MPU6050 e I2C (despierta del modo sleep).
 */
void MPU6050(void);

// Lee aceleración en eje X (raw, ±2g → 16384 LSB/g)
/*
 *  @brief          Lee aceleracion en eje X (raw, ±2g → 16384 LSB/g).
 *  @return         Aceleracion del eje X.
 */
int16_t MPU6050_ReadX(void);

/*
 *  @brief          Lee aceleracion en eje Y.
 *  @return         Aceleracion del eje Y.
 */
int16_t MPU6050_ReadY(void);

/*
 *  @brief          Lee aceleracion en eje Z.
 *  @return         Aceleracion del eje Z.
 */
int16_t MPU6050_ReadZ(void);

/*
 *  @brief          Lee los tres ejes de una sola vez (mas eficiente).
 *  @param *ax      Aceleracion del eje X.
 *  @param *ay      Aceleracion del eje Y.
 *  @param *az      Aceleracion del eje Z.
 */
void MPU6050_ReadAll(int16_t *ax, int16_t *ay, int16_t *az);

#endif
