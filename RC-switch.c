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
 
#include <avr/io.h>
#include <avr/interrupt.h>
 
#define F_CPU 8000000UL			//defines clock as 8Mhz
#define PORTB4 = debugPin		//defines PB4 as debuPin

int main(void)
{
	GIMSK |= (1 << PCIE);		//enable pin change interrupt
	PCMSK |= (1 << PCINT3); 	//mask PB3 as pin chang interrupt pin
	sei();			//enable global interrupt
	
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2 | 1 << DDB4);		//sets PB0, PB1, PB2 and PB4 as output pins

	TCCR1 |= (1 << CS13); 		 //set Timer/Counter1 to increment every 16 us
	
	while(1)		//leave blank or put your own code here
	{
		
	}
	
}

ISR(PCINT0_vect)    	//when pin level changes on PB3
{ 
	static uint8_t pulse;

	pulse = TCNT1;			//saves Timer/Counter1 into pulse variable
	
	PORTB |= (1 << debugPin);		//pin is HIGH on when interrupt is intialized

	if(PINB & (1 << PINB3))			//if PB3 is HIGH
	{
		TCNT1 = 0;		//resets Timer/Counter1
	}
	
	else
	{
		if(pulse > 96)			//when stick 1 travels from 1.52 ms towards 2.06 ms
		{
			PORTB &= ~(1 << PORTB2);		//relay pole switch, + & - on motor 
			PORTB |= (1 << PORTB1);			//LED green indicates forward motion
			PORTB &= ~(1 << PORTB0);		//turn off red LED
		}
			
			else if (pulse < 93) 		//when stick 1 travels from 1.48 ms towards 0.92 ms
			{
				PORTB |= (1 << PORTB2);			//relay pole switch, - & + on motor 
				PORTB &= ~(1 << PORTB1);		//turn off green LED
				PORTB |= (1 << PORTB0);			//LED red indicates backward motion
			}	
			
		else 	//if µs is 1480> or <1520 - dead span to prevent glitches on relay when stick is in centre position
		{
			
		}
		
	}
	
}
