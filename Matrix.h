/*  
 * ------------------------------------------------------------
 *  Autor:          Gustavo Ruiz Luis
 *  Archivo:        Matrix.h
 *  Descripcion:    Control de matrices MAX7219 (buffer, refresco)
 * ------------------------------------------------------------
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <avr/io.h>
#include <util/delay.h>
#include "SPI.h"

// Codigos MAX7219
#define OP_NOOP        0x00
#define OP_DIGIT0      0x01
#define OP_DIGIT1      0x02
#define OP_DIGIT2      0x03
#define OP_DIGIT3      0x04
#define OP_DIGIT4      0x05
#define OP_DIGIT5      0x06
#define OP_DIGIT6      0x07
#define OP_DIGIT7      0x08
#define OP_DECODEMODE  0x09
#define OP_INTENSITY   0x0A
#define OP_SCANLIMIT   0x0B
#define OP_SHUTDOWN    0x0C
#define OP_DISPLAYTEST 0x0F

/*
 *  @brief          Inicializa dos matrices MAX7219 en cascada. 
 */
void Matrix(void);

/*
 *  @brief          Envia comando a una matriz especifica. 
 *  @param op       Operacion.
 *  @param data     Byte a transmitir.
 *  @param matriz   (0=superior, 1=inferior).
 */
void Matrix_Write(uint8_t op, uint8_t data, uint8_t matriz);

/*
 *  @brief          Refresca el buffer en las matrices. 
 */
void Matrix_Refresh(void);

/*
 *  @brief          Enciende o apaga un pixel.
 *  @param x        x=0..7.
 *  @param y        y=0..15.
 *  @param estado   1=encendido, 0=apagado.
 */
void Matrix_Pixel(uint8_t x, uint8_t y, uint8_t estado);

/*
 *  @brief          Limpia toda la pantalla. 
 */
void Matrix_Clear(void);

/*
 *  @brief          Lee el estado de un pixel del buffer interno.
 *  @param x        x=0..7.
 *  @param y        y=0..15.
 *  @return         1=encendido, 0=apagado.
 */
uint8_t Matrix_GetPixel(uint8_t x, uint8_t y);

/*
 *  @brief          Animacion de inicio.
 *  @param isUp     1 para matriz inferior, 0 para matriz superior.
 */
void Matrix_Animacion(uint8_t isUp);

/*
 *  @brief          Animacion de caida de particulas.
 *  @param matriz   1 para caida normal, 0 para caida invertida.
 */
void Matrix_Caida(uint8_t matriz_inferior);

#endif
