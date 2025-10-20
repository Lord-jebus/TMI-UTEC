#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "src/UART.h"
#include "src/SPI.h"
#include "src/RC522.h"

#define BAUD 9600
#define MY_UBRR F_CPU/16/BAUD-1

// ==== MAIN ====
int main(void) {
	uart_init(MY_UBRR);
	spi_init();
	mfrc522_resetPinInit();
	mfrc522_init();
	_delay_ms(100);

	char card_uid[10] = {0};

	//mfrc522_debug_REQA(); //Funcion debug
	//_delay_ms(500);
		
	while (1) {
		mfrc522_standard(card_uid);
		_delay_ms(500);
		uart_print("Recibido: ");	
		if(card_uid[0] != 0)
			uart_print_hex_array(card_uid, 5);
		uart_print("\n");	
		_delay_ms(500);
	}
}