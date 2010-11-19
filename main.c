#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "hardware.h"
#include "arche.h"

#define WAIT            _delay_us(2);
#define DELAY           50

void init_leds(void);
void callback_3min(void);

#ifndef noagenda
char addNewCallback(void (* newcallbackaddr)(void), unsigned int duration, unsigned char executionNumber);
void stopCallback(char callbackNumber);
#endif

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

        init_leds();
        init_arche();

        // test IR LEDS
        int j;
        for(j=0; j<4; j++)
        {
            ir_blasters_on();
            _delay_ms(1000);
            ir_blasters_off();
            _delay_ms(1000);
        }


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
                    _delay_us(A_440); // set delay for 440 Hz
                }
                SPEAKER_DISABLE;
                TCCR1B = 0x00;
                TIFR |= (1 << TOV1);

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


                /*
                // TURN OFF IR LEDS
                IR_LED_PORT &= ~_BV(IR_LED_BIT);
                game_flags.game_active = 1;

                // start timer for 0.5s
                TCCR1B = 0x03; // no prescaling

                SPEAKER_ENABLE;
                while(!(TIFR & (1 << TOV1))) // loop until timer1 overflows
                {
                    TOGGLE_SPEAKER;
                    _delay_us(A_880); // set delay for 880 Hz
                }
                SPEAKER_DISABLE;
                TCCR1B = 0x00;
                TIFR |= (1 << TOV1);

                //addNewCallback(callback_3min, 5000, 1); // 3 min - 0.5 sec
                addNewCallback(callback_3min, (18363), 5); // 3 min - 0.5 sec / 10
                _delay_ms(1000);

                //LED1_PORT = LED3_PORT = 0;
                LEDS_OFF;


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



void init_leds(void)
{


    PORTD = 0x00;
    DDRD = 0xF0;
    
    PORTC = 0x00;
    DDRC = 0x00;

    rgb_leds_init();
    init_indicators();

    //TCCR1B = 0b00000100;
    //TIMSK = (1 << TOIE1); // enable Timer 1 Overflow
    //TIMSK = 0x04;
    //
    // TURN ON IR LEDS
    //IR_LED_DDR |= _BV(IR_LED_BIT);
    //IR_LED_PORT |= _BV(IR_LED_BIT);

    //sei();
    return;
}
/*



ISR(TIMER1_OVF_vect)
{
    // TODO : timer 1 overflow
            LED1_PORT = LED2_PORT = LED3_PORT = 0;
            LED1_PORT |= (YELLOW << LED1);
}

void callback_3min(void)
{
    static uint8_t minutes = 0;
    minutes ++;
    if (minutes >= 5)
    {
        game_flags.game_active = 0;
        cli();
                LED1_PORT = LED2_PORT = LED3_PORT = 0;
                LED1_PORT |= (PURPLE << LED1);
                LED2_PORT |= (PURPLE << LED2);
                LED3_PORT |= (PURPLE << LED3);

        TCCR1B = 0x04; // prescaling 256
        TCNT1 = 0x7FFF;
        SPEAKER_ENABLE;
        while(!(TIFR & (1 << TOV1))) // loop until timer1 overflows
        {
            TOGGLE_SPEAKER;
            _delay_us(A_440); // set delay for 440 Hz
            _delay_us(A_440); // set delay for 440 Hz
        }
        SPEAKER_DISABLE;
        TCCR1B = 0x00;
        TIFR |= (1 << TOV1);
    }
}

#ifndef noagenda

void(*callbackFct[8])(void);
static volatile unsigned char callbackStatus = 0;
static volatile unsigned int timeInterval[8] = {0,0,0,0,0,0,0,0};
static volatile unsigned char numberRepetition[8] = {0,0,0,0,0,0,0,0};
static volatile unsigned char numberRepeted[8] = {0,0,0,0,0,0,0,0};
static volatile unsigned long nextExecutionTime[8] = {0,0,0,0,0,0,0,0};
static volatile unsigned long time = 0;

// max absolute time: 8589934sec
// time resolution ~2msec
char addNewCallback(void (* newcallbackaddr)(void), unsigned int duration, unsigned char executionNumber)
{
        unsigned char i;

        if(callbackStatus == 0)
        {
                //start agenda (again) !
                TCCR0 = 3;      // normal timer, fclk/64
                OCR0 = 250;
                TIMSK |= (1<<OCIE0);//(1<<TOIE0);
                TIFR &= ~((1<<OCF0)+(1<<TOV0));
                sei();
        }

        for(i=0; i<8; i++)
        {
                if((callbackStatus & (1<<i)) != 0)
                {
                        continue;
                }

                callbackStatus |= (1<<i);
                callbackFct[i] = newcallbackaddr;
                timeInterval[i] = duration;
                numberRepetition[i] = executionNumber;
                numberRepeted[i] = 0;
                nextExecutionTime[i] = time + duration;
                return i;       
        }

        return -1;
}

void stopCallback(char callbackNumber)
{
        callbackStatus &= ~(1<<callbackNumber);
        if(callbackStatus == 0)
        {
                TIMSK &= ~(1<<TOIE0);
                TCCR0=0;
        }
}

ISR(TIMER0_COMP_vect) //interruption bloquante !!
{
        unsigned char i;
        TCNT0 = 0;
        time++;
        for(i=0;i<8;i++)
        {
                if((callbackStatus & (1<<i)) != 0)
                {
                        if(nextExecutionTime[i] <= time)
                        {
                                if((numberRepeted[i] < numberRepetition[i]) || (numberRepetition[i]==0))
                                {
                                        (* callbackFct[i])();
                                        nextExecutionTime[i] = time + timeInterval[i];
                                                
                                        if((numberRepeted[i] >= (numberRepetition[i]-1)) && (numberRepetition[i] != 0))
                                        {
                                                callbackStatus &= ~(1<<i);
                                        }
                                        else
                                        {       
                                                numberRepeted[i]++;
                                        }
                                        break;
                                }
                        }
                }
        }
}


#endif
*/
