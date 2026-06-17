
#include "I2C.h"

void I2C(void) {
    TWBR = 0x02;          // 50 kHz
    TWSR = 0x00;          // prescaler = 1
    TWCR = (1 << TWEN);   // habilitar TWI
}

uint8_t I2C_Start(void) {
    uint8_t status;
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
    while (!(TWCR & (1 << TWINT)));
    status = TWSR & 0xF8;
    if (status == 0x08 || status == 0x10)   // START o START repetido
        return 1;
    return status;
}

void I2C_Stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while (TWCR & (1 << TWSTO));
}

uint8_t I2C_Write(uint8_t dato) {
    uint8_t status;
    TWDR = dato;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    status = TWSR & 0xF8;
    if (status == 0x18 || status == 0x28 || status == 0x40)
        return 1;   // ACK recibido
    return status;
}

uint8_t I2C_Read(uint8_t *dato, uint8_t ack) {
    uint8_t status;
    if (ack)
        TWCR |= (1 << TWEA);
    else
        TWCR &= ~(1 << TWEA);
    TWCR |= (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    status = TWSR & 0xF8;
    if (status == 0x58 || status == 0x50) {
        *dato = TWDR;
        return 1;
    }
    return status;
}