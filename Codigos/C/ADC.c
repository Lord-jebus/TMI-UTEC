// ===========================================================
// Programa: Lectura de ADC con indicador LED
// Microcontrolador: ATmega328P
// Frecuencia: 1 MHz interna
// Autor: [Tu nombre]
// Descripción:
// Lee el valor analógico de un potenciómetro conectado al canal ADC0 (PC0)
// y enciende un LED en PB0 si el valor leído supera la mitad del rango (512).
// ===========================================================

#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 1000000UL   // Definición de frecuencia del reloj

// -----------------------------------------------------------
// Función: ADC_init()
// Configura el ADC para usar AVCC como referencia y un prescaler de 128
// -----------------------------------------------------------
void ADC_init() {
    // Seleccionar AVCC como referencia de voltaje (con capacitor en AREF)
    ADMUX = (1 << REFS0);
    
    // Habilitar ADC y configurar prescaler a 128:
    // Frecuencia del ADC = F_CPU / 128 = 7.8 kHz (dentro del rango recomendado)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// -----------------------------------------------------------
// Función: ADC_read(channel)
// Lee el valor analógico del canal especificado (0–7) y devuelve un entero de 10 bits
// -----------------------------------------------------------
uint16_t ADC_read(uint8_t channel) {
    // Asegurar que el canal esté en el rango válido (0–7)
    channel &= 0x07;
    
    // Seleccionar el canal sin afectar los bits de referencia
    ADMUX = (ADMUX & 0xF0) | channel;
    
    // Iniciar conversión
    ADCSRA |= (1 << ADSC);
    
    // Esperar a que la conversión termine (ADSC = 0)
    while (ADCSRA & (1 << ADSC));
    
    // Retornar el resultado de 10 bits del registro ADC (ADCL + ADCH)
    return ADC;
}

// -----------------------------------------------------------
// Función principal
// -----------------------------------------------------------
int main(void) {
    uint16_t adc_value;
    
    ADC_init();               // Inicializar el ADC
    
    DDRB |= (1 << PB0);       // Configurar PB0 como salida (LED indicador)
    
    while (1) {
        adc_value = ADC_read(0);  // Leer el canal ADC0 (pin PC0)
        
        // Encender LED si el valor ADC > 512 (mitad del rango 0–1023)
        if (adc_value > 512) {
            PORTB |= (1 << PB0);
        } else {
            PORTB &= ~(1 << PB0);
        }
        
        _delay_ms(100);       // Pequeño retardo para estabilizar la lectura
    }
    
    return 0;
}
