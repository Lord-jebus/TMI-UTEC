#define F_CPU 16000000UL     // Frecuencia del reloj del microcontrolador (16 MHz)
#include <avr/io.h>          // Librería principal de E/S
#include <util/delay.h>      // Librería para usar _delay_ms y _delay_us
#include <stdint.h>          // Tipos de datos enteros estándar (uint8_t, uint16_t, etc.)

// ------------------------------
// Definiciones de pines
// ------------------------------
#define TRIG_DDR   DDRB       // Registro de dirección del puerto del pin TRIG
#define TRIG_PORT  PORTB      // Registro de salida del puerto del pin TRIG
#define TRIG_BIT   PB1        // Pin PB1 (pin físico 15 en ATmega328P)

// ------------------------------
// Inicialización del Timer1 para captura de entrada (Input Capture)
// ------------------------------
static void t1_icp_init(void) {
	// Modo normal (TCCR1A = 0)
	TCCR1A = 0;

	// Configura:
	// - ICNC1: habilita el filtro de ruido en el pin ICP1
	// - ICES1: detecta flanco de subida inicialmente
	// - CS11 y CS10: prescaler de 64 → cada tick = 4 µs (a 16 MHz)
	TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS11) | (1 << CS10);

	// Limpia las banderas de captura y desbordamiento previas
	TIFR1  = (1 << ICF1) | (1 << TOV1);
}

// ------------------------------
// Genera el pulso de disparo (TRIG) al sensor HC-SR04
// Duración aproximada: 10 microsegundos
// ------------------------------
static void trig_pulse(void) {
	TRIG_PORT &= ~(1 << TRIG_BIT);  // Asegura que TRIG inicie en bajo
	_delay_us(2);
	TRIG_PORT |= (1 << TRIG_BIT);   // Envía un pulso alto
	_delay_us(12);                  // Mantiene el pulso por ~10 µs
	TRIG_PORT &= ~(1 << TRIG_BIT);  // Finaliza el pulso (baja)
}

// ------------------------------
// Mide la duración del pulso ECHO usando el pin ICP1 (PB0)
// Retorna el número de ticks entre el flanco de subida y bajada
// ------------------------------
static uint16_t measure_echo_ticks(void) {
	uint16_t rise, fall;   // Variables para almacenar el inicio y fin del pulso
	uint32_t guard;        // Variable para temporización de seguridad (timeout)

	// Reinicia el contador y las banderas
	TCNT1 = 0;
	TIFR1 = (1 << ICF1) | (1 << TOV1);

	// Espera por el flanco de subida (inicio del pulso ECHO)
	TCCR1B |= (1 << ICES1);      // Captura flanco de subida
	guard = 60000UL;             // Límite de espera (~240 ms aprox.)
	while (!(TIFR1 & (1 << ICF1))) {
		if (--guard == 0) return 0; // Si se agota el tiempo, retorna 0
	}
	rise = ICR1;                 // Guarda el valor del contador al inicio del pulso
	TIFR1 = (1 << ICF1);         // Limpia la bandera de captura

	// Espera por el flanco de bajada (fin del pulso ECHO)
	TCCR1B &= ~(1 << ICES1);     // Cambia a detección de flanco de bajada
	guard = 60000UL;
	while (!(TIFR1 & (1 << ICF1))) {
		if (--guard == 0) return 0; // Timeout si no llega el flanco
	}
	fall = ICR1;                 // Guarda el valor del contador al final del pulso
	TIFR1 = (1 << ICF1);         // Limpia la bandera

	// Si el valor de bajada es menor que el de subida, hay error → retorna 0
	if (fall <= rise) return 0;

	// Retorna el número total de ticks (duración del pulso)
	return (uint16_t)(fall - rise);
}

// ------------------------------
// Programa principal
// ------------------------------
int main(void) {
	uint16_t distancia_cm = 0;  // Variable donde se guardará la distancia medida

	// Configura el pin TRIG como salida
	TRIG_DDR |= (1 << TRIG_BIT);
	TRIG_PORT &= ~(1 << TRIG_BIT);  // Inicialmente en bajo

	// Inicializa el Timer1 en modo captura de entrada
	t1_icp_init();

	while (1) {
		// Envía un pulso al sensor para iniciar la medición
		trig_pulse();

		// Mide la duración del pulso de respuesta (ECHO)
		uint16_t ticks = measure_echo_ticks();

		// Si se obtuvo una medición válida
		if (ticks > 0) {
			// Convierte los ticks en distancia (centímetros)
			// Aproximación: distancia (cm) = (ticks * 2) / 29
			// Cada tick ≈ 4 µs, el sonido recorre ~29 µs por cm ida y vuelta
			distancia_cm = ((uint32_t)ticks * 2u) / 29u;
		}

		// Pequeño retardo antes de la siguiente medición
		_delay_ms(100);
	}
}
