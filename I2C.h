#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

// Inicializa el bus I2C (TWI) a 50 kHz (modo maestro)
void I2C(void);

// Genera una condición de START. Retorna 1 si OK, o código de error.
uint8_t I2C_Start(void);

// Genera una condición de STOP.
void I2C_Stop(void);

// Escribe un byte (dirección o dato). Retorna 1 si ACK recibido.
uint8_t I2C_Write(uint8_t dato);

// Lee un byte. Si 'ack' = 1 envía ACK; si 0 envía NACK. Retorna 1 si OK.
uint8_t I2C_Read(uint8_t *dato, uint8_t ack);

#endif
