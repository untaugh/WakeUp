/*
 * ports.h
 *
 * Created: 2012-02-13 21:47:50
 *  Author: Oskari Rundgren
 */ 


#ifndef PORTS_H_
#define PORTS_H_

/* Settings Begin */

#define AlarmCooldown 5 // alarm cooldown in minutes when touch not allowed
#define RealTimeDefault 1320
#define AlarmTimeDefault 1321

/* Settings End */

#define REDLEDPIN PINB
#define	REDLEDP	PORTB
#define REDLED	PB5
#define GRNLEDPIN PINB
#define GRNLEDP	PORTB
#define GRNLED	PB4

#define SEGPORT PORTD
#define SEGDDR	DDRD

#define ALL 0b01111111



#define SEGP
#define SEGA	PD5
#define SEGB	PD6
#define SEGC	PD4
#define SEGD	PD3
#define SEGE	PD2
#define SEGF	PD0
#define SEGG	PD1

#define	SEG0	(1<<SEGA) | (1<<SEGB) | (1<<SEGC) | (1<<SEGD) | (1<<SEGE) | (1<<SEGF)
#define	SEG1	(1<<SEGB) | (1<<SEGC)
#define	SEG2	(1<<SEGA) | (1<<SEGB) | (1<<SEGG) | (1<<SEGD) | (1<<SEGE)
#define	SEG3	(1<<SEGA) | (1<<SEGB) | (1<<SEGC) | (1<<SEGD) | (1<<SEGG)
#define	SEG4	(1<<SEGB) | (1<<SEGC) | (1<<SEGF) | (1<<SEGG)
#define	SEG5	(1<<SEGA) | (1<<SEGC) | (1<<SEGD) | (1<<SEGF) | (1<<SEGG)
#define	SEG6	(1<<SEGC) | (1<<SEGD) | (1<<SEGE) | (1<<SEGF) | (1<<SEGG) | (1<<SEGA)
#define	SEG7	(1<<SEGA) | (1<<SEGB) | (1<<SEGC)
#define	SEG8	(1<<SEGA) | (1<<SEGB) | (1<<SEGC) | (1<<SEGD) | (1<<SEGE) | (1<<SEGF) | (1<<SEGG)
#define	SEG9	(1<<SEGA) | (1<<SEGB) | (1<<SEGC) | (1<<SEGF) | (1<<SEGG) | (1<<SEGD)

#define FETP	PORTC
#define FETD	DDRC
#define FET0	PC2
#define FET1	PC3
#define FET2	PC4
#define FET3	PC5

#define SPEAKER_ddr		DDRC
#define SPEAKER_port	PORTC
#define SPEAKER_pin		PINC
#define SPEAKER			PC1

#define BTNP	PORTC
#define BTNPIN	PINC
#define BTN		PC0

#define SNSPIN	PINB
#define SNS		PB0

#define ENCPIN	PINB
#define ENCP	PORTB
#define ENCA	PB3
#define ENCB	PB2

#define TOUCHPIN	PINB
#define TOUCH		PINB0

#endif /* PORTS_H_ */