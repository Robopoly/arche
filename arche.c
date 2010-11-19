#include <avr/io.h>

#include "hardware.h"
#include "arche.h"

#include <util/delay.h>
#include <avr/interrupt.h>

void init_arche(void)
{
    rgb_leds_init();
    init_indicators();
    init_switch();
    init_speakers();
    init_ir_blasters();
    init_communications();
}


// ----------------------------
// RGB Power LEDs
// ----------------------------
void rgb_leds_init(void)
{
    // init ports OFF
    LED1_PORT &= ~(WHITE << LED1);
    LED2_PORT &= ~(WHITE << LED2);
    LED3_PORT &= ~(WHITE << LED3);
    // set DDRS as outputs
    LED1_DDR |= (WHITE << LED1);
    LED2_DDR |= (WHITE << LED2);
    LED3_DDR |= (WHITE << LED3);
}

// ----------------------------
     
void rgb_led_set(uint8_t _led, uint8_t _colour)
{
    switch(_led)
    {
        case 1:
            // turn all colours off
            LED1_PORT &= ~(WHITE << LED1);
            // selectively add in desired colours
            LED1_PORT |= (_colour << LED1);
            break;
        case 2:
            // turn all colours off
            LED2_PORT &= ~(WHITE << LED2);
            // selectively add in desired colours
            LED2_PORT |= (_colour << LED2);
            break;
        case 3:
            // turn all colours off
            LED3_PORT &= ~(WHITE << LED3);
            // selectively add in desired colours
            LED3_PORT |= (_colour << LED3);
            break;
    }
}

// ----------------------------

void rgb_leds_set(uint8_t _colour)
{
    rgb_led_set(1, _colour);
    rgb_led_set(2, _colour);
    rgb_led_set(3, _colour);
}

// ----------------------------

#define VIC_RUNS        3
#define VIC_RUNSPEED    150
#define VIC_FLASHES     6
#define VIC_FLASHSPEED  200
void rgb_victory_dance(uint8_t _colour)
{
    rgb_leds_set(OFF);
    uint8_t i = 0;

    for(i=0; i<VIC_RUNS*3; i++)
    {
        rgb_led_set(i%3+1, _colour);
        _delay_ms(VIC_RUNSPEED);
        rgb_led_set(i%3+1, OFF);
    }
    for(i=0; i<VIC_FLASHES; i++)
    {
        rgb_leds_set(_colour);
        _delay_ms(VIC_FLASHSPEED);
        rgb_leds_set(OFF);
        _delay_ms(VIC_FLASHSPEED);
    }
}
// ----------------------------
// Speaker drivers
// ----------------------------
void init_speakers(void)
{
    SPEAKER_PORT &= ~(_BV(SPEAKER_PIN) | _BV(SPEAKER_PWR));
    SPEAKER_DDR  |=  (_BV(SPEAKER_PIN) | _BV(SPEAKER_PWR));

    return;
}

// ----------------------------

void play_short(uint8_t note)
{
    // start timer for 0.5s
    TCNT1 = 0;
    TIFR |= (1 << TOV1);
    TCCR1B = 0x03; // no prescaling
    //TIFR &= ~(1 << TOV1); // clear flag
    TIMSK &= ~(1 << TOIE1); // disable timer 1 overflow


    SPEAKER_ENABLE;
    while(!(TIFR & (1 << TOV1))) // loop until timer1 overflows
    {
        TOGGLE_SPEAKER;
        _delay_us(note); // set delay for 440 Hz
    }
    SPEAKER_DISABLE;
    TCCR1B = 0x00;
    TIFR |= (1 << TOV1);
}


// ----------------------------
// Selector Switch
// ----------------------------
void init_switch(void)
{
    SWITCH_PORT &= ~(_BV(SWITCH_A) | _BV(SWITCH_B));
    SWITCH_DDR  &= ~(_BV(SWITCH_A) | _BV(SWITCH_B));

    return;
}

// ----------------------------

uint8_t read_switch(void)
{
    uint8_t val = 0;
    //val = READ_SWITCH;
    val = PINC;
    val &= 0b11000000;
    val >>= 6;

    switch (val)
    {
        case 0x01:
            return SWITCH_UP;
        case 0x03:
            return SWITCH_MIDDLE;
        case 0x02:
            return SWITCH_DOWN;
    }
    return 0;
}

// ----------------------------
// Indicator LEDs
// ----------------------------
void init_indicators(void)
{
    STATUS_DDR |= (
            _BV(STATUS_GREEN) |
            _BV(STATUS_YELLOW) |
            _BV(STATUS_RED)
            );
    return;
}

// ----------------------------

void indicator_set(uint8_t _colour)
{
    
    STATUS_PORT &= ~(STATUS_MASK);
    switch(_colour)
    {
        case IND_RED:
            STATUS_PORT |= _BV(STATUS_RED);
            break;
        case IND_GREEN:
            STATUS_PORT |= _BV(STATUS_GREEN);
            break;
        case IND_YELLOW:
            STATUS_PORT |= _BV(STATUS_YELLOW);
            break;
        case IND_OFF:
            break;
    }
    return;
}

// ----------------------------

void indicator_clear(uint8_t _colour)
{
    // TODO
    return;
}

// ----------------------------

void indicator_toggle(uint8_t _colour)
{
    static uint8_t previous_state = 0;
    if (previous_state == 0)
    {
        indicator_set(_colour);
        previous_state = 1;
    }
    else
    {
        indicator_set(OFF);
        previous_state = 0;
    }
    return;
}

// ----------------------------
// IR Blasters
// ----------------------------
void init_ir_blasters(void)
{
    IR_LED_PORT &= ~(_BV(IR_LED_LEFT) | _BV(IR_LED_RIGHT));
    IR_LED_DDR  |=  (_BV(IR_LED_LEFT) | _BV(IR_LED_RIGHT));

    SERVO_PORT  &= ~SERVO_MASK;
    SERVO_DDR   |=  SERVO_MASK;
    return;
}

// ----------------------------

void ir_blasters_on(void)
{
    IR_LED_PORT |= IR_LED_MASK;
}

// ----------------------------

void ir_blasters_off(void)
{
    IR_LED_PORT &= ~IR_LED_MASK;
}

// ----------------------------
// INTERNAL FUNCTIONS
// ----------------------------
#define OCR_OFFSET  383
void set_pwm(uint16_t left, uint16_t right)
{
    // 8 bit fast pwm
    TCCR1A =    (0b11 << WGM10) | // WGM11:10
                (0b11 << COM1A0) |
                //(0b11 << FOC1B) | // FOC1A:B
                (0b11 << COM1B0);
    TCCR1B =    (0b01 << WGM12) | // WGM13:12
                (0b100 << CS10); // prescaler 256
    OCR1A = OCR_OFFSET+left;
    OCR1B = OCR_OFFSET+right;
    TIMSK |= _BV(TOIE1);
    sei();
}

void disable_pwm(void)
{
    TCCR1A = 0;
    TCCR1B = 0;
    TIMSK &= ~_BV(TOIE1);
}

ISR(TIMER1_OVF_vect)
{
    TCNT1 = OCR_OFFSET;
    PORTD ^= 0xFF;
}

// ---------------------------
// SERVO Functions
// ---------------------------

void ir_blasters_up(void)
{
    set_pwm(32, 64);

    return;
}

// ----------------------------

void ir_blasters_down(void)
{
    // TODO
    return;
}

// ----------------------------

void ir_blasters_rest(void)
{
    // TODO
    return;
}

// ----------------------------
// External communication
// ----------------------------

void init_communications(void)
{
    COMM_PORT &= ~COMM_OUTMASK;
    COMM_DDR  |=  COMM_OUTMASK;
    COMM_DDR  &= ~COMM_INMASK;

    // activate internal pullups
    COMM_PORT |=  COMM_INMASK;

    return;
}

// ----------------------------

void comm_output_state(uint8_t _state)
{
    if(_state == 0)
        COMM_PORT &= ~_BV(COMM_OUT);
    else
        COMM_PORT |=  _BV(COMM_OUT);

    return;
}

// ----------------------------

uint8_t comm_read_winners(void)
{
    // reactivate internal pullups just in case
    COMM_PORT |=  COMM_INMASK;

    // read input pins
    uint8_t input = COMM_PINS & COMM_INMASK;

    // bit-shift to LSB positions
    input >>= COMM_WIN_BLUE;

    // default state is 1 (internal pullups!)
    switch(input)
    {
        case 0x03:
            return 0;
        case 0x02:
            return WINNER_BLUE;
        case 0x01:
            return WINNER_RED;
        case 0x00:
            return WINNER_TIE;
    }

    // hopefully this will never happen
    return 0xFF;
}
