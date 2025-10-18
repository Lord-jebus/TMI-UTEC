;------------------------------------------------------------
; Programa: Parpadeo de LED usando Timer1 (modo normal)
; Microcontrolador: ATmega328P
;------------------------------------------------------------
; Descripción:
; Este programa configura el Timer1 para generar un retardo
; de aproximadamente 1 segundo. El LED conectado a PB5 (pin 13)
; se enciende y apaga alternadamente con cada retardo.
;------------------------------------------------------------

.include "m328pdef.inc"         ; Archivo de definición del dispositivo ATmega328P

;------------------------------------------------------------
; Vector de reset
;------------------------------------------------------------
.ORG 0x0000
    JMP INICIO                  ; Salta al inicio del programa

;------------------------------------------------------------
; Inicio del programa
;------------------------------------------------------------
INICIO:
    ;--------------------------------------------------------
    ; Inicialización del puntero de pila (Stack Pointer)
    ;--------------------------------------------------------
    LDI R16, HIGH(RAMEND)       ; Carga la parte alta de la dirección final de RAM
    OUT SPH, R16                ; Configura el byte alto del puntero de pila
    LDI R16, LOW(RAMEND)        ; Carga la parte baja de la dirección final de RAM
    OUT SPL, R16                ; Configura el byte bajo del puntero de pila

    ;--------------------------------------------------------
    ; Configuración del pin PB5 como salida (LED en Arduino UNO)
    ;--------------------------------------------------------
    LDI R16, (1 << DDB5)        ; Pone a '1' el bit DDB5 → PB5 como salida
    OUT DDRB, R16

    ;--------------------------------------------------------
    ; Configuración del Timer1
    ;--------------------------------------------------------
    ; Modo normal (cuenta desde TCNT1 hasta 0xFFFF y desborda)
    LDI R16, 0
    STS TCCR1A, R16             ; TCCR1A = 0 → modo normal

    ; Prescaler = 256 (CS12=1, CS11=0, CS10=0)
    LDI R16, (1 << CS12)
    STS TCCR1B, R16             ; TCCR1B = 00000100b → reloj/256

    LDI R16, 0
    STS TCCR1C, R16             ; Registro de control adicional, sin uso aquí

    ; No se habilitan interrupciones del timer
    LDI R16, 0
    STS TIMSK1, R16

    ; Carga inicial del contador (para obtener 1 segundo aprox.)
    ; A 16 MHz con prescaler 256 → 1 tick = 16 μs
    ; 1 s / 16 μs = 62500 ticks
    ; Como el contador desborda a 65536 → 65536 - 62500 = 3036
    LDI R16, HIGH(3036)
    STS TCNT1H, R16
    LDI R16, LOW(3036)
    STS TCNT1L, R16

    SEI                         ; Habilita interrupciones globales (no se usan aquí, pero buena práctica)

;------------------------------------------------------------
; Bucle principal
;------------------------------------------------------------
VOLVER:
    SBI PORTB, PORTB5           ; Enciende LED (pone PB5 en alto)
    CALL RETARDO1S              ; Espera ~1 segundo
    CBI PORTB, PORTB5           ; Apaga LED (pone PB5 en bajo)
    CALL RETARDO1S              ; Espera ~1 segundo
    RJMP VOLVER                 ; Repite indefinidamente

;------------------------------------------------------------
; Subrutina de retardo usando Timer1
;------------------------------------------------------------
; Espera hasta que ocurra un desbordamiento del Timer1
; y recarga el contador para el siguiente ciclo.
;------------------------------------------------------------
RETARDO1S:
    SBIS TIFR1, TOV1            ; ¿El bit de overflow (TOV1) está en 1?
    RJMP RETARDO1S              ; Si no, sigue esperando

    SBI TIFR1, TOV1             ; Limpia el flag de overflow (se borra escribiendo un 1)

    ; Recarga el valor inicial en TCNT1 (3036)
    LDI R16, HIGH(3036)
    STS TCNT1H, R16
    LDI R16, LOW(3036)
    STS TCNT1L, R16

    RET                         ; Regresa a la rutina que llamó (1 segundo ha pasado)

;------------------------------------------------------------
; Fin del programa
;------------------------------------------------------------

