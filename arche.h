/************************************************
 *      Arche.h
 *
 *    Définitions de fonctions pour l'arche
 *
 *    Andrew Watson - Novembre 2010
 ************************************************/
#ifndef ARCHE_H
#define ARCHE_H

void init_arche(void);

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
// Speaker drivers
// ----------------------------
void init_speakers(void);


// ----------------------------
// Selector Switch
// ----------------------------
void init_switch(void);
uint8_t read_switch(void);

// ----------------------------
// Indicator LEDs
// ----------------------------
#define IND_RED     1
#define IND_YELLOW  2
#define IND_GREEN   3

void init_indicators(void);
void indicator_set(uint8_t _colour);
void indicator_clear(uint8_t _colour);
void indicator_toggle(uint8_t _colour);

// ----------------------------
// IR Blasters
// ----------------------------
void init_ir_blasters(void);

#endif
