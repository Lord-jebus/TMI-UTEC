/*
------------------------------------------------------------
Programa: Lectura y escritura en EEPROM interna
Microcontrolador: ATmega328P (compatible con AVR)
Compilador: avr-gcc
------------------------------------------------------------
Descripción general:
Este programa demuestra cómo escribir y leer un byte en la 
EEPROM interna del microcontrolador usando acceso directo 
a registros (sin las funciones de biblioteca eeprom_write_byte).

Recordar que la escritura en la EEPROM toma 4ms aproximadamente.

------------------------------------------------------------
Conexiones: No requiere hardware adicional.
------------------------------------------------------------
*/

#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdint.h>   // Para los tipos uint8_t, uint16_t

//------------------------------------------------------------
// Función: EEPROM_write
// Escribe un byte en una dirección específica de la EEPROM
//------------------------------------------------------------
// Parámetros:
//   address → dirección de 0x000 a 0x3FF (para 1 KB de EEPROM en ATmega328P)
//   data    → byte a escribir
//------------------------------------------------------------
void EEPROM_write(uint16_t address, uint8_t data) {    
    // Espera a que no haya una escritura previa en curso
    while (EECR & (1 << EEPE));  
    
    // Carga la dirección de memoria y el dato
    EEAR = address;              // Dirección donde se escribirá
    EEDR = data;                 // Dato a escribir
    
    // Secuencia obligatoria para habilitar escritura:
    EECR |= (1 << EEMPE);        // Habilita la escritura (Master Write Enable)
    EECR |= (1 << EEPE);         // Inicia la operación de escritura real
}

//------------------------------------------------------------
// Función: EEPROM_read
// Lee un byte de una dirección específica de la EEPROM
//------------------------------------------------------------
// Parámetro:
//   address → dirección de 0x000 a 0x3FF
// Retorno:
//   Byte leído desde la EEPROM
//------------------------------------------------------------
uint8_t EEPROM_read(uint16_t address) {
    // Espera a que no haya una escritura en curso
    while (EECR & (1 << EEPE));
    
    // Carga la dirección de memoria a leer
    EEAR = address;
    
    // Inicia la operación de lectura
    EECR |= (1 << EERE);
    
    // Retorna el valor leído desde el registro de datos EEPROM
    return EEDR;
}

//------------------------------------------------------------
// Función principal
//------------------------------------------------------------
int main(void) {
    // --- Configuraciones iniciales (si se requieren) ---
    // Aquí podrían inicializarse puertos, UART, etc.
    
    // Escribir el valor 42 en la dirección 0x10 de la EEPROM
    EEPROM_write(0x10, 42);  
    
    // Leer el valor previamente escrito
    uint8_t read_value = EEPROM_read(0x10);
    
    // Variable "read_value" contendrá 42 si la escritura fue correcta
    
    while (1) {
        // Bucle infinito (se puede usar para depuración, etc.)
    }
    
    return 0;
}
