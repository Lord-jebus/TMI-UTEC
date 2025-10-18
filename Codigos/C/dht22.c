#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define numeroBits 50
char myBuffer = '0';

#define DHT_DDR DDRD
#define DHT_PORT PORTD
#define DHT_PIN	PIND
#define PIN 2


void USART_Init(unsigned int ubrr) {
	cli();

	// USART como UART
	UCSR0C &=~ (1<<UMSEL00);
	UCSR0C &=~ (1<<UMSEL01);

	// Paridad desactivada
	UCSR0C &=~ (1<<UPM00);
	UCSR0C &=~ (1<<UPM01);

	// Stops = 1
	UCSR0C &=~ (1<<USBS0);

	// Datos de 8 bits
	UCSR0C |=  (1<<UCSZ00);
	UCSR0C |=  (1<<UCSZ01);
	UCSR0B &=~ (1<<UCSZ02);
	
	// Calculo del baudrate
	UCSR0A |= (1<<U2X0);
	UBRR0 = (F_CPU/8/9600) - 1;

	UCSR0B |= (1<<TXEN0);
	UCSR0B |= (1<<RXEN0);

	UCSR0B |= (1<<RXCIE0);

	sei();
}

void USART_Transmit(char *data) {
	for(int i = 0; data[i]; i++) {
		UDR0 = data[i];
	}
}


void UART_write(unsigned char caracter){
	while(!(UCSR0A&(1<<5)));    // mientras el registro UDR0 esté lleno espera
	UDR0 = caracter;            //cuando el el registro UDR0 está vacio se envia el caracter
}

void UART_write_txt(char* cadena){			//cadena de caracteres de tipo char
	while(*cadena !=0x00){				//mientras el último valor de la cadena sea diferente a el caracter nulo
		UART_write(*cadena);			//transmite los caracteres de cadena
		cadena++;						//incrementa la ubicación de los caracteres en cadena
		//para enviar el siguiente caracter de cadena
	}
}

void DHT22_init(void){
	DHT_DDR |= (1<<PIN);		//PIN COMO SALIDA
	DHT_PORT |= (1<<PIN);		//NIVEL ALTO
}

/*		  18ms	   40us	    80us	80us
	¯¯¯¯|_______|¯¯¯¯¯¯¯¯|_______|¯¯¯¯¯¯¯|....datos....
	
	---------PUC---------|----------dht22-------------|      */					

uint8_t DHT22_read(float *dht_temperatura, float *dht_humedad){
	uint8_t bits[5];
	uint8_t i,j=0;
	uint8_t contador = 0;
	
	//Paso 1, enviar un puslo en bajo durante18ms 
	
	DHT_PORT &= ~(1<<PIN);		//Nivel bajo
	_delay_ms(18);
	DHT_PORT |= (1<<PIN);		//Nivel alto
	DHT_DDR &= ~(1<<PIN);		//Pin como entrada
	
	
	//Paso 2 esperamos 20 a 40us hasta que el dht22 envie 0
	contador = 0;
	while(DHT_PIN & (1<<PIN))
	{
		_delay_us(2);
		contador += 2;
		if (contador > 60)
		{
			DHT_DDR |= (1<<PIN);	//Pin como salida
			DHT_PORT |= (1<<PIN);	//Nivel alto
			return 0;
		}	
	}	

	//Paso 3 esperamos 80us hasta que el dht22 envie 1
	contador = 0;
	while(!(DHT_PIN & (1<<PIN)))
	{
		_delay_us(2);
		contador += 2;
		if (contador > 100)
		{
			DHT_DDR |= (1<<PIN);	//Pin como salida
			DHT_PORT |= (1<<PIN);	//Nivel alto
			return 0;
		}	
	}

	//Paso 4 esperamos 80us hasta que el dht22 envie 0
	contador = 0;
	while(DHT_PIN & (1<<PIN))
	{
		_delay_us(2);
		contador += 2;
		if (contador > 100)
		{
			DHT_DDR |= (1<<PIN);	//Pin como salida
			DHT_PORT |= (1<<PIN);	//Nivel alto
			return 0;
		}
	}

	//Paso 5 leemos los 40 bits o 5 bytes
	for (j=0; j<5; j++)
	{
	uint8_t result=0;
		for (i=0; i<8; i++)
		{
			while (!(DHT_PIN & (1<<PIN)));
				_delay_us(35);

			if (DHT_PIN & (1<<PIN))
				result |= (1<<(7-i));
					
			while(DHT_PIN & (1<<PIN));
		}
		bits[j] = result;
	}

	DHT_DDR |= (1<<PIN);	//Pin como salida
	DHT_PORT |= (1<<PIN);	//Nivel alto
	
	//Paso 6 convertir la temperatura y humedad
	
	/*Ejemplo	humedad 25.8  
					258
	*/			
	
	if ((uint8_t) (bits[0] + bits[1] + bits[2] +bits[3]) == bits[4])		//Pregunta por el chekin
	{
		uint16_t rawhumedad = bits[0]<<8 | bits[1];
		uint16_t rawtemperatura = bits[2] <<8 | bits[3];
	
		
		if (rawtemperatura & 0x8000)
		{
			*dht_temperatura = (float)((rawtemperatura & 0x7fff) / 10.0)* -1.0;
		}else{
			*dht_temperatura = (float)(rawtemperatura)/10.0;
		}

		*dht_humedad = (float)(rawhumedad)/10.0;

		return 1;
	}
	return 1;
}


// Función para realizar la comunicación con el sensor DHT22
uint8_t dht22_read2(uint8_t *data) {
	// Configura el pin del sensor como salida
	DDRC |= (1 << DDC0);
	// Coloca el pin en estado bajo durante al menos 18 ms para reiniciar el sensor
	PORTC &= ~(1 << PORTC0);
	_delay_ms(20);
	// Cambia el pin a entrada y espera la respuesta del sensor
	DDRC &= ~(1 << DDC0);
	_delay_us(40);

	// Verifica la respuesta del sensor (debería ser un pulso de 80us bajo seguido de un pulso de 80us alto)
	if ((PINC & (1 << PINC0)) == 0) {
		_delay_us(80);
		if (PINC & (1 << PINC0)) {
			_delay_us(80);
			} else {
			return 0; // Error: no se recibió la respuesta esperada del sensor
		}
		} else {
		return 0; // Error: el sensor no respondió correctamente
	}

	// Leer los datos desde el sensor
	for (uint8_t i = 0; i < 5; ++i) {
		uint8_t byte = 0;

		// Leer cada bit del byte
		for (uint8_t j = 0; j < 8; ++j) {
			// Espera hasta que el sensor envíe un pulso alto
			while (!(PINC & (1 << PINC0)));
			_delay_us(30); // El pulso bajo indica 0, el pulso alto indica 1
			if (PINC & (1 << PINC0)) {
				byte |= (1 << (7 - j));
			}
			// Espera hasta que el sensor vuelva a un pulso bajo
			while (PINC & (1 << PINC0))
			;
		}

		data[i] = byte;
	}

	// Verifica la suma de verificación
	if (data[4] == (uint8_t)(data[0] + data[1] + data[2] + data[3])) {
		return 1; // Lectura exitosa
		} else {
		return 0; // Error: la suma de verificación no coincide
	}
}


void USART_transmit_char(char data) {
	// Espera a que el registro de datos esté vacío
	while (!(UCSR0A & (1 << UDRE0)))
	;
	// Envía el carácter
	UDR0 = data;
}

// Función para enviar una cadena de caracteres a través del puerto serial
void USART_transmit_string(const char *str) {
	while (*str) {
		USART_transmit_char(*str);
		str++;
	}
}


int main(void)
{
	char datos[numeroBits] = "Test Program\n\r";
	float temperatura;
	float humedad;
	
	uint8_t contador=200;
	
	USART_Init(103);//9600 baudios
	USART_Transmit(datos);
	DHT22_init();
	uint8_t dht22_data[5];
	while (1)
	{
		
if (dht22_read2(dht22_data)) {
				// Imprime la temperatura y la humedad a través del puerto serial
				char buffer[50];
				snprintf(buffer, sizeof(buffer), "Temp: %u.%u C, Humidity: %u.%u%%\n\r",
				dht22_data[2], dht22_data[3], dht22_data[0], dht22_data[1]);
				USART_transmit_string(buffer);
				} else {
				USART_transmit_string("Error al leer el sensor DHT22\n\r");
			}

			// Espera antes de tomar otra lectura (ajusta según sea necesario)
			_delay_ms(2000);
		
	}
}
