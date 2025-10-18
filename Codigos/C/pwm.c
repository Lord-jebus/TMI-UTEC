#define F_CPU 16000000UL     // Frecuencia del reloj del sistema: 16 MHz
#include <avr/io.h>
#include <util/delay.h>

//-------------------------------------------------------------
// Configuración de Timer1 para generar PWM en OC1A (PB1)
//-------------------------------------------------------------
void timer1_init_fastPWM(void) {
    DDRB |= (1 << PB1);  // Configura el pin PB1 (OC1A) como salida

    // Configuración del modo Fast PWM con TOP = ICR1 (modo 14)
    // COM1A1=1: salida no invertida en OC1A
    // WGM13:WGM12:WGM11=1:1:1 → Modo 14 (Fast PWM con TOP=ICR1)
    // CS11=1 → Prescaler = 8
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    // ----------------------------------------------------------
    // Frecuencia PWM deseada: 1 kHz
    // Fórmula: f_PWM = F_CPU / (Prescaler * (1 + ICR1))
    // Despejando: ICR1 = (F_CPU / (Prescaler * f_PWM)) - 1
    // ----------------------------------------------------------
    ICR1 = (F_CPU / (8UL * 1000UL)) - 1;  // ICR1 = 1999 → PWM de 1 kHz

    // Duty cycle inicial = 50%
    OCR1A = ICR1 / 2;  
}

//-------------------------------------------------------------
// Programa principal
//-------------------------------------------------------------
int main(void) {
    timer1_init_fastPWM();  // Inicializa el PWM

    while (1) {
        // Ejemplo: variación suave del ciclo de trabajo entre 10% y 90%
        for (uint16_t i = ICR1 / 10; i < (ICR1 * 9) / 10; i += 10) {
            OCR1A = i;          // Cambia el duty cycle
            _delay_ms(10);
        }
        for (uint16_t i = (ICR1 * 9) / 10; i > ICR1 / 10; i -= 10) {
            OCR1A = i;          // Cambia el duty cycle
            _delay_ms(10);
        }
    }
}
