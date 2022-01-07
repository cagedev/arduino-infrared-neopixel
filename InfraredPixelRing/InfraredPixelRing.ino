/* 
 *  InfraredPixelRing
 *  
 *  - Receive code from remote
 *  - output to (blocking) LED sequence
 *  - more codes (left/right/up/down hardcoded)
 *  - COMBO for rainbow-show
 *  - set GRUNDIG RC-7-1 universal remote to code 0053
 *
 *  TODO:
 *  - update codes
 *  - fix hardware
 */

// Config - IR Module
// IRremote - https://github.com/Arduino-IRremote/Arduino-IRremote
#include <IRremote.hpp>
  int IR_RECV_PIN = 11;
  int IR_DEBUG_PIN = 13;
  // IRrecv irrecv(RECV_PIN); deprecated (v2)
  // decode_results results;  deprecated (v2)

// Config - Neopixel
// Adafruit NeoPixel - https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>
  #define NEOPIXEL_PIN 6
  #define NEOPIXEL_SIZE 12
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_SIZE, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Config - Constant values
const int WIPE_WAIT = 50;
const int DEBUG_BLINK_TIME = 0;
const int LED_INTENSITY = 255;
const int LED_INTENSITY_DELTA = 5;
const bool DEBUG = false;

//  Config - IR codes for GRUNDIG RC-7-1 set to config 0053 (default)
//  Other codes can be calibrated by setting DEBUG to true and 
//  reading the codes printed over the serial connection (9600 baud) 
const uint32_t POWER_BUTTON  = 0xFB030D00;
const uint32_t RED_BUTTON    = 0x04682970;
const uint32_t GREEN_BUTTON  = 0x381B5516;
const uint32_t YELLOW_BUTTON = 0xE1B615A7;
const uint32_t BLUE_BUTTON   = 0xFB54A246;
const uint32_t INNER_LEFT    = 0x8E9BB8CB;
const uint32_t INNER_TOP     = 0x72FF0B6C;
const uint32_t INNER_RIGHT   = 0xB4590A56;
const uint32_t INNER_BOTTOM  = 0x1B4AE4E7;
const uint32_t NUMBER_1      = 0x4F9606B3;
const uint32_t NUMBER_2      = 0x12C2B062;
const uint32_t NUMBER_3      = 0xF4487037;
const uint32_t NUMBER_4      = 0xAD778664;
const uint32_t NUMBER_5      = 0xBCE93257;
const uint32_t NUMBER_6      = 0x0BA3578E;
const uint32_t NUMBER_7      = 0xA71FC66D;
const uint32_t NUMBER_8      = 0x6B2E29A2;
const uint32_t NUMBER_9      = 0xD7C6997F;
const uint32_t NUMBER_0      = 0x1BBA45C6;
const uint32_t RECORD_BUTTON = 0x823EA53D;
const uint32_t PAUSE_BUTTON  = 0x2432DE12;

// Config - Initial state values
uint32_t CURRENT_COLOR = strip.Color(255,255,255);
int RING_STATE = 0;
int COMBO = 0;

void setup()
{
  pinMode(IR_DEBUG_PIN, OUTPUT);
  if (DEBUG) {
    Serial.begin(9600);
  }
  // irrecv.enableIRIn(); // deprecated (v2)
  IrReceiver.begin(IR_RECV_PIN, IR_DEBUG_PIN); // Start the receiver
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  if (IrReceiver.decode()) { // irrecv.decode(&results) deprecated (v2)
    if (DEBUG) {
      // Serial.println(results.value, HEX); deprecated (v2)
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    }
    switch(IrReceiver.decodedIRData.decodedRawData) {
      case POWER_BUTTON:
        Serial.println("POWER_BUTTON");
        COMBO = 0;
        debug(0);
        colorWipe(strip.Color(0, 0, 0), WIPE_WAIT); // off
        break;
      case RED_BUTTON:
        COMBO = 0;
        debug(1); // blocking blink
        CURRENT_COLOR = strip.Color(255, 0, 0);
        colorWipe(CURRENT_COLOR, WIPE_WAIT); // red
        break;
      case GREEN_BUTTON: // green button
        COMBO = 0;
        debug(2); // blocking blink
        CURRENT_COLOR = strip.Color(0, 255, 0);
        colorWipe(strip.Color(0, 255, 0), WIPE_WAIT); // green
        break;
      case YELLOW_BUTTON: // yellow button
        COMBO = 0;
        debug(3); // blocking blink
        CURRENT_COLOR = strip.Color(255, 255, 0);
        colorWipe(strip.Color(255, 255, 0), WIPE_WAIT); // yellow
        break;
      case BLUE_BUTTON: // blue button
        COMBO = 0;
        debug(4); // blocking blink
        CURRENT_COLOR = strip.Color(0, 0, 255);
        colorWipe(strip.Color(0, 0, 255), WIPE_WAIT); // blue
        break;

      case INNER_LEFT: // inner left
        COMBO = 0;
        clearRing();
        strip.setPixelColor( 8, CURRENT_COLOR);
        strip.setPixelColor( 9, CURRENT_COLOR);
        strip.setPixelColor(10, CURRENT_COLOR);
        strip.show();
        break;
      case INNER_TOP: // inner top
        COMBO = 0;
        clearRing();
        strip.setPixelColor(11, CURRENT_COLOR);
        strip.setPixelColor( 0, CURRENT_COLOR);
        strip.setPixelColor( 1, CURRENT_COLOR);
        strip.show();
        break;
      case INNER_RIGHT: // inner right
        COMBO = 0;
        clearRing();
        strip.setPixelColor( 2, CURRENT_COLOR);
        strip.setPixelColor( 3, CURRENT_COLOR);
        strip.setPixelColor( 4, CURRENT_COLOR);
        strip.show();
        break;
      case INNER_BOTTOM: // inner bottom
        COMBO = 0;
        clearRing();
        strip.setPixelColor( 5, CURRENT_COLOR);
        strip.setPixelColor( 6, CURRENT_COLOR);
        strip.setPixelColor( 7, CURRENT_COLOR);
        strip.show();
        break;

     case RECORD_BUTTON: // Record
       rainbowCycle(1);
       break;
     case PAUSE_BUTTON: // Pause
       rainbowCycle(10);
       break;
        
      // numbers
      case NUMBER_1: // 1
        COMBO = 1;
        clearRing();
        strip.setPixelColor( 1, CURRENT_COLOR);
        strip.show();
        break;
      case NUMBER_2: // 2
        if (COMBO == 1) {
          COMBO = 2;
        } else {
          COMBO = 0;
        }
        clearRing();
        strip.setPixelColor( 2, CURRENT_COLOR);
        strip.show();
        break;
      case NUMBER_3: // 3
        if (COMBO == 2) {
          COMBO = 3;
        } else {
          COMBO = 0;
        }
        clearRing();
        strip.setPixelColor( 3, CURRENT_COLOR);
        strip.show();
        if (COMBO == 3) {
          rainbowCycle(1);
          COMBO = 0;
        }
        break;
      case NUMBER_4: // 4
        COMBO = 0;
        clearRing();
        strip.setPixelColor( 4, CURRENT_COLOR);
        strip.show();
        break;
      case NUMBER_5: // 5
        COMBO = 0;
        clearRing();
        strip.setPixelColor( 5, CURRENT_COLOR);
        strip.show();
        break;
      case NUMBER_6: // 6
        COMBO = 0;
        clearRing();
        strip.setPixelColor( 6, CURRENT_COLOR);
        strip.show();
        break;
      case NUMBER_7: // 7
        COMBO = 0;
        clearRing();
        strip.setPixelColor( 7, CURRENT_COLOR);
        strip.show();
        break;
      case NUMBER_8: // 8
        COMBO = 0;
        clearRing();
        strip.setPixelColor( 8, CURRENT_COLOR);
        strip.show();
        break;
      case NUMBER_9: // 9
        COMBO = 0;
        clearRing();
        strip.setPixelColor( 9, CURRENT_COLOR);
        strip.show();
        break;
      case NUMBER_0: // 0
        COMBO = 0;
        clearRing();
        strip.setPixelColor( 0, CURRENT_COLOR);
        strip.show();
        break;
      default:
        // Set the first light to white when in debug mode
        if (DEBUG) {
          strip.setPixelColor(0, strip.Color(255,255,255));
          strip.show();
        }
    }
    // irrecv.resume(); // deprecated (v2)
    IrReceiver.resume(); // Enable receiving of the next value
  }
  delay(100);
}


void clearRing() {
  colorWipe(strip.Color(0,0,0), 0);
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void debug(int times) {
  for (int i = 0; i<times; i++) {
    digitalWrite(IR_DEBUG_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(DEBUG_BLINK_TIME);              // wait for a second
    digitalWrite(IR_DEBUG_PIN, LOW);    // turn the LED off by making the voltage LOW
    delay(DEBUG_BLINK_TIME);              // wait for a second
  }
}

