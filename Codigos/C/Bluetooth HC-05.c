#define F_CPU 16000000UL // Frecuencia del CPU

#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600 //105 para 9600 - 26 para 38400
#define MY_UBRR F_CPU/16/BAUD-1

#define RecGPIO_delay 26 //26us
#define SendGPIO_delay 25.275 //25.275us

#define TX_PIN PB0
#define RX_PIN PB1

void initSerial() {
	DDRB |= (1 << TX_PIN);
	DDRB &= ~(1 << RX_PIN);
}

void sendByteGPIO(uint8_t data) {
	// Enviar un byte a través de PB0 (TX)
	PORTB &= ~(1 << TX_PIN); // Start bit
	_delay_us(SendGPIO_delay); // Esperar

	for (int i = 0; i < 8; i++) {
		if (data & (1 << i)) {
			PORTB |= (1 << TX_PIN); // Enviar bit alto
			} else {
			PORTB &= ~(1 << TX_PIN); // Enviar bit bajo
		}
		_delay_us(SendGPIO_delay); // Esperar por el siguiente bit
	}

	PORTB |= (1 << TX_PIN); // Stop bit
	_delay_us(SendGPIO_delay); // Esperar antes de enviar el siguiente byte
}

uint8_t receiveByteGPIO() {
	uint8_t data = 0;

	// Esperar a que el pin RX (PB1) sea bajo (start bit)
	while (PINB & (1 << RX_PIN));

	_delay_us(RecGPIO_delay); // Esperar a que el primer bit se estabilice

	for (int i = 0; i < 8; i++) {
		if (PINB & (1 << RX_PIN)) {
			data |= (1 << i); // Leer bit
		}
		_delay_us(RecGPIO_delay); // Esperar por el siguiente bit
	}

	// Esperar a que el pin RX sea alto (stop bit)
	while (!(PINB & (1 << RX_PIN)));
	
	return data;
}

void USART_Init() {
	UBRR0H = (MY_UBRR >> 8); // Configurar el registro de baudios alto
	UBRR0L = MY_UBRR;        // Configurar el registro de baudios bajo
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Habilitar receptor y transmisor
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits de datos, sin paridad, 1 bit de parada
}

void USART_Transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0))); 
	UDR0 = data; 
}

unsigned char USART_Receive(void) {
	while (!(UCSR0A & (1 << RXC0))); 
	return UDR0;
}

int main() {

	initSerial();
	USART_Init();
	
	while (1) {
		if (!(PINB & (1 << RX_PIN))) {
			uint8_t receivedGPIO = receiveByteGPIO();
			USART_Transmit(receivedGPIO); // Enviar el byte recibido al Monitor Serial
			
		}
		
		if (UCSR0A & (1 << RXC0)) {
			uint8_t receivedUSART = USART_Receive();
			sendByteGPIO(receivedUSART); // Enviar el byte recibido desde el Monitor Serial al HC-06

		}
	}
	
	return 0;
}