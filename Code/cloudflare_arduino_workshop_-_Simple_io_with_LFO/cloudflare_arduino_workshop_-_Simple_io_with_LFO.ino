//Potentiometer control blink rate
//Button steps through modes

long current_time, prev_time, prev_time1, prev_time2;
float pot1;
long led_out;
long mode;
long button_read, prev_button_read;
long lfo1;
long lfo1_mode;

#define led_pin 10
#define button_pin 2

void setup() {
  pinMode(led_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP);
}

void loop() {
  current_time = millis();

  pot1 = analogRead(A0); //returns 0-1023

  prev_button_read = button_read;
  button_read = digitalRead(button_pin); //returns 0 or 1

  if (prev_button_read == 1 && button_read == 0) { //button "fell" it was not being pressed and now it is
    mode++;
    if (mode > 2) {
      mode = 0;
    }
  }

  if (current_time - prev_time > pot1) {
    prev_time = current_time;
    led_out = !led_out; //set led_out to the opposite of led_out, flips between 0 and 1
  }

  if (mode == 0) {
    digitalWrite(10, led_out);
  }
  if (mode == 1) {
    digitalWrite(10, 1);
  }
  if (mode == 2) {
    analogWrite(led_pin, lfo1); //accept 0-255
  }


  if (current_time - prev_time1 > 20) {
    prev_time1 = current_time;
    Serial.println(lfo1);

  }

  if (current_time - prev_time2 > 5) {
    prev_time2 = current_time;

    if (lfo1_mode == 1) {
      lfo1++;
    }
    if (lfo1_mode == 0) {
      lfo1--;
    }
    if (lfo1 > 255) { //255 is the higest value you can analogwrite by defualt, 8 bits
      lfo1 = 255;
      lfo1_mode = 0;
    }
    if (lfo1 < 0) {
      lfo1 = 0;
      lfo1_mode = 1;
    }

  }

}//loop is over
