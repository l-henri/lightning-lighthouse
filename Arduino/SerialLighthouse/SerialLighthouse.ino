/*
 This is an example of how simple driving a Neopixel can be
 This code is optimized for understandability and changability rather than raw speed
 More info at http://wp.josh.com/2014/05/11/ws2812-neopixels-made-easy/
*/
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#define PIXELS 16  // Number of pixels in the string

// These values depend on which pin your string is connected to and what board you are using 
// More info on how to find these at http://www.arduino.cc/en/Reference/PortManipulation

// These values are for digital pin 8 on an Arduino Yun or digital pin 12 on a DueMilinove/UNO
// Note that you could also include the DigitalWriteFast header file to not need to to this lookup.

#define PIXEL_PORT  PORTD  // Port of the pin the pixels are connected to
#define PIXEL_DDR   DDRD  // Port of the pin the pixels are connected to
#define PIXEL_BIT   3      // Bit of the pin the pixels are connected to

// These are the timing constraints taken mostly from the WS2812 datasheets 
// These are chosen to be conservative and avoid problems rather than for maximum throughput 

#define T1H  900    // Width of a 1 bit in ns
#define T1L  600    // Width of a 1 bit in ns
#define T0H  400    // Width of a 0 bit in ns
#define T0L  900    // Width of a 0 bit in ns
#define RES 6000    // Width of the low gap between bits to cause a frame to latch

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays

#define NS_PER_SEC (1000000000L)          // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC )
#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )


String messageString = "";
int messageTimerMs = 300;
int signalIntensity = 10;
void setup() 
{
  ledsetup();
  cli();  
  for(int i=0;i<PIXELS;i++)
  {          
    sendPixel(0,0,0);
  }
  sei();
  show();  
  delay(messageTimerMs);
  Serial.begin(9600);
}


void loop() 
{


  if (Serial.available()) 
    {
      //Serial.println("Jai vu un message");
      delay(1);
      messageString = "";
        
      while(Serial.available())   
        {
          char c = Serial.read();
          messageString += c;
          delay(2);      
        } 
      String codeToSend = "";
      for (int k = 0; k < messageString.length();  k++)
        {
          // Converting letter to Morse code
          codeToSend = morseDictionnary(messageString[k]);
          // Sending morse code to display
          sendMorseLetter(codeToSend);
          delay(2*messageTimerMs);
        }
      Serial.println("OK");
      
    }
      
}


/*
Signal sender
*/

void sendMorseLetter(String codeToSend)
{
  // On vérifie si il s'agit d'un espace
  if (codeToSend == ' ')
    {
      delay(5*messageTimerMs);
      return;
    }
  // On parcours le code
  for (int j = 0; j < codeToSend.length();  j++)
  {
    int timeToDelay = messageTimerMs;
    if (codeToSend[j] == '1')
    {
      timeToDelay = 3* timeToDelay;
      }
    else if (codeToSend[j] == '0')
    {;}
    else
    {continue;}
    //Serial.println(codeToSend[j]);
    turnLedsOn();
    delay(timeToDelay); 
  
    turnLedsOff();
    delay(messageTimerMs);
   }
        
   
}

void turnLedsOff()
{
  delay(10);
  // On eteind les lumieres
  cli();  
  for(int i=0;i<PIXELS;i++)
  {       
    sendPixel(0,0,0);
  }
  sei();
  show(); 
}

void turnLedsOn()
{
  
  // On eteind les lumieres
  cli();  
  for(int i=0;i<PIXELS;i++)
  {       
    sendPixel(signalIntensity,signalIntensity,signalIntensity);
  }
  sei();
  show(); 
}
/*
Morse dictionnary
*/

String morseDictionnary(char letter)
{
  String code = "";
  switch (letter )
  {
    case 'a':
      code = "01";
      break; 
    case 'B':
      code = "1000";
      break;
    case 'C':
      code = "1010";
      break;
    case 'd':
      code = "100";
      break;
    case 'e':
      code = "0";
      break;
    case 'f':
      code = "0010";
      break;
    case 'g':
      code = "110";
      break;
    case 'h':
      code = "0000";
      break;
    case 'i':
      code = "00";
      break;
    case 'j':
      code = "0111";
      break;
    case 'k':
      code = "1010";
      break;
    case 'l':
      code = "0100";
      break;
    case 'm':
      code = "11";
      break;
    case 'n':
      code = "10";
      break;
    case 'o':
      code = "111";
      break;
    case 'p':
      code = "0110";
      break;
    case 'q':
      code = "1101";
      break;
    case 'r':
      code = "010";
      break;
    case 's':
      code = "000";
      break;
    case 't':
      code = "1";
      break;
    case 'u':
      code = "001";
      break;
    case 'v':
      code = "0001";
      break;
    case 'w':
      code = "011";
      break;
    case 'x':
      code = "1001";
      break;
    case 'y':
      code = "1011";
      break;
    case 'z':
      code = "1100";
      break;
    case '0':
      code = "11111";
      break;
    case '1':
      code = "01111";
      break;
    case '2':
      code = "00111";
      break;
    case '3':
      code = "00011";
      break;
    case '4':
      code = "00001";
      break;
    case '5':
      code = "00000";
      break;
    case '6':
      code = "10000";
      break;
    case '7':
      code = "11000";
      break;
    case '8':
      code = "11100";
      break;
    case '9':
      code = "11110";
      break;
    case ' ':
      code = " ";
      break;
  }
  return code;
}

/*

LED Strip library



*/

// Actually send a bit to the string. We must to drop to asm to enusre that the complier does
// not reorder things and make it so the delay happens in the wrong place.

void sendBit( bool bitVal ) {
  
    if (  bitVal ) {				// 0 bit
      
		asm volatile (
			"sbi %[port], %[bit] \n\t"				// Set the output bit
			".rept %[onCycles] \n\t"                                // Execute NOPs to delay exactly the specified number of cycles
			"nop \n\t"
			".endr \n\t"
			"cbi %[port], %[bit] \n\t"                              // Clear the output bit
			".rept %[offCycles] \n\t"                               // Execute NOPs to delay exactly the specified number of cycles
			"nop \n\t"
			".endr \n\t"
			::
			[port]		"I" (_SFR_IO_ADDR(PIXEL_PORT)),
			[bit]		"I" (PIXEL_BIT),
			[onCycles]	"I" (NS_TO_CYCLES(T1H) - 2),		// 1-bit width less overhead  for the actual bit setting, note that this delay could be longer and everything would still work
			[offCycles] 	"I" (NS_TO_CYCLES(T1L) - 2)			// Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness

		);
                                  
    } else {					// 1 bit

		// **************************************************************************
		// This line is really the only tight goldilocks timing in the whole program!
		// **************************************************************************


		asm volatile (
			"sbi %[port], %[bit] \n\t"				// Set the output bit
			".rept %[onCycles] \n\t"				// Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
			"nop \n\t"                                              // Execute NOPs to delay exactly the specified number of cycles
			".endr \n\t"
			"cbi %[port], %[bit] \n\t"                              // Clear the output bit
			".rept %[offCycles] \n\t"                               // Execute NOPs to delay exactly the specified number of cycles
			"nop \n\t"
			".endr \n\t"
			::
			[port]		"I" (_SFR_IO_ADDR(PIXEL_PORT)),
			[bit]		"I" (PIXEL_BIT),
			[onCycles]	"I" (NS_TO_CYCLES(T0H) - 2),
			[offCycles]	"I" (NS_TO_CYCLES(T0L) - 2)

		);
      
    }
    
    // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the 5us reset timeout (which is A long time)
    // Here I have been generous and not tried to squeeze the gap tight but instead erred on the side of lots of extra time.
    // This has thenice side effect of avoid glitches on very long strings becuase 

    
}  

  
void sendByte( unsigned char byte ) {
    
    for( unsigned char bit = 0 ; bit < 8 ; bit++ ) {
      
      sendBit( bitRead( byte , 7 ) );                // Neopixel wants bit in highest-to-lowest order
                                                     // so send highest bit (bit #7 in an 8-bit byte since they start at 0)
      byte <<= 1;                                    // and then shift left so bit 6 moves into 7, 5 moves into 6, etc
      
    }           
} 

/*
  The following three functions are the public API:
  
  ledSetup() - set up the pin that is connected to the string. Call once at the begining of the program.  
  sendPixel( r g , b ) - send a single pixel to the string. Call this once for each pixel in a frame.
  show() - show the recently sent pixel on the LEDs . Call once per frame. 
  
*/


// Set the specified pin up as digital out

void ledsetup() {
  
  bitSet( PIXEL_DDR , PIXEL_BIT );
  
}

void sendPixel( unsigned char r, unsigned char g , unsigned char b )  {  
  
  sendByte(g);          // Neopixel wants colors in green then red then blue order
  sendByte(r);
  sendByte(b);
  
}


// Just wait long enough without sending any bots to cause the pixels to latch and display the last sent frame

void show() {
	_delay_us( (RES / 1000UL) + 1);				// Round up since the delay must be _at_least_ this long (too short might not work, too long not a problem)
}


/*
  That is the whole API. What follows are some demo functions rewriten from the AdaFruit strandtest code...
  
  https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino
  
  Note that we always turn off interrupts while we are sending pixels becuase an interupt
  could happen just when we were in the middle of somehting time sensitive.
  
  If we wanted to minimize the time interrupts were off, we could instead 
  could get away with only turning off interrupts just for the very brief moment 
  when we are actually sending a 0 bit (~1us), as long as we were sure that the total time 
  taken by any interrupts + the time in our pixel generation code never exceeded the reset time (5us).
  
*/
