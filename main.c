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

        ir_blasters_down();

        ir_blasters_on();

        _delay_ms(1000);

        ir_blasters_rest();
        

        for(;;)
        {
            BEGINNING:
            comm_output_state(0);
            //
            // Check switch position
            //
            // Require switch to be disengaged before start
            while (read_switch() == SWITCH_UP)
                indicator_set(read_switch()); 

            // Don't start until it *is* engaged
            while (!(read_switch() == SWITCH_UP))
            {
                indicator_set(read_switch()); 
            }

            for( i=0; i<30; i++)
            {
                indicator_toggle(IND_YELLOW);
                _delay_ms(100);
                // last minute cancel!!
                if(read_switch() != SWITCH_UP)
                    goto BEGINNING;
            }

            indicator_set(IND_GREEN);


            //
            // Launch start sequence
            //

            // three red leds with beeps
            for( i=0; i<3; i++)
            {
                rgb_led_set(i+1,RED);
                play_short(A_440);
                _delay_ms(400);
            }

            // all green and ready to go
            rgb_leds_set(GREEN);
            game_flags.game_active = 1;

            // signal that game has started
            ir_blasters_off();
            comm_output_state(1);
            play_short(A_880);

            rgb_leds_set(OFF);

            _delay_ms(1000);

            ir_blasters_up();

            int winner = 0;
            while(game_flags.game_active == 1)
            {
                _delay_ms(100);
                indicator_toggle(IND_GREEN);

                // check for a winner
                winner = comm_read_winners();
                if(winner != 0)
                {
                    break;
                }
                
                // check for a reset condition
                if(read_switch() == SWITCH_DOWN)
                {
                    game_flags.game_active = 0;
                    // TODO : return to beginning
                }

            }


            // TODO : SIGNAL END OF GAME
            comm_output_state(0);

            if(winner == WINNER_RED)
                rgb_victory_dance(RED);
            else if(winner == WINNER_BLUE)
                rgb_victory_dance(BLUE);
            else if(winner == 0)
                ;
            else
                rgb_victory_dance(PURPLE);

            // TURN ON IR LEDs
            ir_blasters_on();
            ir_blasters_down();
            _delay_ms(1000);
            ir_blasters_rest();


            _delay_ms(1000);

        }





}

