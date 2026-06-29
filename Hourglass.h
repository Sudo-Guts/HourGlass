/*  
 * ------------------------------------------------------------
 *  Autor:          Gustavo Ruiz Luis
 *  Archivo:        Hourglass.h
 *  Descripcion:    Logica del principal del reloj de arena.
 * ------------------------------------------------------------
 */

#ifndef HOURGLASS_H
#define HOURGLASS_H

#include <util/delay.h>
#include <avr/io.h>
#include "Matrix.h"
#include "MPU6050.h"

/*
 *  Inicializa el hardware del reloj de arena:
 *      - Matrices LED (MAX7219)
 *      - MPU6050 (acelerómetro)
 *  Debe llamarse una vez al inicio, despues de I2C() y SPI().
 */
void Hourglass(void);

/*
 *  Resetea completamente el reloj de arena.
 *  @param minutos  Tiempo configurado en minutos (1..60).
 *                  El reloj durara exactamente esa cantidad de minutos.
 *  Se vacían ambas matrices y se llena la matriz superior con 60 partículas.
 *  Se reinician todos los contadores internos.
 */
void Hourglass_Reiniciar(uint8_t minutos);

/**
 * Función principal que debe invocarse periódicamente (cada ~100 ms)
 * para actualizar la animación de caída de partículas y comprobar el volteo.
 * @return 1 si el tiempo se ha agotado (todas las partículas en la parte inferior),
 *         0 en caso contrario.
 *
 * Internamente actualiza la posición de las partículas, gestiona el cuello
 * y detecta si el reloj se ha vaciado.
 */
uint8_t Hourglass_Tic(void);

/**
 * Debe llamarse exactamente cada 1 segundo (por ejemplo, desde el ISR del Timer1).
 * Actualiza el contador de tiempo restante (minutos y segundos) y la cuenta
 * interna que determina cuándo debe caer una partícula.
 * @return 1 si en este segundo se debe soltar una partícula (para activar un
 *         buzzer o efecto sonoro), 0 en caso contrario.
 */
uint8_t Hourglass_SegundoTranscurrido(void);

/**
 * Obtiene el tiempo restante actual.
 * @param minutos  Puntero donde se almacenarán los minutos restantes.
 * @param segundos Puntero donde se almacenarán los segundos restantes.
 */
void Hourglass_ObtenerTiempo(uint8_t *minutos, uint8_t *segundos);

/**
 * Devuelve la cantidad de partículas que aún están en la matriz superior
 * (la que actualmente está arriba según la gravedad).
 * @return Número de partículas (0 a 60).
 */
uint8_t Hourglass_ObtenerParticulasArriba(void);

/**
 * Lee el acelerómetro MPU6050 y, si la orientación ha cambiado significativamente
 * (inclinación > 90° en el eje Y), invierte la lógica de caída e intercambia
 * el tiempo restante con el tiempo ya transcurrido (como un reloj de arena real).
 * Debe llamarse periódicamente (lo hace automáticamente RelojArena_Tic).
 */
void Hourglass_ActualizarGravedad(void);

/**
 * Fuerza un volteo manual del reloj. Útil para pruebas o si se quiere
 * simular el giro sin tener el sensor.
 */
void Hourglass_ForzarVolteo(void);
uint8_t get_matriz_superior(void);

#endif /* RELOJ_ARENA_H */
