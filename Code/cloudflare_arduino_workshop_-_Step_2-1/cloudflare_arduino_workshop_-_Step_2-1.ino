
#include <cloudflare_arduino_workshop.h>  //Library with LED and other code

int button1;
int prev_button1;
int pot1;
int led1;
unsigned long current_time;
unsigned long prev_time[8]; //array of 8 variables named "prev_time"

#define button_pin 2
#define pot_pin A0  //"A" pins are connected to the ADC
#define led_pin 10 //only some pins can do PWM

int shift;

void setup() {

  ledGrid.begin(); //must be done in setup for the LEDs to work.

  pinMode(button_pin, INPUT_PULLUP); //when using buttons this must always be done

}

void loop() {
  current_time = millis();

  if (current_time - prev_time[0] > 33) { //33 milliseconds is about 30Hz
    prev_time[0] = current_time;

    pot1 = analogRead(pot_pin); //10 bit read. returns 0-1023
    led1 = pot1 / 4;
    analogWrite(led_pin, led1); //output is 8 butes, 0-255
    button1 = digitalRead(button_pin); //0 or 1

    Serial.print(button1); //print the variables current value
    Serial.print(" "); //litteraly print a space
    Serial.print(pot1);
    Serial.println(); //print a return

    shift++;

    int xy_count;
    for ( int x_count = 0; x_count < 8; x_count++) {
      for ( int y_count = 0; y_count < 8; y_count++) {
        xy_count = x_count + (y_count * 8); //goes from 0-64
        set_pixel_HSV(xy_count, shift + (x_count * y_count) / 64.0, 1, 1);
      }
      float set_hue = .4 + (random(10) / 40.0);
      set_pixel_HSV(0, set_hue , .9, 1);

      ledGrid.show(); // after we've set what we want all the LEDs to be we send the data out through this function
    }
  }
}
