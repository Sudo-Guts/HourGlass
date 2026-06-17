#include "Matrix.h"

static uint8_t buffer[16];

void Matrix(void) {
	SPI();   // iniciar SPI
	for (uint8_t m = 0; m < 2; m++) {
		Matrix_Write(OP_DISPLAYTEST, 0x00, m);
		Matrix_Write(OP_DECODEMODE, 0x00, m);
		Matrix_Write(OP_INTENSITY, 0x02, m);
		Matrix_Write(OP_SCANLIMIT, 0x07, m);
		Matrix_Write(OP_SHUTDOWN, 0x01, m);
	}
	// precargar una animación simple
	for (uint8_t i = 0; i < 16; i++) buffer[i] = 0x00;
	buffer[3] = 0x18; buffer[4] = 0x18;
	buffer[11] = 0x18; buffer[12] = 0x18;
	Matrix_Refresh();
	_delay_ms(1000);
}

void Matrix_Write(uint8_t op, uint8_t data, uint8_t matriz) {
	PORTB &= ~(1 << PB2);   // SS = 0
	if (matriz == 0) {
		SPI_Enviar(OP_NOOP); SPI_Enviar(0x00);
		SPI_Enviar(op);    SPI_Enviar(data);
		} else {
		SPI_Enviar(op);    SPI_Enviar(data);
		SPI_Enviar(OP_NOOP); SPI_Enviar(0x00);
	}
	PORTB |= (1 << PB2);    // SS = 1
}

void Matrix_Refresh(void) {
	for (uint8_t f = 0; f < 8; f++) {
		PORTB &= ~(1 << PB2);
		SPI_Enviar(f + 1);        // fila para matriz inferior
		SPI_Enviar(buffer[f + 8]);
		SPI_Enviar(f + 1);        // fila para matriz superior
		SPI_Enviar(buffer[f]);
		PORTB |= (1 << PB2);
	}
}

void Matrix_Pixel(uint8_t x, uint8_t y, uint8_t estado) {
	if (x > 7 || y > 15) return;
	if (estado)
	buffer[y] |= (1 << (7 - x));
	else
	buffer[y] &= ~(1 << (7 - x));
}

void Matrix_Clear(void) {
	for (uint8_t i = 0; i < 16; i++) buffer[i] = 0;
	Matrix_Refresh();
}

uint8_t Matrix_GetPixel(uint8_t x, uint8_t y) {
	if (x > 7 || y > 15) return 0;
	return (buffer[y] >> (7 - x)) & 1;
}

void Matrix_Animacion(uint8_t isUp) {
	uint8_t fila;
	isUp = (isUp == 1) ? 0 : 8;
	for (fila = 0; fila < 8; fila++) {
		buffer[fila + isUp] = (fila > 1 && fila < 6) ? 0x3C : 0x00;
		buffer[fila + 8 - isUp] = 0x00;
	}
	buffer[5 + 8 - isUp] = 0x24;
	buffer[2 + 8 - isUp] = 0x24;
	Matrix_Refresh();
	_delay_ms(500);
	for (fila = 0; fila < 8; fila++) {
		buffer[fila + isUp] = (fila > 0 && fila < 7) ? 0x7E : 0;
		buffer[fila + 8 - isUp] = 0x00;
	}
	buffer[1 + 8 - isUp] = 0x42;
	buffer[6 + 8 - isUp] = 0x42;
	Matrix_Refresh();
	_delay_ms(500);
	for (fila = 0; fila < 8; fila++) {
		buffer[fila + isUp] = 0xFF;
		buffer[fila + 8 - isUp] = 0x00;
	}
	buffer[0 + 8 - isUp] = 0x81;
	buffer[7 + 8 - isUp] = 0x81;
	Matrix_Refresh();
	_delay_ms(500);
	for (fila = 0; fila < 8; fila++) {
		buffer[fila + isUp] = 0xFF;
		buffer[fila + 8 - isUp] = 0x00;
	}
	buffer[isUp + 0] = 0x9F;
	buffer[isUp + 1] = 0x7F;
	buffer[isUp + 2] = 0x7F;
	Matrix_Refresh();
	_delay_ms(500);
	
}

void Matrix_Caida(uint8_t matriz_inferior) {
	uint8_t fila = 0;
	uint8_t x, y;
	uint8_t y_base = matriz_inferior ? 8 : 0;
	uint8_t estado = 0;
	do{
		fila++;
		x = 7 - fila;
		y = y_base + 7 - fila;
		estado = Matrix_GetPixel(x, y);
		Matrix_Pixel(x, y, 1);
		Matrix_Refresh();
		_delay_ms(100);
		Matrix_Pixel(x, y, estado);
		Matrix_Refresh();
	} while( estado == 0 && fila < 8 );
}
