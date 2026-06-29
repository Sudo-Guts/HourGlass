/*  
 * ------------------------------------------------------------
 *  Autor:          Gustavo Ruiz Luis
 *  Archivo:        Hourglass.c
 *  Descripcion:    Logica del principal del reloj de arena.
 * ------------------------------------------------------------
 */

#include "Hourglass.h"

/* ────────────────────────────────────────────────────────────
   CONSTANTES Y CONFIGURACIÓN
   ──────────────────────────────────────────────────────────── */
#define TOTAL_PARTICULAS     60
#define UMBRAL_VOLTEO        4096    /* Umbral para detectar volteo (±0.25g) */

/* Identificadores de matrices (coinciden con Matrix.h) */
#define MATRIZ_SUPERIOR_IDX  0       /* Matriz superior (física) */
#define MATRIZ_INFERIOR_IDX  1       /* Matriz inferior (física) */

/* ────────────────────────────────────────────────────────────
   VARIABLES INTERNAS (estáticas)
   ──────────────────────────────────────────────────────────── */
static uint8_t gravedad = 1;              /* 1 = normal, 0 = volteado */
static uint8_t minutos_restantes = 0;
static uint8_t segundos_restantes = 0;
static uint8_t tiempo_total_minutos = 0;  /* Para intercambiar tiempo al voltear */

static uint8_t segundos_por_particula = 1;
static uint8_t contador_segundos = 0;
static uint8_t debe_dropear = 0;
static uint8_t alarma_sonada = 0;

/* ────────────────────────────────────────────────────────────
   PROTOTIPOS DE FUNCIONES AUXILIARES
   ──────────────────────────────────────────────────────────── */
static uint8_t obtener_matriz_superior(void);
static uint8_t obtener_matriz_inferior(void);
static void llenar_matriz(uint8_t direccion, uint8_t maximo);
static uint8_t contar_particulas(uint8_t direccion);
static uint8_t leer_pixel(uint8_t direccion, int8_t x, int8_t y);
static void escribir_pixel(uint8_t direccion, int8_t x, int8_t y, uint8_t valor);
static uint8_t mover_particula(uint8_t direccion, uint8_t x, uint8_t y);
static uint8_t actualizar_matrices(void);
static uint8_t soltar_particula(void);
static void intercambiar_tiempo(void);

/* ────────────────────────────────────────────────────────────
   IMPLEMENTACIÓN DE FUNCIONES PUBLICAS
   ──────────────────────────────────────────────────────────── */

void Hourglass(void) {
    Matrix();      // Inicializa MAX7219
    MPU6050();     // Inicializa MPU6050
    gravedad = 0;
    alarma_sonada = 0;
    debe_dropear = 0;
    contador_segundos = 0;
}

void Hourglass_Reiniciar(uint8_t minutos) {
    if (minutos < 1) minutos = 1;
    if (minutos > 60) minutos = 60;
    minutos_restantes = minutos;
    segundos_restantes = 0;
    tiempo_total_minutos = minutos;
    segundos_por_particula = minutos;   /* 1 partícula cada 'minutos' segundos */
    contador_segundos = 0;
    debe_dropear = 0;
    alarma_sonada = 0;

    Matrix_Clear();
    llenar_matriz(obtener_matriz_superior(), TOTAL_PARTICULAS);
}

uint8_t Hourglass_Tic(void) {
    Hourglass_ActualizarGravedad();
	
	uint8_t inferior = obtener_matriz_inferior();
    uint8_t movio = actualizar_matrices();
    uint8_t solto = soltar_particula();
    Matrix_Refresh();

    /* Si ya no hay partículas arriba y no hemos sonado la alarma */
    if (!movio && !solto && !alarma_sonada &&
        (contar_particulas(obtener_matriz_superior()) == 0)) {
        alarma_sonada = 1;
        return 1;   /* tiempo agotado */
    }
    if (solto) alarma_sonada = 0;
    return 0;
}

uint8_t Hourglass_SegundoTranscurrido(void) {
    /* Decrementar tiempo restante */
    if (minutos_restantes == 0 && segundos_restantes == 0) {
        return 0;   // ya terminó
    }
    if (segundos_restantes == 0) {
        if (minutos_restantes > 0) {
            minutos_restantes--;
            segundos_restantes = 59;
        }
    } else {
        segundos_restantes--;
    }

    /* Contador para caída de partículas */
    contador_segundos++;
    if (contador_segundos >= segundos_por_particula) {
        contador_segundos = 0;
        debe_dropear = 1;
        return 1;   // indica que se debe soltar una partícula (para buzzer)
    }
    return 0;
}

void Hourglass_ObtenerTiempo(uint8_t *minutos, uint8_t *segundos) {
    *minutos = minutos_restantes;
    *segundos = segundos_restantes;
}

uint8_t Hourglass_ObtenerParticulasArriba(void) {
    return contar_particulas(obtener_matriz_superior());
}

void Hourglass_ActualizarGravedad(void) {
    int16_t ax, ay, az;
    MPU6050_ReadAll(&ax, &ay, &az);
    uint8_t nueva_gravedad = gravedad;

    if (ay > UMBRAL_VOLTEO) nueva_gravedad = 1;   /* normal */
    else if (ay < -UMBRAL_VOLTEO) nueva_gravedad = 0; /* volteado */
    else return;   /* zona muerta */

    if (nueva_gravedad != gravedad) {
        gravedad = nueva_gravedad;
        intercambiar_tiempo();              /* intercambia minutos y segundos */
        /* Invertir contador de caída */
        if (contador_segundos == 0)
            contador_segundos = segundos_por_particula;
        else
            contador_segundos = segundos_por_particula - contador_segundos;
        if (contador_segundos >= segundos_por_particula)
            contador_segundos = segundos_por_particula - 1;
        debe_dropear = 0;
    }
}

void RelojArena_ForzarVolteo(void) {
    gravedad = (gravedad == 0) ? 1 : 0;
    intercambiar_tiempo();
    if (contador_segundos == 0)
        contador_segundos = segundos_por_particula;
    else
        contador_segundos = segundos_por_particula - contador_segundos;
    if (contador_segundos >= segundos_por_particula)
        contador_segundos = segundos_por_particula - 1;
    debe_dropear = 0;
}

/* ────────────────────────────────────────────────────────────
   FUNCIONES AUXILIARES 
   ──────────────────────────────────────────────────────────── */

static uint8_t obtener_matriz_superior(void) {
    return (gravedad == 0) ? MATRIZ_SUPERIOR_IDX : MATRIZ_INFERIOR_IDX;
}

static uint8_t obtener_matriz_inferior(void) {
    return (gravedad == 0) ? MATRIZ_INFERIOR_IDX : MATRIZ_SUPERIOR_IDX;
}

static void llenar_matriz(uint8_t direccion, uint8_t maximo) {
    uint8_t n = 8;
    uint8_t x, y;
    uint8_t contador = 0;
    uint8_t baseY = (direccion == MATRIZ_SUPERIOR_IDX) ? 0 : 8;
    for (uint8_t diagonal = 0; diagonal < 2 * n - 1; ++diagonal) {
        uint8_t inicio = (diagonal < n) ? 0 : diagonal - n + 1;
        for (uint8_t j = inicio; j <= diagonal - inicio; ++j) {
            x = j;
            y = diagonal - j;
            Matrix_Pixel(x, y + baseY, (++contador <= maximo) ? 1 : 0);
        }
    }
}

static uint8_t contar_particulas(uint8_t direccion) {
    uint8_t c = 0;
    uint8_t baseY = (direccion == MATRIZ_SUPERIOR_IDX) ? 0 : 8;
    for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            if (Matrix_GetPixel(x, y + baseY)) c++;
        }
    }
    return c;
}

static uint8_t leer_pixel(uint8_t direccion, int8_t x, int8_t y) {
    if (x < 0 || x > 7 || y < 0 || y > 7) return 1;
    uint8_t baseY = (direccion == MATRIZ_SUPERIOR_IDX) ? 0 : 8;
    return Matrix_GetPixel((uint8_t)x, (uint8_t)(y + baseY));
}

static void escribir_pixel(uint8_t direccion, int8_t x, int8_t y, uint8_t valor) {
    if (x < 0 || x > 7 || y < 0 || y > 7) return;
    uint8_t baseY = (direccion == MATRIZ_SUPERIOR_IDX) ? 0 : 8;
    Matrix_Pixel((uint8_t)x, (uint8_t)(y + baseY), valor);
}

static uint8_t mover_particula(uint8_t direccion, uint8_t x, uint8_t y) {
    if (!leer_pixel(direccion, x, y)) return 0;
    uint8_t es_superior = (direccion == obtener_matriz_superior());
    if (x == 7 && y == 7 && es_superior) return 0; // cuello, lo maneja soltar_particula

    uint8_t puedeA, puedeB, puedeDiag;

    if (es_superior) {
        puedeA    = (x < 7) && !leer_pixel(direccion, x+1, y);
        puedeB    = (y < 7) && !leer_pixel(direccion, x, y+1);
        puedeDiag = puedeA && puedeB && !leer_pixel(direccion, x+1, y+1);
        if (!puedeA && !puedeB) return 0;

        if (puedeDiag) {
            escribir_pixel(direccion, x, y, 0);
            escribir_pixel(direccion, x+1, y+1, 1);
        } else if (puedeA && !puedeB) {
            escribir_pixel(direccion, x, y, 0);
            escribir_pixel(direccion, x+1, y, 1);
        } else if (puedeB && !puedeA) {
            escribir_pixel(direccion, x, y, 0);
            escribir_pixel(direccion, x, y+1, 1);
        } else {
            static uint8_t rngT = 42;
            rngT = rngT * 31 + 7;
            if (rngT & 1) { escribir_pixel(direccion, x, y, 0); escribir_pixel(direccion, x+1, y, 1); }
            else          { escribir_pixel(direccion, x, y, 0); escribir_pixel(direccion, x, y+1, 1); }
        }
    } else {
        // matriz inferior: gravedad hacia (0,0)
        puedeA    = (x > 0) && !leer_pixel(direccion, x-1, y);
        puedeB    = (y > 0) && !leer_pixel(direccion, x, y-1);
        puedeDiag = puedeA && puedeB && !leer_pixel(direccion, x-1, y-1);
        if (!puedeA && !puedeB) return 0;

        if (puedeDiag) {
            escribir_pixel(direccion, x, y, 0);
            escribir_pixel(direccion, x-1, y-1, 1);
        } else if (puedeA && !puedeB) {
            escribir_pixel(direccion, x, y, 0);
            escribir_pixel(direccion, x-1, y, 1);
        } else if (puedeB && !puedeA) {
            escribir_pixel(direccion, x, y, 0);
            escribir_pixel(direccion, x, y-1, 1);
        } else {
            static uint8_t rngB = 17;
            rngB = rngB * 31 + 7;
            if (rngB & 1) { escribir_pixel(direccion, x, y, 0); escribir_pixel(direccion, x-1, y, 1); }
            else          { escribir_pixel(direccion, x, y, 0); escribir_pixel(direccion, x, y-1, 1); }
        }
    }
    return 1;
}

static uint8_t actualizar_matrices(void) {
    uint8_t n = 8;
    uint8_t algo_movio = 0;
    static uint8_t rng2 = 13;
    for (int8_t diagonal = (int8_t)(2 * n - 2); diagonal >= 0; diagonal--) {
        rng2 = rng2 * 31 + 7;
        uint8_t direccion = rng2 & 1;
        uint8_t inicio = (diagonal < n) ? 0 : (uint8_t)diagonal - n + 1;
        for (uint8_t j = inicio; j <= (uint8_t)diagonal - inicio; ++j) {
            uint8_t y = direccion ? (uint8_t)(diagonal - j) : j;
            uint8_t x = direccion ? j : (uint8_t)(diagonal - j);
            if (mover_particula(MATRIZ_INFERIOR_IDX, x, y)) algo_movio = 1;
            if (mover_particula(MATRIZ_SUPERIOR_IDX, x, y)) algo_movio = 1;
        }
    }
    return algo_movio;
}

static uint8_t soltar_particula(void) {
    if (!debe_dropear) return 0;
    debe_dropear = 0;

    uint8_t superior = obtener_matriz_superior();
    uint8_t inferior = obtener_matriz_inferior();
    uint8_t baseSup = (superior == MATRIZ_SUPERIOR_IDX) ? 0 : 8;
    uint8_t baseInf = (inferior == MATRIZ_SUPERIOR_IDX) ? 0 : 8;
	
    // Si el cuello de la superior está vacío, buscar partícula cercana
    if (!Matrix_GetPixel(7, 7 + baseSup)) {
        int8_t mejorX = -1, mejorY = -1;
        int8_t mejorPuntaje = -1;
        for (uint8_t px = 0; px < 8; px++) {
            for (uint8_t py = 0; py < 8; py++) {
                if ((px == 7 || py == 7) && Matrix_GetPixel(px, py + baseSup)) {
                    int8_t puntaje = (int8_t)px + (int8_t)py;
                    if (puntaje > mejorPuntaje) {
                        mejorPuntaje = puntaje;
                        mejorX = (int8_t)px;
                        mejorY = (int8_t)py;
                    }
                }
            }
        }
        if (mejorX < 0) return 0; // no hay nada cerca
        Matrix_Pixel((uint8_t)mejorX, (uint8_t)mejorY + baseSup, 0);
        Matrix_Pixel(7, 7 + baseSup, 1);
    }

    // ¿Cuello inferior libre?
    if (Matrix_GetPixel(7, 7 + baseInf)) return 0;

    // Transferir partícula
    Matrix_Pixel(7, 7 + baseSup, 0);
	
	// Mostrar animación de caída en la matriz inferior
	// 'inferior' es MATRIZ_SUPERIOR_IDX o MATRIZ_INFERIOR_IDX.
	// Necesitamos un 0/1 para Matrix_Caida: 0 = matriz de arriba (filas 0-7), 1 = matriz de abajo (filas 8-15)
	uint8_t esMatrizInferior = (inferior == MATRIZ_INFERIOR_IDX); // 1 si la inferior es la física de abajo
	Matrix_Caida(esMatrizInferior);
	
	// Transferir partícula
    Matrix_Pixel(7, 7 + baseInf, 1);
	
    return 1;
}

static void intercambiar_tiempo(void) {
    uint16_t total_seg = tiempo_total_minutos * 60;
    uint16_t restante = minutos_restantes * 60 + segundos_restantes;
    uint16_t nuevo_restante = total_seg - restante;
    minutos_restantes = nuevo_restante / 60;
    segundos_restantes = nuevo_restante % 60;
}

uint8_t get_matriz_superior(void){
	return (gravedad == 1) ? MATRIZ_SUPERIOR_IDX : MATRIZ_INFERIOR_IDX;
}
