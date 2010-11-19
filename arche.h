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

void rgb_leds_init(void);
void rgb_led_set(uint8_t _led, uint8_t _colour);
void rgb_leds_set(uint8_t _colour);
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
void ir_blasters_on(void);
void ir_blasters_off(void);
void ir_blasters_up(void);
void ir_blasters_down(void);

// ----------------------------
// External communication
// ----------------------------
#define WINNER_BLUE 1
#define WINNER_RED  2
#define WINNER_TIE  3

void init_communications(void);
void comm_output_state(uint8_t _state);
uint8_t comm_read_winners(void);

#endif
