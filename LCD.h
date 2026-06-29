/*  
 * ------------------------------------------------------------
 *  Autor:          Gustavo Ruiz Luis
 *  Archivo:        LCD.h
 *  Descripcion:    Driver para LCD 16x2 en modo 4 bits.
 * ------------------------------------------------------------
 */

#ifndef LCD_H
#define LCD_H

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#define LCD_PORT PORTD

/*
 *  @brief          Inicializa el LCD en modo 4 bits.
 */
void LCD(void);

/*
 *  @brief          Envia una instruccion de 8 bits.
 *  @param inst     Byte a transmitir.
 */
void LCD_Inst(uint8_t inst);

/*
 *  @brief          Escribe un caracter o dato.
 *  @param data     Dato a transmitir.
 */
void LCD_Data(uint8_t data);

/*
 *  @brief          Limpiar la pantalla.
 */
void LCD_Clear(void);

/*
 *  @brief          Posiciona cursor.
 *  @param pos      nibble alto = renglón (0x00 o 0x10), bajo = columna.
 */
void LCD_Goto(uint8_t pos);

/*
 *  @brief          Escribe una cadena. 
 *  @param cad      Cadena
 *  a transmitir.
 *  @param tam      Longitud de la cadena.
 */
void LCD_Write(char cad[], uint8_t tam);

#endif
