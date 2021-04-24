/* 
 * File:   main.c
 * Author: Victor Tran (github.com/victorvantran)
 *
 * Created on April 16, 2021, 5:47 PM
 */

#ifndef STEPPER_MOTOR_H
#define	STEPPER_MOTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

    
    
#define WGM_FAST_PWM_NI 0 
#define WGM_FAST_PWM_I 1 
#define WGM_PHASE_CORRECTED_PWM_NI 2
#define WGM_PHASE_CORRECTED_PWM_I 3

    
    
void init_pwm_stepper_motor(uint8_t mode)
{
    // Rely on built-in hardware unit for wave generation instead of software
    // manual delays...
    
    // Set Direction of OC0 to output
    DDRB |= _BV(PINB3);
    
    // Set PWM Waveform mode
    if (mode == WGM_FAST_PWM_NI)
    {
        // Set fast PWM mode, non_inverted
        TCCR0 |= _BV(WGM01) | _BV(WGM00) | _BV(COM01);
    }
    else if (mode == WGM_PHASE_CORRECTED_PWM_NI)
    {
        TCCR0 |= _BV(WGM00) | _BV(COM01);
    }
    else if (mode == WGM_PHASE_CORRECTED_PWM_I)
    {
        TCCR0 |= _BV(WGM01) | _BV(WGM00) | _BV(COM01) | _BV(COM00);
    }
        else if (mode == WGM_PHASE_CORRECTED_PWM_I)
    {
        TCCR0 |= _BV(WGM00) | _BV(COM01) | _BV(COM00);
    }

    // Set clock source with pre-scaling 1 (too dangerously fast for stepper motor)
    //TCCR0 |= _BV(CS00);
    
    // Set clock source with pre-scaling 256
    TCCR0 |=  _BV(CS01);

    // (1/256) % duty cycle
    //
    // Duty cycle does not matter here because the period is based on
    // the TOP (which is 0xFF) and the pre-scaling for timer0 in FastPWM
    // Hence, we are sending a pulse to the stepper motor driver at the
    // same rate, and can only vary by pre-scale steps.
    //
    // Alternatively, we can send it pulses in variable rate by making TOP vary.
    // That means we can control the speed using clear timer on compare match
    // (CTC) mode, where TOP = OCR0.
    OCR0 = 1;
}



#ifdef	__cplusplus
}
#endif

#endif	/* STEPPER_MOTOR_H */

