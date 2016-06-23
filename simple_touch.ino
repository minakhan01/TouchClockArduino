
/*******************************************************************************

 Bare Conductive Touch MP3 player
 ------------------------------
 
 Touch_MP3.ino - touch triggered MP3 playback

 You need twelve MP3 files named TRACK000.mp3 to TRACK011.mp3 in the root of the 
 microSD card. 
 
 When you touch electrode E0, TRACK000.mp3 will play. When you touch electrode 
 E1, TRACK001.mp3 will play, and so on.

  SD card    
  │
    TRACK000.mp3  
    TRACK001.mp3  
    TRACK002.mp3  
    TRACK003.mp3  
    TRACK004.mp3  
    TRACK005.mp3  
    TRACK006.mp3  
    TRACK007.mp3  
    TRACK008.mp3  
    TRACK009.mp3  
    TRACK010.mp3  
    TRACK011.mp3  
 
 Based on code by Jim Lindblom and plenty of inspiration from the Freescale 
 Semiconductor datasheets and application notes.
 
 Bare Conductive code written by Stefan Dzisiewski-Smith and Peter Krige.
 
 This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 
 Unported License (CC BY-SA 3.0) http://creativecommons.org/licenses/by-sa/3.0/
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*******************************************************************************/

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

// touch behaviour definitions
#define firstPin 0
#define lastPin 11

int myPins[24];
int touchToHourMapping[12] = {
  5, 6, 4, 7, 8, 9, 3, 10, 11, 2, 0, 1
}; 
int hourToLEDmapping[12] = {
  18, 20, 22, 0, 2, 4, 6, 8, 10, 12, 14, 16
};
int ledCount = 24;
int touchCount = 12;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(ledCount, PIN, NEO_GRB + NEO_KHZ800);

void setup(){  
  Serial.begin(57600);
  
  pinMode(LED_BUILTIN, OUTPUT);
   
  //while (!Serial) ; {} //uncomment when using the serial monitor 
  Serial.println("Bare Conductive Touch MP3 player");

  if(!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  MPR121.setTouchThreshold(40);
  MPR121.setReleaseThreshold(20);   

  for (int i=0; i < ledCount; i++){ 
    Serial.println("myPins[i]: "+myPins[i]);
    myPins[i] = 0;
  }

  strip.begin();
  strip.setBrightness(20);
  strip.show(); // Initialize all pixels to 'off'
}

void loop(){
  readTouchInputs();
  lightUp();  
}


void readTouchInputs(){
  if(MPR121.touchStatusChanged()){
    
    MPR121.updateTouchData();

    // only make an action if we have one or fewer pins touched
    // ignore multiple touches
    
//    if(MPR121.getNumTouches()<=1){
      for (int i=0; i < touchCount; i++){  // Check which electrodes were pressed
        if(MPR121.isNewTouch(i)){
        
            //pin i was just touched
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" was just touched");
            Serial.println(myPins[i]);
            myPins[i] = !myPins[i];
//            digitalWrite(LED_BUILTIN, HIGH);
            
        }
      }
//    }
  }
}

void lightUp() {
  uint32_t color = strip.Color(255, 0, 0);
  for (int i=0; i < 12; i++){
    int ledIndex = hourToLEDmapping[touchToHourMapping[i]];
    Serial.print("led Index ");
    Serial.println(ledIndex);
    if (myPins[i] == 1) {
      strip.setPixelColor(ledIndex, color);
    }
    else {
      strip.setPixelColor(ledIndex, 0, 0, 0);
    }
  }
  strip.show();
}


