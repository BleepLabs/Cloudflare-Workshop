//More info: https://github.com/BleepLabs/Cloudflare-Workshop/wiki/Cloudflare-workshop-library

//dispays a moving rainbow with a white diagonal line from a bitmap

#include <cloudflare_arduino_workshop.h>

digitalSmoothing sm1; //Smooth two analog readings
digitalSmoothing sm2;
digitalSmoothing sm3;

unsigned long current_time;
unsigned long prev[8]; //array of 8 variables named "prev"
int shift;
int raw1, smoothedPot1;
int raw2, smoothedPot2;
float scaled1;
int  prev_touch1, touch1, smooth_tocuh1;;
int threshold1 = 2000;
int flag1 = 0;
int flag_counter1 = 0;
byte bitmap1[8][8] = {
  {1, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 1},
};

void setup() {
  ledGrid.begin(); //must be done in setup for the LEDs to work.
  analogReadResolution(12); //0-4095 pot values
  analogReadAveraging(32);  //Take 32 readings and average everytime you do an analogRead(). First step of smoothing
  //pinMode(1, INPUT);
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

    for (int x_axis = 0; x_axis < 8; x_axis++) {
      for (int y_axis = 0; y_axis < 8; y_axis++) {
        int current_pixel = x_axis + (y_axis * 8);

        if (bitmap1[x_axis][y_axis] == 1) {
          set_pixel_HSV(current_pixel, 0, 0, 1);
        }

        if (x_axis == int(scaled1)) {  //convert to integer https://www.arduino.cc/reference/en/language/variables/conversion/intcast/
          set_pixel_HSV(current_pixel, 0, 0, 0);
        }

      }

    }


    if (smooth_tocuh1 < threshold1) {
      for ( int led_c = 0; led_c < 64; led_c++) { //64 leds total
        set_pixel_HSV(led_c, 0, 0, 0);
      }
    }
    if (flag1 == 1) {
      float bright1=(5-flag_counter1)/5.0;
      for ( int led_c = 0; led_c < 64; led_c++) { //64 leds total
        set_pixel_HSV(led_c, 0, 0, bright1);
      }
      flag_counter1--;
      if (flag_counter1 <= 0) {
        flag1 = 0;
      }
    }

    ledGrid.show(); // after we've set what we want all the LEDs to be we send the data out through this function
  }

  //the faster we read the less lag there will be in the smoothes output but
  // if we read more slowly it will be smoother
  if (current_time - prev[1] > 4) {
    prev[1] = current_time;
    raw1 = analogRead(A0);

    smoothedPot1 = sm1.smooth(raw1);
    raw2 = analogRead(A4);
    smoothedPot2 = sm2.smooth(raw2);

    touch1 = touchRead(1);
    prev_touch1 = smooth_tocuh1;
    smooth_tocuh1 = sm3.smooth(touch1);

    if (prev_touch1 < threshold1 && smooth_tocuh1 > threshold1) {
      flag1 = 1;
      flag_counter1 = 5;
    }

    //fscale( inputValue, originalMin, originalMax, newBegin, newEnd, curve)
    //The analogReads don't alays get to thier max an min, especially when the LED grid is on
    //I used the raw readings to find theapproxamate max an min then put them in here.
    scaled1 = fscale(smoothedPot2, 700.0, 2700.0, 0, 7.0, 0);
  }



  if (current_time - prev[2] > 20) {
    prev[2] = current_time;

    Serial.print(smooth_tocuh1);
    Serial.println();
  }


}// loop is over
