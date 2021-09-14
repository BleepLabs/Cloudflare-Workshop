//More info: https://github.com/BleepLabs/Cloudflare-Workshop/wiki/Cloudflare-workshop-library

#include <cloudflare_arduino_workshop.h>

digitalSmoothing sm1; //Smooth two analog readings
digitalSmoothing sm2;

unsigned long current_time;
unsigned long prev[8]; //array of 8 variables named "prev"
int shift;
int raw1, smoothedPot1;
int raw2, smoothedPot2;
float scaled1;

void setup() {
  ledGrid.begin(); //must be done in setup for the LEDs to work.

  analogReadResolution(12); //0-4095 pot values
  analogReadAveraging(32);  //Take 32 readings and average everytime you do an analogRead(). First step of smoothing

}


void loop() {
  current_time = millis();


  if (current_time - prev[0] > 33) { //33 is about 30Hz
    prev[0] = current_time;

    shift++;

    for ( int led_c = 0; led_c < 64; led_c++) { //64 leds total
      float hue = shift + (led_c / 64.0);
      set_pixel_HSV(led_c, hue, 1, 1);
    }
    
    for ( int led_c = 33; led_c < 39; led_c++) { //overwrite some of the pixels
      set_pixel_HSV(led_c, 0, 0, random(100) / 100.0); //random is integer only
    }
    
    ledGrid.show(); // after we've set what we want all the LEDs to be we send the data out through this function
  }

  //the faster we read the less lag there will be in the smoothes output but
  // if we read more slowly it will be smoother
  if (current_time - prev[1] > 4) {
    prev[1] = current_time;
    raw1 = analogRead(A0);
    smoothedPot1 = sm1.smooth(raw1);
    raw2 = analogRead(A1);
    smoothedPot2 = sm2.smooth(raw2);

    //fscale( inputValue, originalMin, originalMax, newBegin, newEnd, curve)
    //The analogReads don't alays get to thier max an min, especially when the LED grid is on
    //I used the raw readings to find theapproxamate max an min then put them in here.
    scaled1 = fscale(smoothedPot1, 250, 3950, 1000, 0, -.8);
  }


  if (current_time - prev[2] > 100) {
    prev[2] = current_time;

    Serial.print(raw1);
    Serial.print(" ");
    Serial.print(smoothedPot1);
    Serial.print(" ");
    Serial.print(scaled1);
    Serial.println();
  }



}// loop is over
