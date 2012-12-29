/*
 * wakeup.c
 *
 * Created: 2012-02-13 21:46:39
 *  Author: Oskari Rundgren
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "ports2.h"
#include "wakeup.h"

uint8_t Seconds;	// Count seconds
uint8_t PrevEnc;	// Rotary encoder value

/* clock modes */
uint8_t Modes;
#define m_clk		0	// display clock
#define m_btn		1	// button is pressed
#define m_setclk	2	// set and view alarm
#define m_alm		3	// alarm is triggered
#define m_almon		4	// alarm enabled
#define m_sleep		5	// sleep mode
#define m_touch		6	// touch sensor triggered
#define m_beep		7	// beep beep

uint8_t BtnCounter;
uint16_t SetAlmCounter;
uint16_t CfmAlmCounter;
uint16_t SetClkCounter;
uint16_t CfmClkCounter;
uint16_t BeepCounter;

uint16_t AlarmTime;		// Alarm time in minutes
uint16_t RealTime;		// Real time in minutes

int main(void)
{
	RealTime = RealTimeDefault;
	AlarmTime = AlarmTimeDefault;
	CfmAlmCounter = 0;
	SetAlmCounter = 0;
	SetClkCounter = 0;
	CfmClkCounter = 0;
	
	/* Config Ports */
	ENCP |= (1<<ENCA) | (1<<ENCB);			// encoder pullups
	DDRB |= (1<<REDLED) | (1<<GRNLED);	// led ports out
	BTNP |= (1<<BTN);						// button pullup
	SEGDDR |= 0x7F;							// 7seg out
	FETD |= (1<<FET1) | (1<<FET2) | (1<<FET3) | (1<<FET0);		// fets out
	DDRB |= (1<<PB1);	// Touch sensor power
	PORTB |= (1<<PB1);	// Touch sensor power

	/* pin change interrupt */
	PCICR |= (1<<PCIE0) | (1<<PCIE1);		// enable PC-interrupts 0 and 1
	PCMSK0 |= (1<<PCINT2) | (1<<PCINT3);		// Button interrupt
	PCMSK1 |= (1<<PCINT8);	// Rotary rotate interrupt
	
	/* Timer2, 32khz timer */
	ASSR |= (1<<AS2);
	OCR2A = 128;
	TCNT2 = 0;
	TCCR2A = 0;
	TCCR2B = (6<<CS20);  // prescaler 256, overflow=2sec
	TIMSK2 |= (1<<TOIE2);
	while (ASSR & 0b00011111);
	TIFR2 = 0;
	
	/* Timer0, Refresh 7-segment display */
	TCCR0B |= (1<<CS01); //prescaler 8
	TIMSK0 |= (1<<TOIE0); //overflow interrupt
	
	/* Timer1, Piezo speaker output*/
	SPEAKER_ddr |= (1<<SPEAKER);		// speaker out
	TCCR1B |= (1<<CS11) | (1<<WGM12);	// prescaler 8, clear timer on compare
	TIMSK1 |= (1<<OCIE1A);				// compare interrupt
	
	OCR1A = 0x0F;
	
	sei();	// enable interrupts
	
    while(1)
    {
	}
}

/* Timer1 Compare */
ISR(TIMER1_COMPA_vect)
{
	if (Modes & (1<<m_beep))
		SPEAKER_pin |= (1<<SPEAKER);
	else
		SPEAKER_port &= ~(1<<SPEAKER);
}

/* 400Hz Timer */
ISR(TIMER0_OVF_vect)
{
	if (BeepCounter){
		if (--BeepCounter)
			Modes |= (1<<m_beep);
		else
			Modes &= ~(1<<m_beep);
	}
	
	/* ## touch sensor ## */
	if (TOUCHPIN & (1<<TOUCH)){
		Modes |= (1<<m_touch);
		GRNLEDP |= (1<<GRNLED);
	} else {
		Modes &= ~(1<<m_touch);
		GRNLEDP &= ~(1<<GRNLED);
	}				
	
	/* we count if button is pressed down long */
	if (Modes & (1<<m_btn))
	{
		if(BtnCounter < 0xFF)
			BtnCounter++;
	}
	
	if (Modes & (1<<m_setclk))
	{
		
	}
	
	if (Modes & (1<<m_alm) && Modes & (1<<m_touch))
		BeepCounter = 100; // continuous beep
	
	
	if (SetAlmCounter)  // if we are in alarm setting mode
	{
		if (--SetAlmCounter)
			PrintTime(AlarmTime);		
		else
			CfmAlmCounter = 300;
	}
	else if (CfmAlmCounter)   // was in alarm settings mode, waiting for timeout
	{
		if (--CfmAlmCounter / 32 % 2)   // this makes the display blink
		{
			PrintTime(AlarmTime);
			OCR1A = 0x0F;	// beep frequency
			TCNT1 = 0;		// reset timer
			BeepCounter = 30;	// beep time
		}
		else
		{
			FETP &= ~(1<<FET0) & ~(1<<FET1) & ~(1<<FET2) & ~(1<<FET3);					
		}
	}
	else if (SetClkCounter)		// if we are in real time setting mode
	{
		if (--SetClkCounter)
			PrintTime(RealTime);
		else
			CfmClkCounter = 300;
	}	
	else if (CfmClkCounter)		// was in real time settings mode, waiting for timeout
	{
		if (--CfmClkCounter / 32 % 2)	// makes display blink
		{
			PrintTime(RealTime);
			OCR1A = 0x0F;	// beep frequency
			TCNT1 = 0;		// reset timer
			BeepCounter = 30;	// beep time
		}
		else
		{
			FETP &= ~(1<<FET0) & ~(1<<FET1) & ~(1<<FET2) & ~(1<<FET3);
		}
	}
	else	// just print real time, normal mode
	{
		PrintTime(RealTime);
	}
}

/* Interrupt triggered by button */
ISR(PCINT1_vect)
{
	if (BTNPIN & (1<<BTN))  // Button released
	{
		Modes &= ~(1<<m_btn);
		
		if (BtnCounter == 0xFF)
			{
				ShortBeep();
				// Enter set real time mode.
				//Modes |= (1<<m_setclk);
				SetClkCounter = 0x2FF;
				
			}
			
		else
			{	
				// Leave or don't enter set real time mode.
				//Modes &= ~(1<<m_setclk);
				ShortBeep();
				if (Modes & (1<<m_almon)){
					REDLEDP &= ~(1<<REDLED);
					Modes &= ~(1<<m_almon);
				}					
				else {
					REDLEDP |= (1<<REDLED);
					Modes |= (1<<m_almon);
				}					
			}
			
		BtnCounter = 0; // Ok, we can stop counting button press now. 
	}
	else
	{
		// Button pressed
		Modes |= (1<<m_btn);
	}
}

/* Interrupt triggered by rotary encoder */
ISR(PCINT0_vect)
{
	OCR1A = 0xFF;	// beep frequency
	TCNT1 = 0;		// reset timer
	BeepCounter = 10;

	uint8_t readenc = 0;

	if (ENCPIN & (1<<ENCA))		// Read encoder pins.
		readenc |= 1;
	if (ENCPIN & (1<<ENCB))		// Read encoder pins.
		readenc |= 2;
		
	if(!(Modes & (1<<m_btn)))	// don't respond to rotary encoder if button is pressed
	{
			/* Check if encoder rotated clockwise */
		if ((readenc == 0 && PrevEnc == 2) || 
			(readenc == 1 && PrevEnc == 0) || 
			(readenc == 2 && PrevEnc == 3) || 
			(readenc == 3 && PrevEnc == 1))
		{
			if (SetClkCounter)
			{
				if (++RealTime == 1440) RealTime = 0;
				SetClkCounter = 0x2FF;
			}
			else
			{
				if (++AlarmTime == 1440) AlarmTime = 0;	
				SetAlmCounter = 0x2FF;
			}
			//TimerX = 0;			
		}

			/* Check if encoder rotated counter-clockwise */
		if ((readenc == 2 && PrevEnc == 0) || 
			(readenc == 0 && PrevEnc == 1) || 
			(readenc == 3 && PrevEnc == 2) || 
			(readenc == 1 && PrevEnc == 3))
		{
			if (SetClkCounter)
			{		
				if (--RealTime == 0xFFFF) RealTime = 1439;
				SetClkCounter = 0x2FF;
			}
			else
			{
				if (--AlarmTime == 0xFFFF) AlarmTime = 1439;
				SetAlmCounter = 0x2FF;
			}		
			//TimerX = 0;
		}
	}	
	
	PrevEnc = readenc;	// Store previous value of encoder to detect direction 
}

/* Asynchronous 32kHz crystal Timer, 0.5Hz */
ISR(TIMER2_OVF_vect)
{
	if (++Seconds == 30)		//  if we get 60 seconds, that makes a minute
	{
		if (++RealTime == 1440)	// 1440 minutes makes 24 hours
			RealTime = 0;
			
		Seconds = 0;
	}		
	
	if (AlarmTime == RealTime && Modes & (1<<m_almon))	// alarm is triggered
		Modes |= (1<<m_alm);
}


void ledprint(uint8_t c)
{
	switch (c)
		{
		case 0:
			SEGPORT = SEG0;
			break;
		case 1:
			SEGPORT = SEG1;
			break;
		case 2:
			SEGPORT = SEG2;
			break;
		case 3:
			SEGPORT = SEG3;
			break;
		case 4:
			SEGPORT = SEG4;
			break;
		case 5:
			SEGPORT = SEG5;
			break;
		case 6:
			SEGPORT = SEG6;
			break;
		case 7:
			SEGPORT = SEG7;
			break;
		case 8:
			SEGPORT = SEG8;
			break;
		case 9:
			SEGPORT = SEG9;
			break;
		default:
			break;
		}
}


void PrintTime(uint16_t minutes)
{
		if (FETP & (1<<FET3))
	{
		ledprint(minutes % 60 % 10);
		FETP &= ~(1<<FET1) & ~(1<<FET2) & ~(1<<FET3);
		FETP |= (1<<FET0);
	}
	else if (FETP & (1<<FET0))
	{
		ledprint(minutes % 60 / 10);
		FETP &= ~(1<<FET0) & ~(1<<FET2) & ~(1<<FET3);
		FETP |= (1<<FET1);
	}
	else if (FETP & (1<<FET1))
	{
		ledprint(minutes / 60 % 10);
		FETP &= ~(1<<FET0) & ~(1<<FET1) & ~(1<<FET3);
		FETP |= (1<<FET2);
	}
	else
	{
		ledprint(minutes / 600);
		FETP &= ~(1<<FET0) & ~(1<<FET1) & ~(1<<FET2);
		FETP |= (1<<FET3);
	}
}

void ShortBeep(void)
{
	OCR1A = 0x0E;	// beep frequency
	TCNT1 = 0;		// reset timer
	BeepCounter = 100;	
}