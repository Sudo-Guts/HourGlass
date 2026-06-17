# ⏳ HourGlass

Un reloj de arena electrónico con 60 partículas (LEDs) distribuidas en dos matrices 8×8 en cascada.  
Detecta el volteo mediante un acelerómetro MPU6050 e invierte el flujo del tiempo y la gravedad, imitando el comportamiento de un reloj de arena real.


## 🧠 Características

- **Física simulada**: 60 partículas que caen siguiendo una gravedad diagonal hacia el cuello de la matriz.
- **Volteo real**: Al girar el reloj 180°, el MPU6050 detecta el cambio y el tiempo restante se intercambia con el tiempo transcurrido (como en un reloj de arena verdadero).
- **Interfaz de usuario**:
  - LCD 16×2 muestra el tiempo restante (MM:SS) y el estado (RUNNING, PAUSE, SETTING).
  - Encoder rotatorio para configurar los minutos (1 a 60).
  - Botón de play/pausa.
  - Buzzer que emite un pitido corto por cada grano que cae y alarma final.
- **Modo configuración**: Permite ajustar el tiempo sin necesidad de reiniciar el microcontrolador.


## 🛠️ Hardware utilizado

| Componente           | Cantidad | Conexión / Notas                                   |
|----------------------|----------|----------------------------------------------------|
| ATMega328P           | 1        | Microcontrolador principal (1 MHz interno)         |
| Matriz LED 8×8       | 2        | En cascada, controladas con MAX7219 (SPI)          |
| MPU6050              | 1        | Acelerómetro/giroscopio (I2C)                      |
| LCD 16×2             | 1        | Modo 4 bits (PORTD)                                |
| Encoder rotatorio    | 1        | Pines PC2 (A), PC1 (B), PC3 (botón SET)            |
| Botón pulsador       | 1        | Play/Pausa en PC0                                  |
| Buzzer pasivo        | 1        | PB1                                                |

## 🔌 Diagrama de pines

| Puerto | Función                     |
|--------|-----------------------------|
| PB0    | (Libre, podría usarse para INT del MPU6050) |
| PB1    | Buzzer                      |
| PB2    | SS (selección esclavo SPI)  |
| PB3    | MOSI                        |
| PB5    | CLK                         |
| PC0    | Encoder / Botón 1           |
| PC1    | Encoder B                   |
| PC2    | Encoder A                   |
| PC3    | Botón SET                   |
| PC4    | SDA (I2C)                   |
| PC5    | SCL (I2C)                   |
| PC6    | Botón RST                   |
| PD0-PD5| LCD (4 bits: D0-D3 + RS, E) |


## 📁 Estructura del proyecto

- hourglass/
- ├── [main.c](https://github.com/Sudo-Guts/HourGlass/blob/main/main.c)                      # Bucle principal y máquina de estados
- ├── I2C.c / I2C.h               # Comunicación I2C (maestro)
- ├── SPI.c / SPI.h               # Comunicación SPI (maestro)
- ├── Matrix.c / Matrix.h         # Control de matrices MAX7219 (buffer, refresco)
- ├── MPU6050.c / MPU6050.h       # Lectura del acelerómetro
- ├── LCD.c / LCD.h               # Driver para LCD 16x2 en modo 4 bits
- ├── Timer.c / Timer.h           # Timer1 para base de tiempo de 1 segundo
- ├── Hourglass.c / Hourglass.h   # Lógica principal del reloj de arena
- └── README.md                   # Este archivo


## 📜 Diagrama de conexión

![Logo](https://github.com/Sudo-Guts/HourGlass/blob/main/Diagrama.jpg)






