/*  
 * ------------------------------------------------------------
 *  Autor:          Gustavo Ruiz Luis
 *  Archivo:        Timer.h
 *  Descripcion:    Libreria para el Timer1 del ATmega328P
 *                  Genera una interrupcion cada 1 segundo (F_CPU = 1MHz, prescaler = 64, CTC)
 *                  OCR1A   = (F_CPU / (prescaler * f_deseada)) - 1
 *                          = (1000000 / (64 * 1)) - 1
 *                          = 15624
 * ------------------------------------------------------------
 */

#ifndef TIMER_H
#define TIMER_H

#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

/*
 *  @brief          Inicializa el Timer1 en modo CTC, preescalador 64, sin iniciar la cuenta.
 */
void Timer(void);

/*
 *  @brief          Arranca la cuenta (activa el preescalador).
 */
void Timer_Start(void);

/*
 *  @brief          Detiene la cuenta (desconecta el preescalador) .
 */
void Timer_Stop(void);

/*
 *  @brief          Reinicia el contador a cero .
 */
void Timer_Reset(void);

#endif /* TIMER_H */
