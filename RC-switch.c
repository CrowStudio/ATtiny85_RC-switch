/*
 * RC-switch.c
 * 
 * Copyright 2015 Daniel Arvidsson <daniel.arvidsson@crowstudio.se>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
 /* 
 * The ATtiny85 RC-switch reads the PWM-signal on channel one's pin 
 * (throttle) from a OrangeRx R615 receiver, it works as a polarity 
 * switch for a DC-motor controlled with a cheap electrical speed 
 * controller (ESC) that has no reverse. I have programmed my Spectrum 
 * DX7 radio's throttle stick to have zero speed in the centre and 
 * forward speed both up and down.
 * 
 * The main function of the ATtiny85 will check the position of the 
 * throttle stick and do the following: 
 * When you move the stick up, the motor will go into forward motion, 
 * and when you move the stick back to the centre - 1500 µs - the motor 
 * stops. Once the stick's position will go below 1480 µs (or other 
 * value of your choice) the switch will change the polarity of the 
 * motor and it will go into reverse motion, when it comes back to 
 * centre the motor once again stops. If you then again move the 
 * stick's position above 1520 µs (or other value of your choice) the 
 * switch will change the polarity back to it's origin and the motor 
 * will once again go into forward motion.
 *  
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>
 
#define F_CPU 8000000UL			//defines clock as 8Mhz

#define debugPin PORTB4		 //defines PB4 as debugPin
#define relayPin PORTB2		 //defines PB2 as relayPin
#define greenLED PORTB1		 //defines PB1 as greenLED
#define redLED PORTB0		//defines PB0 as redLED

// volatile uint8_t pulse;				// global variable to store pulse length
volatile uint8_t tot_overflow;			// global variable to count the number of Timer/Counter1 overflows
volatile uint16_t pulse16;			//global variable to make a 16 bit integer from tot_overflow and TCNT1
volatile uint8_t pInt;				// global variable to indicate PCINT


int main(void)
{
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2 | 1 << DDB4);		//sets PB0, PB1, PB2 and PB4 as output pins
	
//	TCCR1 |= (1 << CS13);		//set Timer/Counter1 to increment every 16 us
	TCCR1 |= (1 << CS12); 		 //set Counter/Timer1 prescaler to increment every 1µs
	
	GIMSK |= (1 << PCIE);		//enable pin change interrupt
	PCMSK |= (1 << PCINT3); 	//mask PB3 as pin chang interrupt pin
	TIMSK |= (1 << TOIE1);		//enable Counter/Timer1 overflow interrupt
	sei();			//enable gloabal interrupt
	
	while(1)		//leave and/or put your own code here
	{
		if(pInt == 1)			//PCNINT-if-statment
		{
//			pulse = TCNT1;			//saves Timer/Counter1 into pulse variable
			
//			PORTB |= (1 << debugPin);		//pin is HIGH on when interrupt is intialized
			
			pulse16 = (tot_overflow << 8) | TCNT1;			//adds tot_overflow and TCNT1 to be able to set if-statements in PCINT-while-loop with µs
				
			if(PINB & (1 << PINB3))			//if PB3 is HIGH
			{
				TCNT1 = 0;		//resets Timer/Counter1
				tot_overflow = 0;		//resets tot_overflow variable
			}
			
			else 		
			{ 
				if (pulse16 >1555)			//when stick 1 travels from 1555 µs towards 2006 µs
//				if((tot_overflow == 5 && pulse > 240) || tot_overflow > 6)			
				{
					PORTB &= ~(1 << relayPin);		  //relay pole switch, + & - on motor 
					PORTB |= (1 << greenLED);		 //LED green indicates forward motion
					PORTB &= ~(1 << redLED);		//turn off red LED
				}
				
					else if (pulse16 <1490)			//when stick 1 travels from 1490 ms towards 920 µs
//					else if((tot_overflow == 5 && pulse < 200) || tot_overflow  < 5)
					{
						PORTB |= (1 << relayPin);		 //relay pole switch, - & + on motor 
						PORTB &= ~(1 << greenLED);		  //turn off green LED
						PORTB |= (1 << redLED);			//LED red indicates backward motion
					}	
				
				else        //if µs is 1490> or <1555 - dead-span to prevent gliteches on relay when stick is in centre position
				{ 
//					PORTB |= (1 << greenLED);			//for debug to indicate dead-span	
//					PORTB |= (1 << redLED);			//for debug to indicate dead-span	
				}	

			}
							
			pInt = 0;		//resets pInt to exit PCNINT-if-statment
		}
		
		else
		{
			
		}
			
	}
	
}

ISR(TIMER1_OVF_vect)			//when Counter/Timer1 overflows
{
    tot_overflow++;			// keeps track of Counter/Timer1's overflow
}

ISR(PCINT0_vect)    	//when pin-level changes on PB3
{ 
	pInt = 1;			//indicates PCINT
}
