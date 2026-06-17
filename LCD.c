
#include "LCD.h"

static void pulse_E(void) {
    LCD_PORT |= 0x10;
    LCD_PORT &= 0xEF;
}

static void write4(uint8_t nibble) {
    LCD_PORT = nibble & 0x0F;
    pulse_E();
}

void LCD_Inst(uint8_t inst) {
    write4((inst & 0xF0) >> 4);
    write4(inst & 0x0F);
    _delay_us(40);
}

void LCD_Data(uint8_t data) {
    uint8_t high = (data & 0xF0) >> 4;
    uint8_t low  = data & 0x0F;
    LCD_PORT = high | 0x20;   // RS=1
    pulse_E();
    LCD_PORT = low | 0x20;
    pulse_E();
    _delay_us(40);
}

void LCD(void) {
	DDRD = 0X0FF;
    _delay_ms(15);
    write4(0x03); _delay_ms(4);
    write4(0x03); _delay_us(100);
    write4(0x03); _delay_us(40);
    write4(0x02); _delay_us(40);

    LCD_Inst(0x28);   // 2 líneas, 5x7
    LCD_Inst(0x0C);   // display ON, cursor OFF
    LCD_Clear();
    LCD_Inst(0x06);   // incremento automático
	
	
}

void LCD_Clear(void) {
    LCD_Inst(0x01);
    _delay_ms(2);
}

void LCD_Goto(uint8_t pos) {
    uint8_t addr = pos & 0x0F;
    if ((pos & 0xF0) == 0)
        addr |= 0x80;
    else
        addr |= 0xC0;
    LCD_Inst(addr);
}

void LCD_Write(char cad[], uint8_t tam) {
    for (uint8_t i = 0; i < tam; i++) {
        LCD_Data((uint8_t)cad[i]);
        if (i == 15) LCD_Goto(0x10);
    }
}