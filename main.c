#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "hardware.h"
#include "arche.h"

volatile struct GAME_FLAGS {
    unsigned game_active:1;
    unsigned player1_win:1;
    unsigned player2_win:1;
    unsigned game_started:1;
    unsigned intermatch:1;
} game_flags;


int main (void)
{
    uint8_t i = 0;

        init_arche();

        // test IR LEDS
        /*
        int j;
        for(j=0; j<4; j++)
        {
            ir_blasters_on();
            _delay_ms(1000);
            ir_blasters_off();
            _delay_ms(1000);
        }
        */


        for(;;)
        {
            {
                int a = 0;
                a = read_switch();
                switch(a)
                {
                    case SWITCH_UP:
                        rgb_leds_set(RED);
                        indicator_set(IND_RED);
                        break;
                    case SWITCH_MIDDLE:
                        rgb_leds_set(YELLOW);
                        indicator_set(IND_YELLOW);
                        break;
                    case SWITCH_DOWN:
                        rgb_leds_set(GREEN);
                        indicator_set(IND_GREEN);
                        break;
                    default:
                        rgb_leds_set(BLUE);
                        break;
                }
            }

            //_delay_ms(1000);
            //rgb_led_set(1, WHITE);


            rgb_victory_dance(RED);
            _delay_ms(1000);
            _delay_ms(1000);
            _delay_ms(1000);
            rgb_victory_dance(BLUE);

            game_flags.game_started = 1; // set to 0 to wait for signal

            // STEP 0 : WAIT FOR INIT SIGNAL
            //while(!game_flags.game_started);
            //while(PINC & _BV(PC0));

            // STEP 1 : GENERATE START SIGNAL WITH LEDs and sound
            for( i=0 ; i < 3 ; i++) 
            {
                if(i == 0)
                {
                    LED3_PORT = 0x00;
                }
                if(i == 1)
                    LED2_PORT |= (RED << LED2); // add second LED
                if(i == 2)
                    LED3_PORT |= (RED << LED3); // add third


                play_short(A_440);

                _delay_ms(400); // wait a total of 500 + 200 = 700ms
            }
                // STEP 2 :: TURN ON ALL GREEN LEDS AND BEEP HIGHER

                LED1_PORT = LED2_PORT = LED3_PORT = 0;
                LED1_PORT |= (GREEN << LED1);
                LED2_PORT |= (GREEN << LED2);
                LED3_PORT |= (GREEN << LED3);

                // TODO : start 3 minute timer
                /*
                TCNT0 = 100; // just to be sure...
                TCCR0 = 0b101; // prescaler = 1024
                TIMSK |= (1 << TOIE0);
                */


                
                // TURN OFF IR LEDS
                game_flags.game_active = 1;

                play_short(A_880);
                /*


                sei();


                while(game_flags.game_active == 1)
                {
                    _delay_ms(100);
                    PORTC ^= 0x04;
                }


                // TURN ON IR LEDs
                IR_LED_PORT |= _BV(IR_LED_BIT);

                // TODO : SIGNAL END OF GAME
                LED1_PORT |= (BLUE << LED1);

                //while(1)
                //{
                //    _delay_ms(200);
                //    PORTC ^= 0x04;
                //}
                _delay_ms(3000);


                _delay_ms(5000);
                LEDS_OFF;
                game_flags.player1_win = 0;
                game_flags.player2_win = 0;

*/
                
        }





}

