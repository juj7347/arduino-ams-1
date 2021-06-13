#if defined (__AVR__)
    #include <avr/sleep.h>
    #include <avr/wdt.h>
    #include <avr/power.h>
    #include <avr/interrupt.h>
#endif /* !__AVR__ */

#include "low_power.h"

#if defined (__AVR__)
    #if defined (__AVR_ATmega328P__)
        #ifndef sleep_bod_disable
        #define sleep_bod_disable()                                         \
        do {                                                                \
            unsigned char tempreg;                                          \
            __asm__ __volatile__("in %[tempreg], %[mcucr]" "\n\t"           \
                                 "ori %[tempreg], %[bods_bodse]" "\n\t"     \
                                 "out %[mcucr], %[tempreg]" "\n\t"          \
                                 "andi %[tempreg], %[not_bodse]" "\n\t"     \
                                 "out %[mcucr], %[tempreg]"                 \
                                 : [tempreg] "=&d" (tempreg)                \
                                 : [mcucr] "I" _SFR_IO_ADDR(MCUCR),         \
                                 [bods_bodse] "i" (_BV(BODS) | _BV(BODSE)), \
                                 [not_bodse] "i" (~_BV(BODSE)));            \
        } while (0)
        #endif /* !sleep_bod_disable */
    #endif /* !__AVR_ATmega328P__ */

    #define lowPowerBodOn(mode)     \
    do {                            \
        set_sleep_mode(mode);       \
        cli();                      \
        sleep_enable();             \
        sei();                      \
        sleep_cpu();                \
        sleep_disable();            \
        sei();                      \
    } while (0);

    #if defined (__AVR_ATmega328P__)
    #define lowPowerBodOff(mode)    \
    do {                            \
        set_sleep_mode(mode);       \
        cli();                      \
        sleep_enable();             \
        sleep_bod_disable();        \
        sei();                      \
        sleep_cpu();                \
        sleep_disable();            \
        sei();                      \
    } while (0);
    #endif /* !__AVR_ATmega328P__ */

    #if defined (__AVR_ATmega328P__)
    void LowPowerClass::idle(period_t period, adc_t adc, timer2_t timer2,
                             timer1_t timer1, timer0_t timer0, spi_t spi,
                             usart0_t usart0, twi_t twi) {
        unsigned char clockSource = 0;
        
        if (timer2 == TIMER2_OFF) {
            clockSource = TCCR2B;

            TCCR2B &= ~(1 << CS22);
            TCCR2B &= ~(1 << CS21);
            TCCR2B &= ~(1 << CS20);

            power_timer2_disable();
        }
        
        if (adc == ADC_OFF) {
            ADCSRA &= ~(1 << ADEN);
            power_adc_disable();
        }

        if (timer1 == TIMER1_OFF)
            power_timer1_disable();
        if (timer0 == TIMER0_OFF)
            power_timer0_disable();
        if (spi == SPI_OFF)
            power_spi_disable();
        if (usart0 == USART0_OFF)
            power_usart0_disable();
        if (twi == TWI_OFF)
            power_twi_disable();

        if (period != SLEEP_FOREVER) {
            wdt_enable(period);
            WDTCSR |= (1 << WDIE);
        }
        
        lowPowerBodOn(SLEEP_MODE_IDLE);
        
        if (adc == ADC_OFF) {
            power_adc_enable();
            ADCSRA |= (1 << ADEN);
        }

        if (timer2 == TIMER2_OFF) {
            TCCR2B = clockSource;
            power_timer2_enable();
        }

        if (timer1 == TIMER1_OFF)
            power_timer1_enable();
        if (timer0 == TIMER0_OFF)
            power_timer0_enable();
        if (spi == SPI_OFF)
            power_spi_enable();
        if (usart0 == USART0_OFF)
            power_usart0_enable();
        if (twi == TWI_OFF)
            power_twi_enable();
    }
    #endif /* !__AVR_ATmega328P__ */

    void LowPowerClass::adcNoiseReduction(period_t period, adc_t adc,
                                          timer2_t timer2) {
        unsigned char clockSource = 0;

        #if !defined(__AVR_ATmega32U4__)
        if (timer2 == TIMER2_OFF) {
            clockSource = TCCR2B;

            TCCR2B &= ~(1 << CS22);
            TCCR2B &= ~(1 << CS21);
            TCCR2B &= ~(1 << CS20);
        }
        #endif /* !__AVR_ATmega32U4__ */

        if (adc == ADC_OFF)
            ADCSRA &= ~(1 << ADEN);

        if (period != SLEEP_FOREVER) {
            wdt_enable(period);
            WDTCSR |= (1 << WDIE);
        }

        lowPowerBodOn(SLEEP_MODE_ADC);

        if (adc == ADC_OFF)
            ADCSRA |= (1 << ADEN);

        #if !defined(__AVR_ATmega32U4__)
        if (timer2 == TIMER2_OFF) {
            TCCR2B = clockSource;
        }
        #endif /* !__AVR_ATmega32U4__ */
    }

    void LowPowerClass::powerDown(period_t period, adc_t adc, bod_t bod) {
        if (adc == ADC_OFF)
            ADCSRA &= ~(1 << ADEN);

        if (period != SLEEP_FOREVER) {
            wdt_enable(period);
            WDTCSR |= (1 << WDIE);
        }

        if (bod == BOD_OFF) {
            #if defined (__AVR_ATmega328P__)
                lowPowerBodOff(SLEEP_MODE_PWR_DOWN);
            #else
                lowPowerBodOn(SLEEP_MODE_PWR_DOWN);
            #endif /* !__AVR_ATmega328P__ */
        } else {
            lowPowerBodOn(SLEEP_MODE_PWR_DOWN);
        }

        if (adc == ADC_OFF)
            ADCSRA |= (1 << ADEN);
    }

    void LowPowerClass::powerSave(period_t period, adc_t adc, bod_t bod,
                                  timer2_t timer2) {
        unsigned char clockSource = 0;

        #if !defined(__AVR_ATmega32U4__)
        if (timer2 == TIMER2_OFF) {
            clockSource = TCCR2B;

            TCCR2B &= ~(1 << CS22);
            TCCR2B &= ~(1 << CS21);
            TCCR2B &= ~(1 << CS20);
        }
        #endif /* !__AVR_ATmega32U4__ */

        if (adc == ADC_OFF)
            ADCSRA &= ~(1 << ADEN);

        if (period != SLEEP_FOREVER) {
            wdt_enable(period);
            WDTCSR |= (1 << WDIE);
        }

        if (bod == BOD_OFF) {
            #if defined (__AVR_ATmega328P__)
                lowPowerBodOff(SLEEP_MODE_PWR_SAVE);
            #else
                lowPowerBodOn(SLEEP_MODE_PWR_SAVE);
            #endif /* !__AVR_ATmega328P__ */
        } else {
            lowPowerBodOn(SLEEP_MODE_PWR_SAVE);
        }

        if (adc == ADC_OFF)
            ADCSRA |= (1 << ADEN);

        #if !defined(__AVR_ATmega32U4__)
        if (timer2 == TIMER2_OFF) {
            TCCR2B = clockSource;
        }
        #endif /* !__AVR_ATmega32U4__ */
    }

    void LowPowerClass::powerStandby(period_t period, adc_t adc, bod_t bod) {
        if (adc == ADC_OFF)
            ADCSRA &= ~(1 << ADEN);

        if (period != SLEEP_FOREVER) {
            wdt_enable(period);
            WDTCSR |= (1 << WDIE);
        }

        if (bod == BOD_OFF) {
            #if defined (__AVR_ATmega328P__)
                lowPowerBodOff(SLEEP_MODE_STANDBY);
            #else
                lowPowerBodOn(SLEEP_MODE_STANDBY);
            #endif /* !__AVR_ATmega328P__ */
        } else {
            lowPowerBodOn(SLEEP_MODE_STANDBY);
        }

        if (adc == ADC_OFF)
            ADCSRA |= (1 << ADEN);
    }

    void LowPowerClass::powerExtStandby(period_t period, adc_t adc, bod_t bod,
                                        timer2_t timer2) {
        unsigned char clockSource = 0;

        #if !defined(__AVR_ATmega32U4__)
        if (timer2 == TIMER2_OFF) {
            clockSource = TCCR2B;

            TCCR2B &= ~(1 << CS22);
            TCCR2B &= ~(1 << CS21);
            TCCR2B &= ~(1 << CS20);
        }
        #endif /* !__AVR_ATmega32U4__ */

        if (adc == ADC_OFF)
            ADCSRA &= ~(1 << ADEN);

        if (period != SLEEP_FOREVER) {
            wdt_enable(period);
            WDTCSR |= (1 << WDIE);
        }

        #if defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)
        #else
            if (bod == BOD_OFF) {
                #if defined (__AVR_ATmega328P__)
                    lowPowerBodOff(SLEEP_MODE_EXT_STANDBY);
                #else
                    lowPowerBodOn(SLEEP_MODE_EXT_STANDBY);
                #endif /* !__AVR_ATmega328P__ */
            } else {
                lowPowerBodOn(SLEEP_MODE_EXT_STANDBY);
            }
        #endif /* !__AVR_ATmega88__ || __AVR_ATmega168__ */

        if (adc == ADC_OFF)
            ADCSRA |= (1 << ADEN);

        #if !defined(__AVR_ATmega32U4__)
        if (timer2 == TIMER2_OFF) {
            TCCR2B = clockSource;
        }
        #endif /* !__AVR_ATmega32U4__ */
    }

    ISR (WDT_vect) {
        wdt_disable();
    }

#endif /* !__AVR__ */

LowPowerClass LowPower;
