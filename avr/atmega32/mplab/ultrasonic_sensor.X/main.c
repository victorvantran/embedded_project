/* 
 * File:   main.c
 * Author: Victor Tran (github.com/victorvantran)
 *
 * Created on April 6, 2021, 11:18 PM
 */


#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "LCD_16x2.h"


// Define helper macros
#define SET_BIT(p,i) ((p) |=  _BV(i))
#define CLR_BIT(p,i) ((p) &= ~_BV(i))
#define GET_BIT(p,i) ((p) &   _BV(i))
#define FLIP_BIT(p,i) ((p) ^= _BV(i))


#define ULTRASONIC_SENSOR_BEGIN_PULSE 0
#define ULTRASONIC_SENSOR_END_PULSE 1
#define CENTIMETER_SCALE 58.30875f //466.47f


static volatile int pulse = 0;
static volatile uint8_t i = ULTRASONIC_SENSOR_BEGIN_PULSE;


void initDebug(void)
{
    // Set GPIO directions for PORTA
	DDRA = 0b11111111;
}


void setupUltrasonicSensor(void)
{
    // Set up external hardware interrupt, specifically for INT0/PIND2. 
    // Call ISR for INT0_vect on edge change.
    // The edge change will occur through the "echo" pin of the ultrasonic sensor
    
    // General Interrupt Control Register (GICR): enables INT0 and INT1 interrupts
    // MicroController Unit Control Register (MCUCR): configures the type of interrupt
   
    // Enable Interrupt0 (INT0 == PIND2)
    GICR |= 1<<INT0; 
    
    // Configure interrupt type of INT0 to be edge change
    MCUCR &= ~(1<<ISC01);
    MCUCR |= (1<<ISC00);
}


void setupLCD(void)
{
    // Set up LCD
    LCD_Init();
    LCD_Command(0x01);
    LCD_String_xy(1, 0, "With One Look");
}


void setupServo(void)
{
    // Set waveform generation mode for servo using:
    // 1) Timer/Counter Control Register 1A (TCCR1A)
    // 2) Timer/Counter Control Register 1B (TCCR1B)
    
    // Set Waveform Generation Mode: Mode 14:
    // Set Waveform generator mode bits to be Fast PWM (not inverting)
    // Input Capture Register (ICR1) as TOP
    // Leaves the OC1A register as PWM output
    TCCR1A |=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);
    TCCR1A &= ~(1<<WGM10);
    TCCR1B |=(1<<WGM13)|(1<<WGM12); 
    
    // Set prescalar to 8
    TCCR1B &= ~(1<<CS12);
    TCCR1B |= (1<<CS11);
    TCCR1B &= ~(1<<CS10);

    // Our servo motor is rated for 50Hz PWM signal (20ms period).
    // We needed to prescalar of 8 to have enough bits to represent 20ms as TOP
    // Write the proper number of cycles for the Up-counting
    ICR1=19999;
}


void setupGPIOs(void)
{
    // Pin direction
    DDRD  = 0b11111011;
    // Enable pull-up
    PORTD = 0b00000100;
}


void setup(void)
{
    // Setup GPIOs, general purpose registers, and special function registers
    setupGPIOs();    
    setupLCD();
    setupUltrasonicSensor();
    setupServo();
    
    // Enable Global Interrupt in the Status Register
    sei();
}



void testBlink(void)
{
	// Wiggle LEDs
	const int lightPin = 7;
	CLR_BIT(PORTA, lightPin);
	_delay_ms(500);
	SET_BIT(PORTA, lightPin);
	_delay_ms(500);
}



void sampleUltrasonicSensor(void)
{       
    // Toggle the trigger of the ultrasonic sensor,
    // supplying a pulse of sufficient width (10 microseconds)
    SET_BIT(PORTD, PIND0);
    _delay_us(10);
    CLR_BIT(PORTD, PIND0);
}


void debugDistance(int16_t distance_cm)
{
    // Toggle LED based on distance
    const int lightPin = 7;    
    if (distance_cm > 25)
    {
        CLR_BIT(PORTA, lightPin);
    }
    else
    {
        SET_BIT(PORTA, lightPin);
    }
}


void displayDistance(char* show_a)
{
    // Display the distance captured by the ultrasonic sensor on the LCD
    int16_t distance_cm = (int16_t)((float)pulse/CENTIMETER_SCALE);
    itoa(distance_cm, show_a, 10);
    LCD_Command (0x01);    
    LCD_String_xy(2, 0, show_a);
    debugDistance(distance_cm);
}


void moveServo(void)
{
    // Enable Fast PWM (Mode 14)
    SET_BIT(TCCR1A,WGM11);
   
    float distance_cm = (float)pulse/CENTIMETER_SCALE;

    int motor_angle = (int)(distance_cm * 50);
    if (motor_angle < 500) motor_angle = 500;
    if (motor_angle > 2500) motor_angle = 2500;
    
    OCR1A = motor_angle;
    _delay_ms(100);
 
    // Disable PWM operation (Mode 13))
    CLR_BIT(TCCR1A,WGM11); 
}



int main(void)
{
    setup();
	initDebug();
    
    char show_a[16];
   
	while (1)
	{ 
        sampleUltrasonicSensor();
        _delay_ms(10);
        moveServo();
        displayDistance(show_a);
	}
}



ISR(INT0_vect)
{
  // Interrupt service routine for measuring
  // the echoed pulse of the ultrasonic sensor
  // with the TimerCounter1.
    
  if(i == ULTRASONIC_SENSOR_BEGIN_PULSE)
  {
    // Reset TimerCounter1
    TCNT1 = 0;
    i = ULTRASONIC_SENSOR_END_PULSE;
  }
  else if (i == ULTRASONIC_SENSOR_END_PULSE)
  {   
    pulse = TCNT1; 
    i = ULTRASONIC_SENSOR_BEGIN_PULSE;
  }
  else 
  {
    i = ULTRASONIC_SENSOR_BEGIN_PULSE;
  }  
}