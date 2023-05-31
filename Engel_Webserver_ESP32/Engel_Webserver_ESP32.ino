/*Engel_Webserver by Florian Kropf
 * used for Embedded Systems I project
 * 
 * 
 * 
 * changeColor based on "Christmas Tree Lights"
 * by JormaSnow
 * https://github.com/JormaSnow/NeoPixel_ChristmasLights/blob/master/NeoPixel_ChristmasLights.ino
 */

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define LED 2
#define LED_PIN 15 // Pin for Neopixel Data Line
#define LED_COUNT 22


// PARAMETERS--for /random application
#define VMAX       150   // the maximum value that can be sent to any one of the RGB channels--255 is the upper limit
#define VMIN       15   // the minimum non-zero value that can be sent to any one of the RGB channels
#define SLOW       1500 // slows down the transition between values when changing LED RGB values in microseconds
#define SWITCH_MAX 13   // the max value for the switch statement--should be >7. See comment below
#define SWITCH_MIN 1    // the minimum for the switch statement--should be 1. See comment below
// constants for the switch statement
#define RED            1
#define GREEN          2
#define RED_AND_GREEN  3
#define BLUE           4
#define RED_AND_BLUE   5
#define GREEN_AND_BLUE 6
#define RGB            7

int op = 0;
const char* ssid = "your_ssid";  //replace
const char* password =  "your_PW"; //replace

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
AsyncWebServer server(80);

void setup() {
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50);

  
  Serial.begin(115200);
  // Set pin mode
  pinMode(LED,OUTPUT);
  digitalWrite(LED, LOW);
  
  WiFi.begin(ssid, password);
 
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("CONNECTED!");

  Serial.print("IP-Adress: ");
  Serial.print(WiFi.localIP());

   server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello World");
  });

  server.on("/ON", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "LED ON");
    digitalWrite(LED, HIGH);
  });

   server.on("/OFF", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "LED OFF");
    digitalWrite(LED, LOW);
  });

  server.on("/blink", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "LED blinking");
    op = 1;
   
  });

  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "LED white");
    op = 2;
   
  });

  server.on("/random", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "LED random");
    op = 3;
   
  });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "LED stop");
    op = 0;
    //strip.clear();
    //strip.show();
   
  });

  
  server.begin();

}
void loop(){
  
  if(op == 1)  {
    //RGB fade
  rainbow(10);
  }
  else if(op == 2) {
    //all white
  light(strip.Color(75, 50, 50));
  }
  else if(op == 3) {
    //random RGB effect
  switch (random(SWITCH_MIN, SWITCH_MAX)) {
    case RED: // sends random value to the red led and 0 to others
      changeColor(random(LED_COUNT), (uint8_t)random(VMIN, VMAX), 0x00, 0x00);
      break;
    case GREEN:// sends random value to the green led and 0 to others
      changeColor(random(LED_COUNT), 0x00, (uint8_t)random(VMIN, VMAX), 0x00);
      break;
    case RED_AND_GREEN: // sends random value to the red and green leds and 0 to blue
      changeColor(random(LED_COUNT), (uint8_t)random(VMIN, VMAX), (uint8_t)random(VMIN, VMAX), 0x00);
      break;
    case BLUE: // sends random value to the blue led and 0 to others
      changeColor(random(LED_COUNT), 0x00, 0x00, (uint8_t)random(VMIN, VMAX));
      break;
    case RED_AND_BLUE: // sends random value to the red and blue leds and 0 to green
      changeColor(random(LED_COUNT), (uint8_t)random(VMIN, VMAX), 0x00, (uint8_t)random(VMIN, VMAX));
      break;
    case GREEN_AND_BLUE: // sends random value to the green and blue leds and 0 to red
      changeColor(random(LED_COUNT), 0x00, (uint8_t)random(VMIN, VMAX), (uint8_t)random(VMIN, VMAX));
      break;
    case RGB: // sends random value to the each of the leds
      changeColor(random(LED_COUNT), (uint8_t)random(VMIN, VMAX), (uint8_t)random(VMIN, VMAX), (uint8_t)random(VMIN, VMAX));
      break;
    default:
      changeColor(random(LED_COUNT), 0x00, 0x00, 0x00);
      break;
  }
  }
  else {
    strip.clear();
    strip.show();
    }
  
  }

void rainbow(int wait) {
  
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    strip.rainbow(firstPixelHue);
    
    if(op == 1){
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
    }
    else{
      return;
      }
  }
}

void light(uint32_t color) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }
  strip.show();                          //  Update strip to match
}

void changeColor(long led, uint8_t newR, uint8_t newG, uint8_t newB) {
  //capture the current state of the led
  uint8_t currentR = (strip.getPixelColor(led) >> 16);
  uint8_t currentG = (strip.getPixelColor(led) >>  8);
  uint8_t currentB = (strip.getPixelColor(led)      );

  boolean changing = true;
  while (changing) {
    changing = false;
    if (currentR < newR) {
      currentR++;
      strip.setPixelColor(led, currentR, currentG, currentB);
      changing = true;
    } else if (currentR > newR) {
      currentR--;
      strip.setPixelColor(led, currentR, currentG, currentB);
      changing = true;
    }

    if (currentG < newG) {
      currentG++;
      strip.setPixelColor(led, currentR, currentG, currentB);
      changing = true;
    } else if (currentG > newG) {
      currentG--;
      strip.setPixelColor(led, currentR, currentG, currentB);
      changing = true;
    }

    if (currentB < newB) {
      currentB++;
      strip.setPixelColor(led, currentR, currentG, currentB);
      changing = true;
    } else if (currentB > newB) {
      currentB--;
      strip.setPixelColor(led, currentR, currentG, currentB);
      changing = true;
    }
    if(op == 3){
    strip.show();
  delayMicroseconds(SLOW); // pauses breifly to slow the overall transition
    }
    else{
      return;
      }
  
  }
}
