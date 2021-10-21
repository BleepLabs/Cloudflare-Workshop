//A random walking snake that leaves a trail
// Here two arrays are used to remeber the past locations of the snake
// and to fade the brightness out.
// pot 1 controls color

#include <cloudflare_arduino_workshop.h>

#define pot1_pin A0
#define pot2_pin A1

unsigned long current_time;
unsigned long prev_time[8]; //array of 8 variables named "prev_time[0] to prev_time[7]"

int xy_sel;
int xy_count;
int x_pot;
int y_pot;
int rate1 = 30;
int x_sel;
int y_sel;

#define snake_length 20
int prev_pos[snake_length]; //you can only do this with a #define, varibles won't work
float fade_level[snake_length];
int trail;
float fade;
float set_hue;
int pos_count, last_xy_sel; //both of these raribles will be integers set to 0
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

  if (current_time - prev_time[1] > 100) {
    prev_time[1] = current_time;
    if (random(10) > 5) { //if a random roll of the dice that retunrs 0-9 is over 5 do this
      x_sel++;
    }
    //% is remainder aka modulo. remainder = dividend % divisor. https://www.arduino.cc/reference/en/language/structure/arithmetic-operators/remainder/
    //when x_sel reaches 7 it gets wrapped around to 0
    // so 6%7= 6,7%7=0, 9%7=2, 15%7=1.
    // you can use this calculator to try it out //https://www.wolframalpha.com/input/?i=9%257%3D
    // in this situation saying if (x_sel>=7){x_sel=0;} would work beacuse we're only incremnting by 1 so we won't have 9%7=2 come up

    x_sel %= 7;

    if (random(10) > 5) {
      y_sel++;
    }
    y_sel %= 7;

  }

  if (current_time - prev_time[0] > rate1) {
    prev_time[0] = current_time;

    set_hue = (analogRead(pot1_pin) / 4095.0); //0.0 to 1
    last_xy_sel = xy_sel;
    xy_sel = x_sel + (y_sel * 8);

    if (xy_sel != last_xy_sel) {
      //shift each space in the array over. 2 goes to 3, 1 goes to 2, etc.
      for (int i = snake_length - 1; i > 0; i--) {
        prev_pos[i] = prev_pos[i - 1];
        fade_level[i] = fade_level[i - 1];
      }

      fade_level[1] = 1.0; // the newest trail gets full brightness
      prev_pos[0] = xy_sel; //The cursor or head idk waht to call it
    }

    //x_count goes from 0-7 and so does y_count but since we have it arranged
    // with one for loop after another we get x_count=0 for y_count from 0-7,
    // then x_count=1 for y_count from 0-7 and so on
    // this way we can more easily deal with the two dimensional LED array

    //Here we just clear the screen
    for ( int x_count = 0; x_count < 8; x_count++) {
      for ( int y_count = 0; y_count < 8; y_count++) {
        xy_count = x_count + (y_count * 8); //goes from 0-64
        set_pixel_HSV(xy_count, 0, 0, 0); // turn everything off. otherwise the last "frame" swill still show
      }
    }
    //now the tail is lit up at the points in the array and at their coresponding brightnesses
    for ( int trail = 0; trail < snake_length; trail++) {
      float sh = set_hue + (float(trail) / float(snake_length));
      if (sh > 1) {
        sh -= 1;
      }
      set_pixel_HSV(prev_pos[trail], sh, 1, fade_level[trail]);
    }
    set_pixel_HSV(prev_pos[0], set_hue, 1, 1); //The cursor or head idk waht to call it

    //fade out each spot in the array seperatly
    for ( int i = 0; i < snake_length; i++) {
      fade_level[i] *= .92;
      if (fade_level[i] <= .01) { //an arbitray cuttoff since it will never quite get to 0 through multiplication
        fade_level[i] = 0;
      }
    }

    ledGrid.show(); // after we've set what we want all the LEDs to be we send the data out through this function

  } //timing "if" is over


}
// loop is over
