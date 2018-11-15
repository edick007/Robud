/*
 * Bi-polor Motor.c
 *
 * Created: 11/12/2018 5:28:00 PM
 * Author : Ed D
 */ 

#include <avr/io.h>

#define F_CPU 16000000UL
#include <util/delay.h>


unsigned char Motor1 = 0;
const unsigned int con = 780;

void M1step1() {
	PORTA  = 6;
	_delay_us(con);
	
}
void M1step2() {
	PORTA = 10;
	_delay_us(con);
}
void M1step3() {
	PORTA = 9;
	_delay_us(con);
}
void M1step4() {
	PORTA = 5;
	_delay_us(con);
}
void M1stopMotor() {
	PORTA = 0;

}

void M2step1() {
	PORTB = 6;
	_delay_us(con);
	
}
void M2step2() {
	PORTB = 10;
	_delay_us(con);
}
void M2step3() {
	PORTB = 9;
	_delay_us(con);
}
void M2step4() {
	PORTB = 5;
	_delay_us(con);
}
void M2stopMotor() {
	PORTB = 0;

}

void M1OnForward(){
	M1step1();
	M1step2();
	M1step3();
	M1step4();
}

void M1OnReverse(){
	M1step4();
	M1step3();
	M1step2();
	M1step1();
}

void M2OnForward(){
	M2step1();
	M2step2();
	M2step3();
	M2step4();
}

void M2OnReverse(){
	M2step4();
	M2step3();
	M2step2();
	M2step1();
}

void GoForward(){
	M2step1(); M1step1();
	M2step2(); M1step2();
	M2step3(); M1step3();
	M2step4(); M1step4();
}

void GoBackward(){
	M2step4(); M1step4();
	M2step3(); M1step3();
	M2step2(); M1step2();
	M2step1(); M1step1();
}
void TurnRight(){
		M2step4(); M1step1();
		M2step3(); M1step2();
		M2step2(); M1step3();
		M2step1(); M1step4();
}

void TurnLeft(){
	M1step4(); M2step1();
	M1step3(); M2step2();
	M1step2(); M2step3();
	M1step1(); M2step4();
}

void Stop(){
	PORTA = 0;
	PORTB = 0;
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}


int main(void)
{
	DDRA = 0xFF; PORTA = 0x00; // set Port a as output
	DDRB = 0xFF; PORTB = 0x00; // set Port a as output
	DDRC = 0x00; PORTC = 0xFF; // set Port a as input
	DDRD = 0x00; PORTD = 0xFF; // set Port a as input
    /* Replace with your application code */
    while (1) 
    {
			unsigned char TempC = ~PINC;
			unsigned char TempD = ~PIND;
		if (GetBit(TempD, 7))
		{
			GoForward();
		}
		else if (GetBit(TempC, 0))
		{
			TurnRight();	
		}
		else if (GetBit(TempC, 1))
		{
			GoBackward();
		}
		else if (GetBit(TempC, 2))
		{
			TurnLeft();
		}
		else
		{
			Stop();
		}
		
		
		
		


			
// 		for (int i=0; i<=50; i++){
// 			step1();
// 			step2();
// 			step3();
// 			step4();
// 		}
// 		stopMotor();
// 		_delay_ms(1500);
// 		
// 		for (int i=0; i<=50; i++){
// 			step3();
// 			step2();
// 			step1();
// 			step4();
// 		}
// 		
// 		stopMotor();
// 		_delay_ms(1500);
    }
}

