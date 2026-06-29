/*  
 * ------------------------------------------------------------
 *  Autor:          Gustavo Ruiz Luis
 *  Archivo:        SPI.h
 *  Descripcion:    Comunicacion SPI (maestro).
 * ------------------------------------------------------------
 */

#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

/*
 *  @brief          Inicializa SPI como maestro, doble velocidad (500 kHz a 1 MHz).
 */
void SPI(void);

/*
 *  @brief          Envía un byte y recibe otro.
 *  @param dato     Byte a transmitir.
 */
void SPI_Enviar(uint8_t dato);

#endif
