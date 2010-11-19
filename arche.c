#include <avr/io.h>

#include "hardware.h"
#include "arche.h"

#include <util/delay.h>

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
// Selector Switch
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
            return 0;
        case 0x03:
            return 1;
        case 0x02:
            return 2;
    }
    return 3;
}

// ----------------------------
// Indicator LEDs
// ----------------------------
void indicator_set(uint8_t _colour)
{
    STATUS_PORT |= _BV(_colour);
    return;
}

void indicator_clear(uint8_t _colour)
{
    STATUS_PORT &= ~_BV(_colour);
    return;
}

void indicator_toggle(uint8_t _colour)
{
    STATUS_PORT ^= _BV(_colour);
    return;
}


