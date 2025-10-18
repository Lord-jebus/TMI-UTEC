;------------------------------------------------------------
; Programa de ejemplo con interrupciones externas en ATmega328P
;------------------------------------------------------------
; Descripción general:
; - Configura las interrupciones externas INT0 (PD2) e INT1 (PD3).
; - Cada interrupción ejecuta una rutina diferente (RSI_0 o RSI_1).
; - En cada rutina, se muestran secuencias distintas en el puerto B.
; - Incluye un retardo de aproximadamente 1 ms entre cada cambio.
;------------------------------------------------------------

.include "m328pdef.inc"        ; Archivo con las definiciones del ATmega328P

;------------------------------------------------------------
; Vector de interrupciones
;------------------------------------------------------------
.ORG 0x0000
    RJMP Inicio                ; Reset vector → salta al inicio del programa

.ORG 0x0002
    RJMP RSI_0                 ; Vector de interrupción INT0 (PD2)

.ORG 0x0004
    RJMP RSI_1                 ; Vector de interrupción INT1 (PD3)

;------------------------------------------------------------
; Inicio del programa principal
;------------------------------------------------------------
Inicio:
    SEI                        ; Habilita las interrupciones globales

    ; Configura el puerto B como salida (para mostrar patrones con LEDs)
    LDI   R16, 0xFF
    OUT   DDRB, R16            ; Todos los pines de PORTB como salida

    ; Activa resistencias pull-up internas en los pines PD2 e PD3 (INT0 e INT1)
    SBI   PORTD, 2             ; Activa pull-up en PD2
    SBI   PORTD, 3             ; Activa pull-up en PD3

    ; Inicializa el puntero de pila (Stack Pointer)
    LDI   R17, 0x08
    OUT   SPL, R16             ; Byte bajo del puntero de pila
    OUT   SPH, R17             ; Byte alto del puntero de pila

    ;--------------------------------------------------------
    ; Configuración de interrupciones externas
    ;--------------------------------------------------------

    ; Habilita INT0 y INT1
    LDI   R18, 0x03
    OUT   EIMSK, R18           ; EIMSK = 0b00000011 → INT1 e INT0 habilitadas

    ; Configura las interrupciones para activarse en flanco de subida
    LDI   R19, 0x0F
    STS   EICRA, R19           ; ISC01=ISC00=1 → INT0 flanco de subida
                               ; ISC11=ISC10=1 → INT1 flanco de subida

;------------------------------------------------------------
; Bucle principal: espera indefinidamente
;------------------------------------------------------------
Wait:
    RJMP Wait                  ; Espera a que ocurra una interrupción

;------------------------------------------------------------
; Rutina de Servicio de Interrupción INT0 (PD2)
;------------------------------------------------------------
RSI_0:
    ; Secuencia de LEDs encendiéndose de izquierda a derecha

    LDI   R20, 0x01
    OUT   PORTB, R20
    RCALL Mseg                 ; Espera breve

    LDI   R20, 0x02
    OUT   PORTB, R20
    RCALL Mseg

    LDI   R20, 0x04
    OUT   PORTB, R20
    RCALL Mseg

    LDI   R20, 0x08
    OUT   PORTB, R20

    RETI                       ; Fin de la interrupción → restaura el contexto

;------------------------------------------------------------
; Rutina de Servicio de Interrupción INT1 (PD3)
;------------------------------------------------------------
RSI_1:
    ; Secuencia de LEDs encendiéndose de derecha a izquierda

    LDI   R20, 0x08
    OUT   PORTB, R20
    RCALL Mseg

    LDI   R20, 0x04
    OUT   PORTB, R20
    RCALL Mseg

    LDI   R20, 0x02
    OUT   PORTB, R20
    RCALL Mseg

    LDI   R20, 0x01
    OUT   PORTB, R20

    RETI                       ; Fin de la interrupción

;------------------------------------------------------------
; Subrutina de retardo (Mseg)
;------------------------------------------------------------
; Genera un retardo aproximado (depende de la frecuencia del reloj)
; Usa tres registros como contadores anidados
;------------------------------------------------------------
Mseg:
    LDI  R21, 21
    LDI  R22, 75
    LDI  R23, 189

L1:
    DEC  R23
    BRNE L1                    ; Bucle interno
    DEC  R22
    BRNE L1
    DEC  R21
    BRNE L1
    NOP
    RET

;------------------------------------------------------------
; Fin del programa
;------------------------------------------------------------

