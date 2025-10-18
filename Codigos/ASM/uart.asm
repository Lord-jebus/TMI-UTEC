;------------------------------------------------------------
; Programa: Comunicación UART (eco con incremento)
; Microcontrolador: ATmega328P
;------------------------------------------------------------
; Descripción:
; - Configura la UART a 9600 baudios, 8N1 (8 bits, sin paridad, 1 stop bit).
; - Espera la recepción de un carácter.
; - Incrementa el valor ASCII recibido en +1.
; - Envía de vuelta el nuevo carácter por el puerto serial.
;------------------------------------------------------------

.include "m328pdef.inc"        ; Archivo de definiciones del ATmega328P

;------------------------------------------------------------
; Vectores de interrupción
;------------------------------------------------------------
.ORG 0x0000
    RJMP Inicio                ; Reset vector → inicio del programa

;------------------------------------------------------------
; Constantes
;------------------------------------------------------------
.equ F_CPU = 16000000          ; Frecuencia del reloj del microcontrolador (16 MHz)
.equ baud = 9600               ; Velocidad de comunicación
.equ bps = (F_CPU/16/baud) - 1 ; Valor del prescaler del baud rate (103 para 9600 baudios)

;------------------------------------------------------------
; Inicio del programa principal
;------------------------------------------------------------
Inicio:
    ; Carga el valor del baud rate en registros
    LDI R16, LOW(bps)
    LDI R17, HIGH(bps)

    ; Llama a la subrutina de inicialización UART
    RCALL initUART

;------------------------------------------------------------
; Bucle principal
;------------------------------------------------------------
loop:
    CALL getc                  ; Espera recepción de un carácter (bloqueante)
    CPI R16, 0                 ; ¿Se recibió un carácter nulo (0)?
    BREQ loop                  ; Si es así, vuelve a esperar (no procesa nada)

    LDI R18, 1
    ADD R16, R18               ; Incrementa el carácter recibido en +1
                               ; Ejemplo: si recibe 'A' (65), envía 'B' (66)

    RCALL putc                 ; Envía el nuevo carácter

    LDI R16, 0                 ; Limpia R16
    RJMP loop                  ; Repite el ciclo infinito

;------------------------------------------------------------
; Subrutina: initUART
;------------------------------------------------------------
; Configura la UART (USART0) con el baud rate deseado
; y habilita transmisión y recepción.
;------------------------------------------------------------
initUART:
    STS UBRR0L, R16            ; Carga byte bajo del divisor del baud rate
    STS UBRR0H, R17            ; Carga byte alto
    LDI R16, (1 << RXEN0) | (1 << TXEN0)
    STS UCSR0B, R16            ; Habilita transmisor y receptor
    RET

;------------------------------------------------------------
; Subrutina: putc
;------------------------------------------------------------
; Transmite un carácter (almacenado en R16) por el puerto serie.
; Espera hasta que el buffer de transmisión esté libre.
;------------------------------------------------------------
putc:
    LDS R17, UCSR0A            ; Lee el registro de estado A
    SBRS R17, UDRE0            ; ¿Buffer de transmisión vacío?
    RJMP putc                  ; Si no, espera
    STS UDR0, R16              ; Envía el carácter a transmitir
    LDI R16, 0                 ; Limpia R16 (opcional)
    RET                        ; Retorna

;------------------------------------------------------------
; Subrutina: getc
;------------------------------------------------------------
; Espera a que haya un dato recibido por UART y lo almacena en R16.
;------------------------------------------------------------
getc:
    LDS R17, UCSR0A            ; Lee el registro de estado A
    SBRS R17, RXC0             ; ¿Se recibió un dato (bit RXC0 = 1)?
    RJMP getc                  ; Si no, sigue esperando
    LDS R16, UDR0              ; Lee el carácter recibido
    RET                        ; Retorna con el dato en R16

