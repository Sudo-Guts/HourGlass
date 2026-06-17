#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

// Inicializa SPI como maestro, doble velocidad (500 kHz a 1 MHz)
void SPI(void);

// Envía un byte y recibe otro (si hay esclavo)
void SPI_Enviar(uint8_t dato);

#endif