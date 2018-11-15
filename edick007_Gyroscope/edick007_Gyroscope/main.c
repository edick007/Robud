

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define INT_ENABLE 0x38
#define PWR_MGMT_1 0x6B
#define SMPLRT_DIV 0x19




#include <avr/io.h>
#include <util/delay.h>							
#include <math.h>								
#include <inttypes.h>									
#include <stdlib.h>										
#include <stdio.h>									



#define LED PORTB				



#ifndef USART_RS232_H_FILE_H_				/* Define library H file if not defined */
#define USART_RS232_H_FILE_H_

#define F_CPU 8000000UL						/* Define CPU clock Frequency e.g. here its 8MHz */
#include <avr/io.h>							/* Include AVR std. library file */
#define BAUD_PRESCALE (((F_CPU / (BAUDRATE * 16UL))) - 1)	/* Define prescale value */


//I2c Headers AND FUNCTION ***********************************************************************************

#define SCL_CLK 100000L							/* Define SCL clock frequency */
#define BITRATE(TWSR)	((F_CPU/SCL_CLK)-16)/(2*pow(4,(TWSR&((1<<TWPS0)|(1<<TWPS1))))) /* Define bit rate */

void I2C_Init();								
uint8_t  I2C_Start(char);						
uint8_t  I2C_Repeated_Start(char);			
void I2C_Stop();								
void I2C_Start_Wait(char);					
uint8_t  I2C_Write(char);						
char I2C_Read_Ack();
char I2C_Read_Nack();							



//I2c Headers AND FUNCTION ***********************************************************************************

void I2C_Init()												/* I2C initialize function */
{
	TWBR = BITRATE(TWSR = 0x00);							/* Get bit rate register value by formula */
}


uint8_t I2C_Start(char slave_write_address)						/* I2C start function */
{
	uint8_t status;											/* Declare variable */
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);					/* Enable TWI, generate start condition and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (start condition) */
	status = TWSR & 0xF8;									/* Read TWI status register with masking lower three bits */
	if (status != 0x08)										/* Check weather start condition transmitted successfully or not? */
	return 0;												/* If not then return 0 to indicate start condition fail */
	TWDR = slave_write_address;								/* If yes then write SLA+W in TWI data register */
	TWCR = (1<<TWEN)|(1<<TWINT);							/* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (Write operation) */
	status = TWSR & 0xF8;									/* Read TWI status register with masking lower three bits */
	if (status == 0x18)										/* Check weather SLA+W transmitted & ack received or not? */
	return 1;												/* If yes then return 1 to indicate ack received i.e. ready to accept data byte */
	if (status == 0x20)										/* Check weather SLA+W transmitted & nack received or not? */
	return 2;												/* If yes then return 2 to indicate nack received i.e. device is busy */
	else
	return 3;												/* Else return 3 to indicate SLA+W failed */
}

uint8_t I2C_Repeated_Start(char slave_read_address)			/* I2C repeated start function */
{
	uint8_t status;											/* Declare variable */
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);					/* Enable TWI, generate start condition and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (start condition) */
	status = TWSR & 0xF8;									/* Read TWI status register with masking lower three bits */
	if (status != 0x10)										/* Check weather repeated start condition transmitted successfully or not? */
	return 0;												/* If no then return 0 to indicate repeated start condition fail */
	TWDR = slave_read_address;								/* If yes then write SLA+R in TWI data register */
	TWCR = (1<<TWEN)|(1<<TWINT);							/* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (Write operation) */
	status = TWSR & 0xF8;									/* Read TWI status register with masking lower three bits */
	if (status == 0x40)										/* Check weather SLA+R transmitted & ack received or not? */
	return 1;												/* If yes then return 1 to indicate ack received */
	if (status == 0x20)										/* Check weather SLA+R transmitted & nack received or not? */
	return 2;												/* If yes then return 2 to indicate nack received i.e. device is busy */
	else
	return 3;												/* Else return 3 to indicate SLA+W failed */
}

void I2C_Stop()												/* I2C stop function */
{
	TWCR=(1<<TWSTO)|(1<<TWINT)|(1<<TWEN);					/* Enable TWI, generate stop condition and clear interrupt flag */
	while(TWCR & (1<<TWSTO));								/* Wait until stop condition execution */
}

void I2C_Start_Wait(char slave_write_address)				/* I2C start wait function */
{
	uint8_t status;											/* Declare variable */
	while (1)
	{
		TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);				/* Enable TWI, generate start condition and clear interrupt flag */
		while (!(TWCR & (1<<TWINT)));						/* Wait until TWI finish its current job (start condition) */
		status = TWSR & 0xF8;								/* Read TWI status register with masking lower three bits */
		if (status != 0x08)									/* Check weather start condition transmitted successfully or not? */
		continue;											/* If no then continue with start loop again */
		TWDR = slave_write_address;							/* If yes then write SLA+W in TWI data register */
		TWCR = (1<<TWEN)|(1<<TWINT);						/* Enable TWI and clear interrupt flag */
		while (!(TWCR & (1<<TWINT)));						/* Wait until TWI finish its current job (Write operation) */
		status = TWSR & 0xF8;								/* Read TWI status register with masking lower three bits */
		if (status != 0x18 )								/* Check weather SLA+W transmitted & ack received or not? */
		{
			I2C_Stop();										/* If not then generate stop condition */
			continue;										/* continue with start loop again */
		}
		break;												/* If yes then break loop */
	}
}

uint8_t I2C_Write(char data)								/* I2C write function */
{
	uint8_t status;											/* Declare variable */
	TWDR = data;											/* Copy data in TWI data register */
	TWCR = (1<<TWEN)|(1<<TWINT);							/* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (Write operation) */
	status = TWSR & 0xF8;									/* Read TWI status register with masking lower three bits */
	if (status == 0x28)										/* Check weather data transmitted & ack received or not? */
	return 0;												/* If yes then return 0 to indicate ack received */
	if (status == 0x30)										/* Check weather data transmitted & nack received or not? */
	return 1;												/* If yes then return 1 to indicate nack received */
	else
	return 2;												/* Else return 2 to indicate data transmission failed */
}

char I2C_Read_Ack()										/* I2C read ack function */
{
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA);					/* Enable TWI, generation of ack and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (read operation) */
	return TWDR;											/* Return received data */
}

char I2C_Read_Nack()										/* I2C read nack function */
{
	TWCR=(1<<TWEN)|(1<<TWINT);								/* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (read operation) */
	return TWDR;											/* Return received data */
}


//END END END I2c Headers AND FUNCTION ***********************************************************************************
void USART_Init(unsigned long);				/* USART initialize function */
char USART_RxChar();						/* Data receiving function */
void USART_TxChar(char);					/* Data transmitting function */
void USART_SendString(char*);				/* Send string of USART data function */


#endif

void USART_Init(unsigned long BAUDRATE)				// USART initialize function 
{
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);				//Enable USART T register and Recieve Register
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ10);	/* Write USCRC for 8 bit data and 1 stop bit */
	UBRR0L = BAUD_PRESCALE;								//Set lower bit prescale values
	UBRR0H = (BAUD_PRESCALE >> 8);					//set the upper bit prescale values
}

char USART_RxChar()									/* Data receiving function */
{
	while (!(UCSR0A & (1 << RXC0)));					/* Wait until new data receive */
	return(UDR0);									/* Get and return received data */
}

void USART_TxChar(char data)						/* Data transmitting function */
{
	while (!(UCSR0A & (1<<UDRE0)));					/* Wait until data transmit and buffer get empty */
	UDR0 = data;										/* Write data to be transmitting in UDR */
}

void USART_SendString(char *str)					/* Send string of USART data function */
{
	int i=0;
	while (str[i]!=0)
	{
		USART_TxChar(str[i]);						/* Send each char of string till the NULL */
		i++;
	}
}

char USART_Flush(){
	static unsigned char dummy;
	while ( UCSR0A & (1 << RXC0) ) { dummy = UDR0; }
}


void Get_Data(){
	retun(UDR0);
}

// ***** END USART FUNCTS***************************************************



//** STARTS GRYO FUNCTS *****************************************************************************
float Acc_x,Acc_y,Acc_z,Temperature,Gyro_x,Gyro_y,Gyro_z;

void MPU6050_Init()										/* Gyro initialization function */
{
	_delay_ms(150);										
	I2C_Start_Wait(0xD0);								/* Start with device write address */
	I2C_Write(SMPLRT_DIV);								
	I2C_Write(0x07);									/* 1KHz sample rate */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(PWR_MGMT_1);								
	I2C_Write(0x01);									/* X axis gyroscope reference frequency */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(CONFIG);									/* Write to Configuration register */
	I2C_Write(0x00);									/* Fs = 8KHz */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(GYRO_CONFIG);								/* Write to Gyro configuration register */
	I2C_Write(0x18);									/* Full scale range +/- 2000 degree/C */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(INT_ENABLE);								/* Write to interrupt enable register */
	I2C_Write(0x01);
	I2C_Stop();
}

void MPU_Start_Loc()
{
	I2C_Start_Wait(0xD0);								/* I2C start with device write address */
	I2C_Write(ACCEL_XOUT_H);							/* Write start location address from where to read */
	I2C_Repeated_Start(0xD1);							/* I2C start with device read address */
}

void Read_RawValue()
{
	MPU_Start_Loc();									/* Read Gyro values */
	Acc_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Temperature = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Nack());
	I2C_Stop();
}


//**** END GYRO FUNCTIONS *********************************************88


int main(void)
{

	
	char buffer[20], float_[10];
	float Xa,Ya,Za,t;
	float Xg=0,Yg=0,Zg=0;
	I2C_Init();											/* Initialize I2C */
	MPU6050_Init();										/* Initialize MPU6050 */

	char Data_in;
    DDRB = 0xff;							/* make PORT as output port */
	DDRA = 0xFF; PORTA = 0x00;
	USART_Init(9600);						/* initialize USART with 9600 baud rate */
	LED = 0;
	int i = 0;
	unsigned char ElectricFlag = 0;
	while(1)
	{
		//PORTA = 1;
		Data_in = UDR0;
		if (Data_in == '1')
		{
			ElectricFlag = 1;
		}
		else if (Data_in == '2')
		{
			ElectricFlag = 0;
		}


		Read_RawValue();
		
		Xa = Acc_x/16384.0;								/* Divide raw value by sensitivity scale factor to get real values */
		Ya = Acc_y/16384.0;
		Za = Acc_z/16384.0;
		
		Xg = Gyro_x/16.4;
		Yg = Gyro_y/16.4;
		Zg = Gyro_z/16.4;
		
		unsigned char tot =0;
		t = (Temperature/340.00)+36.53;					/* Convert temperature in °/c using formula */
		
	
		
		if (Xa > .15)
		{
			tot = 8;
			
		}
		else if (Xa < .05)
		 {
			 tot = 1;
			 
		 }
	
		  if (Ya < -.05)
		  {
			  tot += 2;
		  }
		  
		  else if(Ya > .06) {
			  tot += 4;
		  }
	
		else {
			tot += 0;
		}
		PORTB = tot;
		PORTA = ElectricFlag;
		i++;
		if (i > 5)
		{
			
			dtostrf( Xa, 3, 2, float_ );					/* Take values in buffer to send all parameters over USART */
			sprintf(buffer," Ax = %s g\t",float_);
			USART_SendString(buffer);

			dtostrf( Ya, 3, 2, float_ );
			sprintf(buffer," Ay = %s g\t",float_);
			USART_SendString(buffer);
			
			dtostrf( Za, 3, 2, float_ );
			sprintf(buffer," Az = %s g\t",float_);
			USART_SendString(buffer);
			
			if (ElectricFlag == 1)
			{
				USART_SendString("     EM       ON         ");
			}
			else{
				USART_SendString("     EM       OFF        ");
			}
			i = 0;
		}
		

// 		dtostrf( t, 3, 2, float_ );
// 		sprintf(buffer," T = %s%cC\t",float_,0xF8);           /* 0xF8 Ascii value of degree '°' on serial */
// 		USART_SendString(buffer);
// 
// 		dtostrf( Xg, 3, 2, float_ );
// 		sprintf(buffer," Gx = %s%c/s\t",float_,0xF8);
// 		USART_SendString(buffer);
// 
// 		dtostrf( Yg, 3, 2, float_ );
// 		sprintf(buffer," Gy = %s%c/s\t",float_,0xF8);
// 		USART_SendString(buffer);
// 		
// 		dtostrf( Zg, 3, 2, float_ );
// 		sprintf(buffer," Gz = %s%c/s\r\n",float_,0xF8);
// 		USART_SendString(buffer);
	
		_delay_ms(1000);

// 		Data_in = USART_RxChar();						/* receive data from Bluetooth device*/
// 		if(Data_in =='1')
// 		{
// 			LED = 1;							/* Turn ON LED */
// 			USART_SendString("LED_ON");					/* send status of LED i.e. LED ON */
// 			USART_TxChar(5);
// 			
// 		}
// 		else if(Data_in =='2')
// 		{
// 			LED = 0;							/* Turn OFF LED */
// 			USART_SendString("LED_OFF"); 				/* send status of LED i.e. LED OFF */
// 		}
// 		else
// 			USART_SendString("Enter a correct Command");	/* send message for selecting proper option */
}		
	return 0;
}