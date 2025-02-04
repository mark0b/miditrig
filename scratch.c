#define baud 31250 //baud rate for midi
#define fcpu 8000000 //Ideal clock frequency
#define bitW (cpu/baud) //cpu cycles per bit (256)
#define divisor 8 //need the lowest prescaler, i.e. divide timer clock by 8
#define prescaler 0x02 //bit selection for prescaler
#define countBit (bitW/divisor) //count for full bitwidth
#define halfBit (countBit/2) // half a bitwidth of timer clicks
#define maxSpeed 40 //Max Arp speed in Hz (i.e. 40 triggers/second)
#define pulseLen 5 //Length of S Trig pulse in ms

#define bit0 1
#define bit1 2
#define bit2 3 
#define bit3 4
#define bit4 5
#define bit5 6
#define bit6 7
#define bit7 8


#define PORTB *((volatile unsigned char*) 0x38)
#define DDRB *((volatile unsigned char*) 0x37) // Data Direction
#define PINB *((volatile unsigned char*) 0x36) // Port B input

#define GTCCR *((volatile unsigned char*) 0x4C) //General Timer Control register
#define TCR0A *((volatile unsigned char*) 0x4A) //Timer0 Control Register A
#define TCCR0B *((volatile unsigned char*) 0x53) //Timer0 Control Register B
// Clock Select on bits 0,1,2.
#define TCNT0 *((volatile unsigned char*) 0x52) //define mem location of counter 0 register
#define OCR0A *((volatile unsigned char*) 0x49) //define mem location of output compare reg 0A
#define OCR0B *((volatile unsigned char*) 0x48) //define mem location of output compare reg 0B
#define TIMSK *((volatile unsigned char*) 0x59) //define Timer 0 interrupt mask reg
// Compare Inpterupt enable on bit 4 for 0A, 3 for 0B
// Bit 1 for overflow
#define TIFR *((volatile unsigned char*) 0x58) // define timer 0 interrupt flag reg
// Compare interrupts: bit 4 for 0A, 3 for 0B
// Bit 1 for overflow

#define USIDR *((volatile unsigned char*) 0x2F) // USI Data register
#define USIBR *((volatile unsigned char*) 0x30) // USI buffer register, copy of last byte during new byte transmission
#define USISR *((volatile unsigned char*) 0x2E) // USI Status Reg, interrupts and counter
#define USICR *((volatile unsigned char*) 0x2D) // USI Control Reg 
// We should set up USI Clock to be fed from the configured Timer0
// "Three Wire Mode" but use as Half-Duplex Asynchronous Data Transfer

enum MidiSystemMessage {
  SysExStart      = 0xF0,        // Start of SysEx stream
  SysExEnd        = 0xF7,        // End of SysEx stream
  MTCQuaterFrame  = 0xF1,        // MTC quarter frame time code
  SongPosition    = 0xF2,        // Ask slave to position playback cue
  SongSelect      = 0xF3,        // Select a certain song and cue to beginning
  TuningRequested = 0xF6,        // Being asked to self-tune
  MidiClock       = 0xF8,        // Being kept in sync with a tempo (24 clocks per quarter note)
  MidiTick        = 0xF9,        // Being kept in sync with a tick (every 10ms)
  MidiStart       = 0xFA,        // Master asking for playback from the beginning
  MidiContinue    = 0xFB,        // Master asked that we continue playback from cue
  MidiStop        = 0xFC,        // Master asked to stop playback and retain cue point
  ActiveSense     = 0xFE,        // Keepalive data to let us know things are still connected - i.e. "nervous" devices
  Reset           = 0xFF,        // Reset to default, just turned on, no keys pressed, cue to beginning etc
};

void timer0Setup(void){
    DDRB &= ~(1 << bit0);          // Set pin PB0 to input
    PORTB |= 1 << bit0;             // Enable internal pull-up on pin PB0
    TCCR0B |= prescaler;
}




int main(void){
    while(1);
}