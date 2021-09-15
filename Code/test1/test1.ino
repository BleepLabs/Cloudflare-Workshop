// tests that all the libraries are installed

int interval1 = 500; //time between blinks in millisecods
int on_time1 = 100; // time the LED stays on in microseconds

#include <cloudflare_arduino_workshop.h>

digitalSmoothing sm1;

unsigned long current_time, blink_time;
unsigned long prev_time[8]; //array of 8 variables named "prev"
int shift;
int latch1;
int blink_latch;

void setup() {
  ledGrid.begin(); //must be done in setup for the LEDs to work.
  analogReadResolution(12); //0-4095 pot values
  analogReadAveraging(64);  //smooth the readings some
  pinMode(13, OUTPUT);
}


byte incomingByte = 0;
int raw1, smoothedPot1;
void loop() {
  current_time = millis();


  if (current_time - prev_time[0] > 33) {
    prev_time[0] = current_time;

    shift++;
    for ( int x_count = 0; x_count < 8; x_count++) {
      for ( int y_count = 0; y_count < 8; y_count++) {
        int xy_count = x_count + (y_count * 8); //goes from 0-64
        set_pixel_HSV(xy_count, shift + (x_count * y_count) / 64.0, 1, 1); // turn everything off. otherwise the last "frame" swill still show
      }
      set_pixel_HSV(0, 1 , 0, 1);
      ledGrid.show(); // after we've set what we want all the LEDs to be we send the data out through this function
    }
  }
  if (current_time - prev_time[1] > 2 && 1) {
    prev_time[1] = current_time;
    raw1 = analogRead(A0);
    smoothedPot1 = sm1.smooth(raw1);
  }



  if (current_time - prev_time[2] > interval1) {
    prev_time[2] = current_time;
    blink_time = micros();
    blink_latch = 1;
    digitalWrite(13, 1);
    Serial.print(raw1);
    Serial.print(" ");
    Serial.print(smoothedPot1);
    Serial.println();
  }

  if (blink_latch == 1) {
    if (micros() - blink_time > on_time1) {
      digitalWrite(13, 0);
      blink_latch = 0;
    }
  }


}// loop is over
