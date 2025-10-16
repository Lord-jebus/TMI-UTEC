#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#define LED_PIN 6   // Pin conectado a la Matriz
#define WIDTH   8
#define HEIGHT  8
#define NUM_LEDS (WIDTH * HEIGHT)

uint8_t leds[NUM_LEDS][3];  // [R,G,B] para cada LED

// Función para enviar un bit al WS2812B
void sendBit(uint8_t bitVal) {
	if (bitVal) {
		// T1H ~0.8us en alto
		PORTD |= (1 << LED_PIN);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"  // "\n\t" salto de linea en la programacion
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		);
		PORTD &= ~(1 << LED_PIN);
		// T1L ~0.45us en bajo
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		);
		} else {
		// T0H ~0.4us en alto
		PORTD |= (1 << LED_PIN);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t"
		);
		PORTD &= ~(1 << LED_PIN);
		// T0L ~0.85us en bajo
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		);
	}
}

// Envía un byte (MSB primero)
void sendByte(uint8_t byte) {
	for (uint8_t i = 0; i < 8; i++) {
		sendBit(byte & (1 << (7 - i)));
	}
}

// Refresca toda la tira/matriz (GRB ordenado)
void show(uint8_t (*colors)[3]) {
	cli();  // Desactivar interrupciones para mantener timing
	for (int i = 0; i < NUM_LEDS; i++) {
		sendByte(colors[i][1]); // G
		sendByte(colors[i][0]); // R
		sendByte(colors[i][2]); // B
	}
	sei();
	_delay_us(60); // Reset >=50us
}

void setLedRGB(uint8_t (*leds)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b) {
	if (ledIndex < 0 || ledIndex >= NUM_LEDS) return; // Evitar desbordes
	leds[ledIndex][0] = r;
	leds[ledIndex][1] = g;
	leds[ledIndex][2] = b;
}
	
int main(void) {
	
	DDRD |= (1 << LED_PIN);

	while (1) {
		setLedRGB(leds, 0, 255, 255, 255);  
		setLedRGB(leds, 7, 255, 0, 0);
		setLedRGB(leds, 56, 0, 255, 0);
		setLedRGB(leds, 63, 0, 0, 255);
		show(leds);
		_delay_ms(500);
		
		setLedRGB(leds, 0, 0, 0, 0);
		setLedRGB(leds, 7, 0, 0, 0);
		setLedRGB(leds, 56, 0, 0, 0);
		setLedRGB(leds, 63, 0, 0, 0);
		show(leds);
		_delay_ms(500);
		show(leds);
		_delay_ms(500);
		
		setLedRGB(leds, 0, 120, 120, 120); 
		setLedRGB(leds, 9, 120, 120, 120);
		setLedRGB(leds, 18, 120, 120, 120);
		setLedRGB(leds, 27, 120, 120, 120);
		setLedRGB(leds, 36, 120, 120, 120);
		setLedRGB(leds, 45, 120, 120, 120);
		setLedRGB(leds, 54, 120, 120, 120);
		setLedRGB(leds, 63, 120, 120, 120);
		show(leds);
		_delay_ms(500);

		
		setLedRGB(leds, 0, 0, 0, 0);
		setLedRGB(leds, 9, 0, 0, 0);
		setLedRGB(leds, 18, 0, 0, 0);
		setLedRGB(leds, 27, 0, 0, 0);
		setLedRGB(leds, 36, 0, 0, 0);
		setLedRGB(leds, 45, 0, 0, 0);
		setLedRGB(leds, 54, 0, 0, 0);
		setLedRGB(leds, 63, 0, 0, 0);
		show(leds);
		_delay_ms(500);
	}
}