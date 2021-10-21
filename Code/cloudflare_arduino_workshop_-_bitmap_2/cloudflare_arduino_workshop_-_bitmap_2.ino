//Using arrays as bitmaps
// two pots are used to set the position of a repeating symbol
// these notes have more comments as they were for a beginner calss

#include <cloudflare_arduino_workshop.h>

#define pot2_pin A0
#define pot1_pin A1

unsigned long current_time;
unsigned long prev_time[8]; //array of 8 variables named "prev"
int glyph_location;
float set_hue;
int xy_sel;
int xy_count;
int x_pot;
int y_pot;
int rate1 = 30;
int x_shift, y_shift;

#define glyph_width  4
#define glyph_height 4

//you can just use a one dimnsional array and arrange it visually however you'd like
byte glyph[glyph_width * glyph_height] = {
  0, 0, 0, 0,
  0, 1, 0, 0,
  2, 2, 2, 0,
  0, 3, 0, 0,
};

void setup() {
  ledGrid.begin(); //must be done in setup for the LEDs to work.

 analogReadResolution(12); //0-4095 pot values
  analogReadAveraging(64);  //smooth the readings some
}


void loop() {
  current_time = millis();

  if (current_time - prev_time[0] > rate1) {
    prev_time[0] = current_time;

    //these will go higer than 10 so we can move the shapes areound a little more
    // map wasn't working as well for me so i went back to this method
    x_pot = (analogRead(pot1_pin) / 4095.0) * 10; //0-10
    y_pot = 10 - ((analogRead(pot2_pin) / 4095.0) * 10); //subtrat by 10 to flip it around 10 - 0
    xy_sel = x_pot + (y_pot * 8);

    //x_count goes from 0-7 and so does y_count but since we have it arranged
    // with one for loop after another we get x_count=0 for y_count from 0-7,
    // then x_count=1 for y_count from 0-7 and so on
    // this way we can more easily deal with the two dimensional LED array

    for ( int x_count = 0; x_count < 8; x_count++) {
      for ( int y_count = 0; y_count < 8; y_count++) {
        xy_count = x_count + (y_count * 8); //goes from 0-64
        set_pixel_HSV(xy_count, 0, 0, 0); // turn everything off. otherwise the last "frame" swill still show

        //% is remainder aka modulo. remainder = dividend % divisor. https://www.arduino.cc/reference/en/language/structure/arithmetic-operators/remainder/
        // x_shift will never equal or go over glyph_width. It'll jsut keep wrapping around
        // so 7%8= 7, 8%8=0, 10%8=2, 17%8=1.
        // you can use this calculator to try it out //https://www.wolframalpha.com/input/?i=9%257%3D

        x_shift = (x_count + x_pot) % glyph_width;
        y_shift = (y_count + y_pot) % glyph_height;
        //the bitmap is moved by changing where we're looking at it. Instead of jsut combining x_count and y_cound we shift it
        glyph_location = ((x_shift) + ((y_shift) * glyph_height));

        if (glyph[glyph_location] == 2) {
          //set_pixel_HSV(led to change, hue,saturation,value aka brightness)
          set_pixel_HSV(xy_count, .3 , 1, 1); //xy_count is used here, not glyph_location. Otherwise nothing would move
        }

        if (glyph[glyph_location] == 1) {
          set_pixel_HSV(xy_count, .7 , 1, 1);
        }
        if (glyph[glyph_location] == 3) {
          float r1 = random(0, 100) / 500.0;
          set_pixel_HSV(xy_count, .8 , 1, r1);
        }
      }
    }
    ledGrid.show(); // after we've set what we want all the LEDs to be we send the data out through this function
  } //timing "if" over

}// loop is over
