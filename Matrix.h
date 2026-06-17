#ifndef MATRIX_H
#define MATRIX_H

#include <avr/io.h>
#include <util/delay.h>
#include "SPI.h"

// Códigos MAX7219
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

// Inicializa dos matrices MAX7219 en cascada
void Matrix(void);

// Envía comando a una matriz específica (0=superior, 1=inferior)
void Matrix_Write(uint8_t op, uint8_t data, uint8_t matriz);

// Refresca el buffer en las matrices
void Matrix_Refresh(void);

// Enciende (1) o apaga (0) un píxel en (x, y): x=0..7, y=0..15
void Matrix_Pixel(uint8_t x, uint8_t y, uint8_t estado);

// Limpia toda la pantalla
void Matrix_Clear(void);

// Lee el estado de un píxel del buffer interno (1=encendido, 0=apagado)
// x=0..7, y=0..15
uint8_t Matrix_GetPixel(uint8_t x, uint8_t y);

// Animación de ejemplo
void Matrix_Animacion(uint8_t isUp);

//Animación de caída de partículas (isUp=1 para caída normal, 0 para caída invertida)
void Matrix_Caida(uint8_t matriz_inferior);

#endif