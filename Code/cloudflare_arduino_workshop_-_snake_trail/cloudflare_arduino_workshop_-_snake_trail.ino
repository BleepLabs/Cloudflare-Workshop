//Snake with adjustable length
// move with two pots 

#include <cloudflare_arduino_workshop.h>


#define pot1_pin A0
#define pot2_pin A1

unsigned long current_time;
unsigned long prev_time[8]; //array of 8 variables named "prev"

float set_hue;
int xy_sel;
int xy_count;
int x_pot;
int y_pot;
int rate1 = 30;
int rate2 = 250;
int rate3 = 50;
int led1_state = 0;  // The state of LED1 LOW and 0 are synonymous
int led2_state = 0;  // The LED2 state LOW and 0 are synonymous
float colors[4] = {.2, .6, .1, .9};
int x_sel;
int y_sel;
#define snake_length 6
int prev_pos[snake_length];
int pos_count, last_xy_sel;
int first_position = 1;
int trail_type;
int pot[4];

void setup() {
  ledGrid.begin(); //must be done in setup for the LEDs to work.

  analogReadResolution(12); //0-4095 pot values
  analogReadAveraging(64);  //smooth the readings some
}


void loop() {
  current_time = millis();

  if (current_time - prev_time[0] > rate1) {
    prev_time[0] = current_time;

    last_xy_sel = xy_sel;
    pot[0] = analogRead(pot1_pin);
    pot[1] = analogRead(pot2_pin);
    x_sel = (pot[0] / 4095.0) * 8; //0-7
    y_sel = 8 - ((pot[1] / 4095.0) * 8); //7-0. we need an extra value to flip it around oterwise we get 0-6
    xy_sel = x_sel + (y_sel * 8);
    Serial.println(xy_sel);

    if (xy_sel != last_xy_sel) {
      //the prev_pos[] array should be filled with the first place the curcor so they won't all be at 0
      // to get this to only happen once we set first_position to 1 in the definition section then cahnge it to 0 so this won't happen again
      // we coutl do it in setup but then we'd need to read the post and everthing there too
      
      if (first_position == 1) {
        for (int i = snake_length - 1; i > -1; i--) {
          prev_pos[i] = xy_sel;
        }
        first_position = 0;
      }

      //shift each space in the array over. 2 goes to 3, 1 goes to 2, etc.
      // this will work no matter tha snake length
      for (int i = snake_length - 1; i > 0; i--) {
        prev_pos[i] = prev_pos[i - 1];
      }

      prev_pos[0] = xy_sel; //where it is now

    }

    //x_count goes from 0-7 and so does y_count but since we have it arranged
    // with one for loop after another we get x_count=0 for y_count from 0-7,
    // then x_count=1 for y_count from 0-7 and so on
    // this way we can more easily deal with the two dimensional LED array
    for ( int x_count = 0; x_count < 8; x_count++) {
      for ( int y_count = 0; y_count < 8; y_count++) {
        xy_count = x_count + (y_count * 8); //goes from 0-64
        set_pixel_HSV(xy_count, 0, 0, 0); // turn everything off. otherwise the last "frame" swill still show
      }
    }

    for ( int trails = snake_length - 1; trails > 0; trails--) {
      float fade = 1.0 - (float(trails) / float(snake_length));
      set_pixel_HSV(prev_pos[trails], .4 + (fade / 3.0), 1, fade - .1);
    }

    set_pixel_HSV(prev_pos[0], .4, 1, 1);

    ledGrid.show(); // after we've set what we want all the LEDs to be we send the data out through this function
    //timing "if" over
  }
}// loop is over
