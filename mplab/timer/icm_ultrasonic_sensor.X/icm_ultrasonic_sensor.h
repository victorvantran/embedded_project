/* 
 * File:   icm_ultrasonic_sensor.h
 * Author: Victor Tran (github.com/victorvantran)
 *
 * Created on April 15, 2021, 6:32 PM
 */

#ifndef ICM_ULTRASONIC_SENSOR_H
#define	ICM_ULTRASONIC_SENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif


#define SOUND_VELOCITY_M 343UL
#define SOUND_VELOCITY_CM ((SOUND_VELOCITY_M) * (100))
#define DISTANCE_SCALE_CM ((double)(F_CPU)/((double)(SOUND_VELOCITY_CM)/(2))) 
#define FALLING_EDGE 0
#define RISING_EDGE 1

static volatile uint8_t icm_edge = RISING_EDGE;
volatile long v_count = 0;



void init_ultrasonic_sensor(void)
{    
    // Set trigger pin
    DDRB |= _BV(PINB0);
    
    // Set echo pin
    DDRD &= ~_BV(PIND6);
    PORTD |= _BV(PIND6); //pull-up
    
    // Set input capture interrupt enable
    TIMSK |= _BV(TICIE1);
}



void poll_distance(long* count, double* distance, char* string)
{
    // Poll the ultrasonic sensor for a distance in centimeters
    // Disable input capture interrupt
    TIMSK &= ~_BV(TICIE1);
        
    // Trigger pulse
    PORTB |= _BV(PINB0);
    _delay_us(10);
    PORTB &= ~_BV(PINB0);
         
    // Look for rising edge of echo pulse 
    TCCR1B |= _BV(ICES1);      
    while (!(TIFR & _BV(ICF1)));
    // Seed the timer/counter
    TCNT1 = 0;
    // Clear ICP flag
    TIFR |= _BV(ICF1);

    // Look for falling edge of echo pulse
    TCCR1B &= ~_BV(ICES1); 
    while (!(TIFR & _BV(ICF1)));
    // Read ICR1 (TCNT1) into count
    *count = ICR1;
    // Clear ICP flag
    TIFR |= _BV(ICF1);

    // Calculate distance based on pulse width/count and print
    *distance = (double)*count / DISTANCE_SCALE_CM;
    dtostrf(*distance, 2, 2, string);
    strcat(string, "cm. \r");
    printf(string);
   
    // Enable input capture interrupt
    TIMSK |= _BV(TICIE1);
}



double get_distance_cm(double count)
{
    return count / DISTANCE_SCALE_CM;
}



ISR(TIMER1_CAPT_vect)
{
    if (icm_edge == RISING_EDGE)
    {
        TCNT1 = 0;
        TCCR1B &= ~_BV(ICES1); 
        icm_edge = FALLING_EDGE;
    }
    else // if (icm_edge == FALLING_EDGE)
    {
        v_count = ICR1;
        TCCR1B |= _BV(ICES1); 
        icm_edge = RISING_EDGE;
    }
}



#ifdef	__cplusplus
}
#endif

#endif	/* ICM_ULTRASONIC_SENSOR_H */

