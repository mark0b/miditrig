#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define baud 31250
#define F_CPU 8000000 //may need calibration
#define CYCLES_PER_BIT (F_CPU / baud) //exactly 256 for midi rate at 8MHz

#if (CYCLES_PER_BIT > 256)
    #define DIVISOR             8
    #define PRESCALE            2
#else
    #define DIVISOR             1
    #define PRESCALE            1
#endif
#define FULL_BIT_TICKS      ( CYCLES_PER_BIT / DIVISOR )

// Select count seeds to include or exclude start bit
// If possible it's optimal to not trigger an interrupt to read in the start bit.
// depends on how many cycles per bit and if it will fit into a single tier register.
#if FULL_BIT_TICKS*(3/2) > 256
    #define INIT_TIM0_SEED (256 - FULL_BIT_TICKS/2) //picking half bit because 3/2 bits would be over 256.
    #define USI_SEED (16 - (1 + 8)) //including shifting out start bit
#else 
    #define INIT_TIM0_SEED (256 - FULL_BIT_TICKS*(3/2)) //skip the start bit if possible
    #define USI_SEED (16 - (8)) //excluding shifting out start bit
#endif

#define TIM0_SEED (256-FULL_BIT_TICKS)

volatile uint8_t serialInput;

//Initialization Routine
void initUART(void){
    // Disable USI
    USICR = 0;
    //Set Data input pin as input
    DDRB &= ~(1 << DDB0); // Low = input.
    DDRB |= 1<<DDB5; // enble led pin output.
    //enable pull up on data output pin
    PORTB |= 1 << PB0;
    //enable Pin change interrupt
    GIMSK |= 1 << PCIE; //Pin change interrupts
    PCMSK |= 1 << PCINT0; //Pin change interrupt on pin 0 (DI)
}

//Pin change Interrupt to setup USI Data Load In
ISR(PCINT0_vect){
    if (!(PINB & 1<<PINB0)) { // only handle if pin changed low.
        // Plant initial timer seed
        // cycles/bit= (8MHz/31250) = 256
        // Seed = 256 - (cycles/bit *1/2) = 128
        TCNT0 = INIT_TIM0_SEED;
        // Start Timer0 with prescaler
        TCCR0A |= 1<<WGM01;
        TCCR0B |= 1<<CS00;
        OCR0A = 0; //set to full register compare.
        // Enable Timer0 overflow interrupt
        TIMSK |= 1 << OCIE0A;
        // plant USI counter seed
        // top value 16
        // USI Seed = top vale - (start + data bits)
        // USI Seed = 16 - (1+8) = 7
        USISR |= 7;
        // select 3 wire mode
        USICR |= 1<<USIWM0;
        // set USI Clock source to timer overflow
        USICR |= 1<<USICS0;
    }
}

//Timer Overflow Interrupt to load each bit into the USI Register
ISR(TIM0_COMPA_vect){
    // Plant timer0 seed plus current timer value (1 bit duration)
    TCNT0 += TIM0_SEED;
}


//USI Overflow Interrupt to offload the prepared byte and prepare for the next message.
ISR(USI_OVF_vect){
    // read USIDR (data)
    serialInput = USIDR;
    PORTB ^= PB5; // flip the led output bit every USI overflow.
    
    // Load data into buffer
    // Stop Timer0
    // Disable USI Overflow Interrupt
    // Clear Pin Change interrupt flag
    // Enable Pin Change Interupt
}

//copy paste code from https://becomingmaker.com/usi-serial-uart-attiny85/
// can't we just try to decode messages flipped?
uint8_t ReverseByte (uint8_t x) {
    x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
    x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
    x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
    return x;
}

int main(){
    //Enable UART USI Reciever
    SREG |= 1<<SREG_I; // global interrupt enable. bit 7 mem 0x3F (+0x20)
    initUART(); //Initialize
}