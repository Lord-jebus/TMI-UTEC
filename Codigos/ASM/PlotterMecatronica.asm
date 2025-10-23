;
; Plotter.asm
init:
	ldi r16, HIGH(RAMEND)
	out SPH, r16
	ldi r16, LOW(RAMEND)
	out SPL, r16

	ldi r16, 0xFF
	out DDRD, r16

posicion_inicial:
call delay
call delay
	ldi r16, 0b00001000 // desactiva valvula
    out PORTD, R16
	ldi r16, 0b00010000 // baja
    out PORTD, R16
	call delay
	call delay

	ldi r16, 0b10000000 // izquierda
    out PORTD, R16
	call delay
	call delay
	
// PUERTO
// D2: Activa valvula
// D3: Desactiva valvula
// D4: Baja
// D5: Sube
// D6: Derecha
// D7: Izquierda

start:
	
	ldi r16, 0b00000100 // activa valvula 
	out PORTD, R16
	call delay

	ldi r16, 0b00010100 // baja
    out PORTD, R16
	call delay

	ldi r16, 0b10000100 // izquierda
    out PORTD, R16
	call delay

	ldi r16, 0b00100100 // sube
    out PORTD, R16
	call delay
	
	ldi r16, 0b01000100 // derecha
	out PORTD, R16
	call delay

	ldi r16, 0b00001000 // desactiva valvula
    out PORTD, R16
	call delay

	ldi r16, 0b10000000 // izquierda
    out PORTD, R16
	call delay
	call delay

    rjmp start

delay: 
	ldi r18, 170
	ldi r19, 255
	ldi r20, 255
L1: dec r20
	brne L1
	dec r19
	brne L1
	dec r18
	brne L1
	ret