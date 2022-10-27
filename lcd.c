//Inclusoes de Bibliotecas
#include "lcd.h"
#include <pic32m_builtins.h>
#include "mcc_generated_files/pin_manager.h"
#include <xc.h>


//Definicoes
#define bitSet(arg,bit) ((arg) |=  (1<<(bit)))
#define bitClr(arg,bit) ((arg) &= ~(1<<(bit))) 
#define bitFlp(arg,bit) ((arg) ^=  (1<<(bit))) 
#define bitTst(arg,bit) ((arg) &   (1<<(bit)))
#define LOW 0
#define HIGH 1


//Gera um delay de 'a' microsegundos
void delayMicro(int a) {
    volatile int i;
    //comment on simulator
    for (i = 0; i < (a * 2); i++);
}

//Gera um delay de 'a' milisegundos
void delayMili(int a) {
    volatile int i;
    for (i = 0; i < a; i++) {
        //comment on simulator
        delayMicro(1000);
    }
}

//Gera um clock no enable
void pulseEnablePin() {
    bitSet(PORTC, 7);
    delayMicro(5);
    bitClr(PORTC, 7);
    delayMicro(5);
}

//Envia 4 bits e gera um clock no enable
void pushNibble(char value, int rs) {

    if (value & 0b1000) {
        bitSet(PORTC, 3);
    } else {
        bitClr(PORTC, 3);
    }
    if (value & 0b0100) {
        bitSet(PORTC, 4);
    } else {
        bitClr(PORTC, 4);
    }
    if (value & 0b0010) {
        bitSet(PORTC, 5);
    } else {
        bitClr(PORTC, 5);
    }
    if (value & 0b0001) {
        bitSet(PORTC, 6);
    } else {
        bitClr(PORTC, 6);
    }

    if (rs) {
        bitSet(PORTA, 7);
    }
    else {
        bitClr(PORTA, 7);
    }
    pulseEnablePin();
}

//Envia 8 bits em dois pacotes de 4
void pushByte(char value, int rs) {
    //Verifica se dados ou comandos
    if (rs) {
        bitSet(PORTA, 7);
    }
    else {
        bitClr(PORTA, 7);
    }
     
    //manda os 4 ultimos bits
    if (value & 0x80) {
        bitSet(PORTC, 3);
    } else {
        bitClr(PORTC, 3);
    }
    if (value & 0x40) {
        bitSet(PORTC, 4);
    } else {
        bitClr(PORTC, 4);
    }
    if (value & 0x20) {
        bitSet(PORTC, 5);
    } else {
        bitClr(PORTC, 5);
    }
    if (value & 0x10) {
        bitSet(PORTC, 6);
    } else {
        bitClr(PORTC, 6);
    } 
    pulseEnablePin();

    //manda os 4 primeiros bits
    if (value & 0x08) {
        bitSet(PORTC, 3);
    } else {
        bitClr(PORTC, 3);
    }
    if (value & 0x04) {
        bitSet(PORTC, 4);
    } else {
        bitClr(PORTC, 4);
    }
    if (value & 0x02) {
        bitSet(PORTC, 5);
    } else {
        bitClr(PORTC, 5);
    }
    if (value & 0x01) {
        bitSet(PORTC, 6);
    } else {
        bitClr(PORTC, 6);
    }

    pulseEnablePin(); //---
}

//Manda um comando 
void lcdCommand(char value) {
    pushByte(value, LOW);
    delayMili(2);
}

//Controla a posição no LCD
void lcdPosition(int line, int col) {
    if (line == 0) {
        lcdCommand(0x80 + (col % 16));
    }
    if (line == 1) {
        lcdCommand(0xC0 + (col % 16));
    }
}

//Imprime um Char
void lcdChar(char value) {
    pushByte(value, HIGH);
    delayMicro(80);
}

//Imprime um texto (vetor de char)
void lcdString(char msg[]) {
    int i = 0;
    while (msg[i] != 0) {
        lcdChar(msg[i]);
        i++;
    }
}

//Imprime um numero
void lcdNumber(int value) {
    int i = 10000; //Máximo 99.999
    while (i > 0) {
        lcdChar((value / i) % 10 + 48);
        i /= 10;
    }
}

// Rotina de incialização
void lcdInit() {
    IO_RC8_SetDigitalOutput();
    IO_RC7_SetDigitalOutput();
    IO_RC8_SetLow();
    delayMili(15);
    
    // Comunicação começa em estado incerto
    pushNibble(0x03, LOW);
    delayMili(5);
    pushNibble(0x03, LOW);
    delayMicro(160);
    pushNibble(0x03, LOW);
    delayMicro(160);
    
    // Mudando comunicação para 4 bits
    pushNibble(0x02, LOW);
    delayMili(10);
    
    // Configura o display
    lcdCommand(0x28); //8bits, 2 linhas, fonte: 5x8
    lcdCommand(0x08 + 0x04); //display on
    lcdCommand(0x01); //limpar display, voltar p/ posição 0
}