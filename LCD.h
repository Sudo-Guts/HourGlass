#ifndef LCD_H
#define LCD_H

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#define LCD_PORT PORTD

// Inicializa el LCD en modo 4 bits
void LCD(void);

// Envía una instrucción de 8 bits
void LCD_Inst(uint8_t inst);

// Escribe un carácter o dato
void LCD_Data(uint8_t data);

// Limpia la pantalla
void LCD_Clear(void);

// Posiciona cursor: nibble alto = renglón (0x00 o 0x10), bajo = columna
void LCD_Goto(uint8_t pos);

// Escribe una cadena de longitud 'tam'
void LCD_Write(char cad[], uint8_t tam);

#endif