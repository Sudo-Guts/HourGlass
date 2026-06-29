/*  
 * ------------------------------------------------------------
 *  Autor:          Gustavo Ruiz Luis
 *  Archivo:        I2C.h
 *  Descripcion:    Comunicación I2C (maestro).
 * ------------------------------------------------------------
 */

#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

/*
 *  @brief          Inicializa el bus I2C (TWI) a 50 kHz (modo maestro).
 */
void I2C(void);

/*
 *  @brief          Genera una condicion de START.
 *  @return         1 si OK, o codigo de error.
 */
uint8_t I2C_Start(void);

/*
 *  @brief          Genera una condicion de STOP.
 */
void I2C_Stop(void);

/*
 *  @brief          Escribe un byte (direccion o dato). 
 *  @param dato     Byte a transmitir.
 *  @return         1 si ACK recibido, o codigo de error.
 */
uint8_t I2C_Write(uint8_t dato);

/*
 *  @brief          Lee un byte. 
 *  @param *dato    Byte a recibir.
 *  @param ack      Si 'ack' = 1 envía ACK; si 0 envía NACK.
 *  @return         1 si OK, o codigo de error.
 */
uint8_t I2C_Read(uint8_t *dato, uint8_t ack);

#endif
