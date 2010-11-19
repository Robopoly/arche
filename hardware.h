#ifndef HARDWARE_H
#define HARDWARE_H

#ifndef F_CPU
#define F_CPU   8000000UL
#endif

// ----------------------------
// RGB Power LEDs
// ----------------------------

/*
 * Port and pin definitions
 */ 
#define LED1_PORT       PORTA
#define LED2_PORT       PORTA
#define LED3_PORT       PORTC

#define LED1_DDR        DDRA
#define LED2_DDR        DDRA
#define LED3_DDR        DDRC

#define RED             0x01
#define GREEN           0x02
#define BLUE            0x04

#define OFF             0

#define YELLOW          (RED | GREEN)
#define PURPLE          (RED | BLUE)
#define CYAN            (BLUE | GREEN)
#define WHITE           (RED | GREEN | BLUE)

// location of first pin
#define LED1            0
#define LED2            3
#define LED3            3 // amount to bit-shift

/*
 * Macros
 */
#define LEDS_OFF        LED1_PORT &= ~(WHITE << LED1); \
                        LED2_PORT &= ~(WHITE << LED2); \
                        LED3_PORT &= ~(WHITE << LED3);

// ----------------------------
// Speaker drivers
// ----------------------------

/*
 * Port and pin definitions
 */ 
#define SPEAKER_PIN     PD6
#define SPEAKER_PWR     PD7
#define SPEAKER_PORT    PORTD
#define SPEAKER_DDR     DDRD

/*
 * Macros
 */
#define TOGGLE_SPEAKER  SPEAKER_PORT ^= _BV(SPEAKER_PIN);
#define SPEAKER_ENABLE  SPEAKER_PORT |= _BV(SPEAKER_PWR);
#define SPEAKER_DISABLE SPEAKER_PORT &= ~(_BV(SPEAKER_PWR) | _BV(SPEAKER_PIN));
#define A_440           1136
#define A_880           568


// ----------------------------
// Indicator LEDs
// ----------------------------

/*
 * Port and pin definitions
 */ 
#define STATUS_PORT     PORTB
#define STATUS_DDR      DDRB
#define STATUS_GREEN    PB4
#define STATUS_RED      PB3
#define STATUS_YELLOW   PB2

#define STATUS_MASK     (_BV(STATUS_RED) | _BV(STATUS_GREEN) | _BV(STATUS_YELLOW))

/*
 * Macros
 */

// ----------------------------
// Selector Switch
// ----------------------------

/*
 * Port and pin definitions
 */ 
#define SWITCH_DDR      DDRC
#define SWITCH_PORT     PORTC
#define SWITCH_PINS     PINC

#define SWITCH_A        PC6
#define SWITCH_B        PC7

#define SWITCH_UP       0
#define SWITCH_MIDDLE   1
#define SWITCH_DOWN     2

/*
 * Macros
 */
// return 2-bit mask of switch positions
#define READ_SWITCH     (PINC & (_BV(PC6) | _BV(PC7)) >> PC6)


// ----------------------------
// IR Blasters
// ----------------------------

/*
 * Port and pin definitions
 */ 
#define IR_LED_DDR      DDRB
#define IR_LED_PORT     PORTB

#define IR_LED_LEFT     PB0
#define IR_LED_RIGHT    PB1

/*
 * Macros
 */

#endif
