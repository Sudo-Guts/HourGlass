/*  
 * ------------------------------------------------------------
 *  Autor:          Gustavo Ruiz Luis
 *  Archivo:        SPI.c
 *  Descripcion:    Comunicacion SPI (maestro).
 * ------------------------------------------------------------
 */

#include "SPI.h"

void SPI(void) {
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
    PORTB |= (1 << PB2);    // SS inactivo
    SPCR = (1 << SPE) | (1 << MSTR);
    SPSR = (1 << SPI2X);    // doble velocidad
}

void SPI_Enviar(uint8_t dato) {
    SPDR = dato;
    while (!(SPSR & (1 << SPIF)));
    (void)SPDR;             // limpiar bandera
}
