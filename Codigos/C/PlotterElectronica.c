
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600 
#define MY_UBRR F_CPU/16/BAUD-1
 
#define CLK_Y   PC3 //EJE Y
#define DIR_Y    PC4
#define EN_Y PC5

#define CLK_X   PB3 //EJE X
#define DIR_X    PB4
#define EN_X PB5

#define SOLENOID PC0
#define LIMIT_YD PD3
#define LIMIT_YA PD2

#define LED PD5

// Función para inicializar los pines
void init_pins() {
	// Configurar los pines CLK, DIR y ENABLE como salidas
	DDRC |= (1 << CLK_Y) | (1 << DIR_Y) | (1 << EN_Y); // EJE Y
	
	DDRB |= (1 << CLK_X) | (1 << DIR_X) | (1 << EN_X); // EJE X
		
	DDRC |= (1 << SOLENOID);
	
	DDRD |= (1 << LED);
	
	// Desactivar el motores al iniciar
	PORTB &= ~(1 << EN_X);
	PORTC &= ~(1 << EN_Y);
}

// Función para establecer la dirección del motor
void set_directionY(uint8_t direction) {
	if (direction) {
		PORTC |= (1 << DIR_Y);  // Establecer dirección hacia un sentido
		} else {
		PORTC &= ~(1 << DIR_Y); // Establecer dirección hacia el sentido opuesto
	}
}

// Función para establecer la dirección del motor
void set_directionX(uint8_t direction) {
	if (direction) {
		PORTB |= (1 << DIR_X);  // Establecer dirección hacia un sentido
		} else {
		PORTB &= ~(1 << DIR_X); // Establecer dirección hacia el sentido opuesto
	}
}

// Función para activar o desactivar el motor
void set_enableY(uint8_t enable) {
	if (enable) {
		PORTC |= (1 << EN_Y);  // Desactivar el motor (ENABLE HIGH)
		} else {
		PORTC &= ~(1 << EN_Y); // Activar el motor (ENABLE LOW)
		
	}
}

void set_enableX(uint8_t enable) {
	if (enable) {
		PORTB |= (1 << EN_X);  // Desactivar el motor (ENABLE HIGH)
		} else {
		PORTB &= ~(1 << EN_X); // Activar el motor (ENABLE LOW)
		
	}
}
// Función para mover el motor un pasoY
void step_motorY() {
	PORTC |= (1 << CLK_Y);
	_delay_us(1); 
	PORTC &= ~(1 << CLK_Y);
	_delay_us(1);  
}

// Función para mover el motor un pasoX
void step_motorX() {
	PORTB |= (1 << CLK_X);
	_delay_us(1); 
	PORTB &= ~(1 << CLK_X);
	_delay_us(1); 
}

// Prueba LED
void LED_f() {
	PORTD |= (1 << LED);
	_delay_ms(200);
	PORTD &= ~(1 << LED);
	_delay_ms(200);
}

void cuadrado(){
	PORTC &= ~(1 << SOLENOID); //BAJA LAPIZ
	_delay_ms(100);
	
	//ARRIBA
	set_enableY(1);   // Activar el motor
	set_directionY(1); // Establecer dirección inicial
	
	for (int i = 0; i < 200; i++) {   // Mover el motorY
		step_motorY();
		_delay_ms(1);
	}
	
	set_enableY(0);
	_delay_ms(500);
	LED_f();
	
	//DERECHA
	set_enableX(1);
	set_directionX(0);
	
	for (int i = 0; i < 200; i++) {
		step_motorX();
		_delay_ms(1);
	}
	
	set_enableX(0);
	_delay_ms(500);
	LED_f();
	
	//ABAJO
	set_enableY(1);
	set_directionY(0);
	
	for (int i = 0; i < 200; i++) {
		step_motorY();
		_delay_ms(1);
	}
	
	set_enableY(0);
	_delay_ms(500);
	LED_f();
	
	//IZQUIERDA
	set_enableX(1);
	set_directionX(1);
	
	for (int i = 0; i < 200; i++) {
		step_motorX();
		_delay_ms(1);
	}
	
	set_enableX(0);
	_delay_ms(500);
	LED_f();
	
	PORTC |= (1 << SOLENOID); // SUBE LAPIZ
	_delay_ms(100);
}


void USART_Init() {
	UBRR0H = (MY_UBRR >> 8); // Configurar el registro de baudios alto
	UBRR0L = MY_UBRR;        // Configurar el registro de baudios bajo
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Habilitar receptor y transmisor
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits de datos, sin paridad, 1 bit de parada
}

void USART_Transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0))); // Esperar hasta que el buffer esté vacío
	UDR0 = data; // Enviar el dato
}

unsigned char USART_Receive(void) {
	while (!(UCSR0A & (1 << RXC0))); // Esperar hasta que los datos estén disponibles
	return UDR0; // Leer el dato
}


int main(void) {
	init_pins();
	
	DDRD |= (1 << LED);
	while (1) { 
		cuadrado();
		USART_Transmit('R');
		LED_f();
		
		_delay_ms(100);
	}
		
	set_enableX(0);
	set_enableY(0);
	
}