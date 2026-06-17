#include "Timer.h"

void Timer(void) {
    /* Modo CTC (WGM12 = 1) */
    TCCR1A = 0x00;
    TCCR1B = (1 << WGM12);

    /* Valor de comparaci�n para 1 segundo exacto
     * OCR1A = (1000000 / (64 * 1)) - 1 = 15624 */
    OCR1A = 15624;

    /* Reiniciar contador */
    TCNT1 = 0;

    /* Habilitar interrupci�n por comparaci�n A */
    TIMSK1 = (1 << OCIE1A);
}

void Timer_Start(void) {
    /* Preescalador 64: CS11 | CS10 */
    TCNT1  = 0;
    TCCR1B |= (1 << CS11) | (1 << CS10);
}

void Timer_Stop(void) {
    /* Desconectar preescalador (CS1x = 000) */
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
    TCNT1 = 0;
}

void Timer_Reset(void) {
    TCNT1 = 0;
}