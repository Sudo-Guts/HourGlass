/*
 * HOURGLASS_MAIN.c
 *
 * Created: 11/05/2026
 * Author : Binary BB
 *
 * Hardware:
 *   - ATmega328P @ 1 MHz
 *   - LCD 16x2 en modo 4 bits (PORTD)
 *   - 2x MAX7219 en cascada (SPI por software: PB3=MOSI, PB4=CLK, PB2=SS)
 *   - MPU6050 vía I2C (PC4=SDA, PC5=SCL)
 *   - Encoder rotatorio en PC0(A), PC1(B), PC3(botón SET)
 *   - Botón play/pausa en PC0 (boton1)
 *   - Buzzer en PB1
 *
 * Lógica del reloj de arena:
 *   - 60 partículas totales repartidas en MATRIX_TOP (la que tiene arena al inicio)
 *   - Cada minuto configurado indica cuántas partículas deben caer POR MINUTO:
 *       particulas_por_minuto = 60 / minutos_configurados
 *   - El Timer1 interrumpe cada 1 segundo; cada 60 interrupciones = 1 minuto real
 *   - updateMatrix() anima la física de caída (se llama desde el main loop)
 *   - dropParticle() transfiere partículas del cuello cada vez que toca según el tiempo
 *   - Volteo detectado con MPU6050: |pitch| > 90° → invertir matrices top/bottom
 */

 /*
 * HOURGLASS_MAIN.c
 * Uso de la librería RelojArena para un reloj de arena con matrices LED,
 * MPU6050, encoder y LCD.
 * Hardware: ATmega328P @ 1 MHz
 */

#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "LCD.h"
#include "I2C.h"
#include "Hourglass.h"
#include "Timer.h"
#include "Matrix.h"

/* ─────────────────────────────────────────────
   PINES Y CONFIGURACIÓN
   ───────────────────────────────────────────── */
#define PIN_BUZZER  PB1

/* ─────────────────────────────────────────────
   MÁQUINA DE ESTADOS
   ───────────────────────────────────────────── */
typedef enum { ESPERA, CORRIENDO, PAUSA, CONFIGURACION, FINAL } Estado;

/* ─────────────────────────────────────────────
   VARIABLES GLOBALES
   ───────────────────────────────────────────── */
volatile uint8_t minutos_mostrar = 1;
volatile uint8_t segundos_mostrar = 0;
volatile uint8_t limite_superior = 0, limite_inferior = 0;
volatile uint8_t ultimo_a = 0, ultimo_boton = 1;
volatile uint8_t actualizar_lcd = 0;
volatile uint8_t se_termino = 0;
volatile Estado estado = ESPERA;
Estado estado_anterior = 255;
char lcd_buf[4];

/* ─────────────────────────────────────────────
   PROTOTIPOS LOCALES
   ───────────────────────────────────────────── */
void configurar_tiempo(void);
void esperar_3seg(void);
void pitido_50ms(void);
void buzzer_encender(void);
void buzzer_apagar(void);
static inline void u8_a_dos_digitos(uint8_t v, char *buf) {
    buf[0] = (v / 10) + '0';
    buf[1] = (v % 10) + '0';
}

/* ─────────────────────────────────────────────
   TIMER1 (cada 1 segundo)
   ───────────────────────────────────────────── */
ISR(TIMER1_COMPA_vect) {
    if (estado == CORRIENDO) {
        if (Hourglass_SegundoTranscurrido()) {
            pitido_50ms();   // pitido corto al caer una partícula
        }
        Hourglass_ObtenerTiempo(&minutos_mostrar, &segundos_mostrar);
        actualizar_lcd = 1;
    }
}

/* ─────────────────────────────────────────────
   INTERRUPCIÓN POR CAMBIO DE PIN (encoder y botones)
   ───────────────────────────────────────────── */
ISR(PCINT1_vect) {
    uint8_t a = (PINC >> PC2) & 1;
    uint8_t puerto = PINC & 0x0F;
    uint8_t boton1 = (puerto & 0x01);
    uint8_t boton_set = (puerto & 0x08);

    switch (estado)
    {
    case ESPERA:
        configurar_tiempo();
        if (!boton1 && ultimo_boton) {
            estado = CORRIENDO;
            Hourglass_Reiniciar(minutos_mostrar);
            Timer_Start();
        }
    break;
    
    case CORRIENDO:
        if (!boton1 && ultimo_boton) {
            estado = PAUSA;
            Timer_Stop();
        }
    break;

    case PAUSA:
        if (!boton1 && ultimo_boton) {
            estado = CORRIENDO;
            Timer_Start();
        }
        if (!boton_set) {
            estado = CONFIGURACION;
        }
    break;

    case CONFIGURACION:
        configurar_tiempo();
        if (!boton_set) {
            Hourglass_Reiniciar(minutos_mostrar);
            estado = PAUSA;
        }
    break;

    default:
        configurar_tiempo();
        if (!boton1 && ultimo_boton) {
            estado = CORRIENDO;
            Hourglass_Reiniciar(minutos_mostrar);
            Timer_Start();
        }
    break;
    }

    actualizar_lcd = 1;
    ultimo_boton = boton1;
    ultimo_a = a;
}

/* ─────────────────────────────────────────────
   CONFIGURACIÓN DEL ENCODER
   ───────────────────────────────────────────── */
void configurar_tiempo(void) {
    uint8_t a = (PINC >> PC2) & 1;
    uint8_t b = (PINC >> PC1) & 1;
    if (a && !ultimo_a) {
        if (estado == CONFIGURACION) segundos_mostrar = 0;
        if (a != b) {
            if (minutos_mostrar < 60) minutos_mostrar++;
            else { minutos_mostrar = 60; limite_superior = 1; }
        } else {
            if (minutos_mostrar > 1) minutos_mostrar--;
            else { minutos_mostrar = 1; limite_inferior = 1; }
        }
    }
    ultimo_a = a;
}

/* ─────────────────────────────────────────────
   HELPERS (buzzer, esperas)
   ───────────────────────────────────────────── */
void esperar_1seg(void) {
    for (uint8_t i = 0; i < 10; i++) _delay_ms(100);
}
void buzzer_encender(void)  { PORTB |= (1 << PIN_BUZZER); }
void buzzer_apagar(void) { PORTB &= ~(1 << PIN_BUZZER); }
void pitido_50ms(void) {
    PORTB |= (1 << PIN_BUZZER);
    _delay_ms(50);
    PORTB &= ~(1 << PIN_BUZZER);
}


/* ═══════════════════════════════════════════════════════════════
   MAIN
   ═══════════════════════════════════════════════════════════════ */
int main(void) {
    Matrix();

    /* Configurar puertos */
    DDRC = 0x30;          /* PC4,PC5 salida (I2C), PC0-PC3 entrada con pull-up */
    PORTC = 0x0F;
    DDRD = 0xFF;          /* LCD */
    DDRB |= (1 << PIN_BUZZER);
    PORTB &= ~(1 << PIN_BUZZER);

    /* Interrupciones de cambio de pin */
    PCMSK1 = 0x0F;
    PCICR = 0x02;

    /* Inicializar periféricos */
    LCD();
    I2C();
    Hourglass();     /* Inicializa matrices y MPU6050 */
    Timer();

    ultimo_a = (PINC >> PC2) & 1;
    estado_anterior = 255;
    estado = ESPERA;
    actualizar_lcd = 1;

    LCD_Clear();
    Matrix_Animacion( get_matriz_superior());
    sei();

    while (1) {
        /* Cambio de estado → actualizar primera línea del LCD */
        if ((uint8_t)estado != (uint8_t)estado_anterior) {
            estado_anterior = estado;
            LCD_Clear();
            switch (estado) {
                case ESPERA:
                    LCD_Goto(0x00);
                    LCD_Write("HOURGLASS READY ", 16);
                    if (se_termino) {
                        se_termino = 0;
                        LCD_Goto(0x10);
                        u8_a_dos_digitos(minutos_mostrar, lcd_buf);
                        LCD_Write(" TIME : ", 8);
                        LCD_Data(lcd_buf[0]); LCD_Data(lcd_buf[1]); LCD_Data('m');
                    }
                    break;
                case CORRIENDO:
                    LCD_Goto(0x00);
                    LCD_Write("    RUNNING     ", 16);
                    break;
                case PAUSA:
                    LCD_Goto(0x00);
                    LCD_Write("     PAUSE      ", 16);
                    break;
                case CONFIGURACION:
                    LCD_Goto(0x00);
                    LCD_Write("    SETTING     ", 16);
                    break;
                case FINAL:
                    LCD_Goto(0x00);
                    LCD_Write("   TIME IS UP   ", 16);
                    buzzer_encender();
                    _delay_ms(500);
                    buzzer_apagar();
                    estado = ESPERA;
                    se_termino = 1;
                    break;
            }
        }

        /* Actualizar segunda línea (tiempo) si es necesario */
        if (actualizar_lcd) {
            actualizar_lcd = 0;
            switch (estado) {
                case ESPERA:
                    if (limite_inferior) {
                        LCD_Goto(0x00); LCD_Clear();
                        LCD_Write(" MINIMO LIMITE      ALCANZADO   ", 32);
                        pitido_50ms(); esperar_1seg(); LCD_Clear();
                        limite_inferior = 0;
                    }
                    if (limite_superior) {
                        LCD_Goto(0x00); LCD_Clear();
                        LCD_Write(" MAXIMO LIMITE      ALCANZADO   ", 32);
                        pitido_50ms(); esperar_1seg(); LCD_Clear();
                        limite_superior = 0;
                    }
                    LCD_Goto(0x10);
                    u8_a_dos_digitos(minutos_mostrar, lcd_buf);
                    LCD_Write(" TIME : ", 8);
                    LCD_Data(lcd_buf[0]); LCD_Data(lcd_buf[1]); LCD_Data('m');
                    break;
                case CORRIENDO:
                case PAUSA:
                case CONFIGURACION:
                    LCD_Goto(0x15);
                    u8_a_dos_digitos(minutos_mostrar, lcd_buf);
                    LCD_Data(lcd_buf[0]); LCD_Data(lcd_buf[1]); LCD_Data(':');
                    u8_a_dos_digitos(segundos_mostrar, lcd_buf);
                    LCD_Data(lcd_buf[0]); LCD_Data(lcd_buf[1]);
                    break;
                default: break;
            }
        }

        /* Bucle principal del reloj de arena (solo si está corriendo) */
        if (estado == CORRIENDO) {
            if (Hourglass_Tic()) {   // tiempo agotado
                Timer_Stop();
                estado = FINAL;
                actualizar_lcd = 1;
            }
            _delay_ms(100);   // frame rate de animación
        }
    }
}