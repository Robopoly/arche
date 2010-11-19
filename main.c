#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED1_PORT       PORTB
#define LED2_PORT       PORTA
#define LED3_PORT       PORTA

#define LED1_DDR        DDRB
#define LED2_DDR        DDRA
#define LED3_DDR        DDRA

#define RED             0x01
#define GREEN           0x02
#define BLUE            0x04
#define YELLOW          (RED | GREEN)
#define PURPLE          (RED | BLUE)
#define CYAN            (BLUE | GREEN)
#define WHITE           (RED | GREEN | BLUE)

#define LED1            4 // amount to bit-shift
#define LED2            0
#define LED3            3

#define LEDS_OFF        LED1_PORT &= ~(WHITE << LED1); \
                        LED2_PORT &= ~(WHITE << LED2); \
                        LED3_PORT &= ~(WHITE << LED3);

#define WAIT			_delay_us(2);
#define DELAY           50

#define SPEAKER_PIN     PD5
#define SPEAKER_PWR     PD4
#define SPEAKER_PORT    PORTD
#define SPEAKER_DDR     DDRD

#define DIS_FINISH_INTS MCUCR &= ~0x0F; // disable finish line interrupts
#define EN_FINISH_INTS  MCUCR |= 0b00001010; // INT1, INT0, falling edge (10)

#define VICTORY_PORT    PORTA
#define VICTORY_LIGHT1  PA6
#define VICTORY_LIGHT2  PA7

#define IR_LED_DDR      DDRB
#define IR_LED_PORT     PORTB
#define IR_LED_BIT      PB3

#define TOGGLE_SPEAKER  SPEAKER_PORT ^= _BV(SPEAKER_PIN);
#define SPEAKER_ENABLE  SPEAKER_PORT |= _BV(SPEAKER_PWR);
#define SPEAKER_DISABLE SPEAKER_PORT &= ~(_BV(SPEAKER_PWR) | _BV(SPEAKER_PIN));
#define A_440           1136
#define A_880           568

#define DATA_PORT       PORTB
#define DATA_DDR        DDRB
#define DATA_PIN        PINB
#define DATA_BIT0       PB0
#define DATA_BIT1       PB1
//#define DATA_CLKOUT     PB3
#define DATA_CLK        PB2 // also INT2
#define DATA_IN         (DATA_PIN | ( _BV(DATA_BIT0) | _BV(DATA_BIT1))) // mask for data lines 
#define DATA_BVs        (_BV(DATA_BIT0) | _BV(DATA_BIT1))

#define COMM_START      0x01
#define COMM_TIMEOUT    0x00

#define COMM_STOP       0x00
#define COMM_CHRONO     0x01
#define COMM_DECOMPTE   0x02

void init_leds(void);
void callback_3min(void);

#ifndef	noagenda
char addNewCallback(void (* newcallbackaddr)(void), unsigned int duration, unsigned char executionNumber);
void stopCallback(char callbackNumber);
#endif

//uint8_t counter = 0; // global PWM variable
//volatile uint8_t pwm1, pwm2, pwm3 = 0;

//volatile uint8_t pwm4, pwm5, pwm6 = 0;

//volatile uint8_t pwm7, pwm8, pwm9 = 0;
volatile struct GAME_FLAGS {
    unsigned game_active:1;
    unsigned player1_win:1;
    unsigned player2_win:1;
    unsigned game_started:1;
    unsigned intermatch:1;
} game_flags;

struct DATA {
    unsigned in0:1;
    unsigned in1:1;
    unsigned out0:1;
    unsigned out1:1;
} data;

int main (void)
{
    uint8_t i = 0;

		init_leds();


        for(;;)
        {

            _delay_ms(1000);
            game_flags.game_started = 1; // set to 0 to wait for signal

            // STEP 0 : WAIT FOR INIT SIGNAL
            //while(!game_flags.game_started);
            while(PINC & _BV(PC0));

            // STEP 1 : GENERATE START SIGNAL WITH LEDs and sound
            for( i=0 ; i < 3 ; i++) 
            {
                if(i == 0)
                {
                    LED3_PORT = 0x00;
                    LED1_PORT = ((RED << LED1) | _BV(IR_LED_BIT)); // turn on first red LED
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


                // STEP 3 : WAIT FOR  END SIGNAL FROM CHRONO OR MICROSWITCHES
                // Enable external interrupts on both switch pins + "clock" line
                //EN_FINISH_INTS;
                //
                //MCUCSR |= _BV(ISC2); //  INT2 on falling edge
                //MCUCSR &= ~_BV(ISC2); //  INT2 on falling edge
                //GICR |= (_BV(INT0) | _BV(INT1) /*| _BV(INT2)*/);

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

                /*
                while(1)
                {
                    _delay_ms(200);
                    PORTC ^= 0x04;
                }
                */
                _delay_ms(3000);


                _delay_ms(5000);
                LEDS_OFF;
                game_flags.player1_win = 0;
                game_flags.player2_win = 0;

                
        }





}



void init_leds(void)
{
    LED1_PORT = 0x0F;
    LED2_PORT = 0x00;
    LED3_PORT = 0x00;

    LED1_DDR = 0xF0;
    LED2_DDR = 0xFF;
    LED3_DDR = 0xFF;

    PORTD = 0x00;
    DDRD = 0xF0;
    
    PORTC = 0x01;
    DDRC = 0xFE;

    //TCCR1B = 0b00000100;
    //TIMSK = (1 << TOIE1); // enable Timer 1 Overflow
    //TIMSK = 0x04;
    //
    // TURN ON IR LEDS
    IR_LED_DDR |= _BV(IR_LED_BIT);
    IR_LED_PORT |= _BV(IR_LED_BIT);

    //sei();
    return;
}



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

#ifndef	noagenda

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
				TCCR0 = 3;		// normal timer, fclk/64
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
