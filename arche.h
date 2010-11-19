/************************************************
 *      Arche.h
 *
 *    Définitions de fonctions pour l'arche
 *
 *    Andrew Watson - Novembre 2010
 ************************************************/
#ifndef ARCHE_H
#define ARCHE_H

// ----------------------------
// RGB Power LEDs
// ----------------------------

/*
 * Function definitions
 */

// set ports and DDRs for power leds
void rgb_leds_init(void);

// Set LED colour as bitmask
// 0 = turn off
void rgb_led_set(uint8_t _led, uint8_t _colour);

// Set colour on all at once
void rgb_leds_set(uint8_t _colour);

// announce victory for a robot
void rgb_victory_dance(uint8_t _colour);


// ----------------------------
// Selector Switch
// ----------------------------
uint8_t read_switch(void);

// ----------------------------
// Indicator LEDs
// ----------------------------
void indicator_set(uint8_t _colour);
void indicator_clear(uint8_t _colour);
void indicator_toggle(uint8_t _colour);

#endif
