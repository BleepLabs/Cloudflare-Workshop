
#include <WS2812Serial.h>
//we'll be using the Teensy audio library and it doesn't play nicely with neopixels.h or fastled
// so Paul of PJRC made this much more efficient version
const int num_of_leds = 65;
const int pin = 8; // only these pins can be used on the Teensy 3.2:  1, 5, 8, 10, 31
byte drawingMemory[num_of_leds * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[num_of_leds * 12]; // 12 bytes per LED
WS2812Serial ledGrid(num_of_leds, displayMemory, drawingMemory, pin, WS2812_GRB);

#define max_brightness 0.1


//This function is a little different than you might see in other libraries but it works pretty similar
// instead of 0-255 you see in other libraries this is all 0-1.0
// you can copy this to the bottom of any code as long as the declarations at the top in "led biz" are done

//set_pixel_HSV(led to change, hue,saturation,value aka brightness)
// led to change is 0-63
// all other are 0.0 to 1.0
// hue - 0 is red, then through the ROYGBIV to 1.0 as red again
// saturation - 0 is fully white, 1 is fully colored.
// value - 0 is off, 1 is the value set by max_brightness
// (it's not called brightness since, unlike in photoshop, we're going from black to fully lit up

//based on https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

void set_pixel_HSV(int pixel, float fh, float fs, float fv) {
	byte RedLight;
	byte GreenLight;
	byte BlueLight;

	byte h = fh * 255;
	byte s = fs * 255;
	byte v = fv * max_brightness * 255;

	h = (h * 192) / 256;  // 0..191
	unsigned int i = h / 32;   // We want a value of 0 thru 5
	unsigned int f = (h % 32) * 8;   // 'fractional' part of 'i' 0..248 in jumps

	unsigned int sInv = 255 - s;  // 0 -> 0xff, 0xff -> 0
	unsigned int fInv = 255 - f;  // 0 -> 0xff, 0xff -> 0
	byte pv = v * sInv / 256;  // pv will be in range 0 - 255
	byte qv = v * (256 - s * f / 256) / 256;
	byte tv = v * (256 - s * fInv / 256) / 256;

	switch (i) {
	case 0:
		RedLight = v;
		GreenLight = tv;
		BlueLight = pv;
		break;
	case 1:
		RedLight = qv;
		GreenLight = v;
		BlueLight = pv;
		break;
	case 2:
		RedLight = pv;
		GreenLight = v;
		BlueLight = tv;
		break;
	case 3:
		RedLight = pv;
		GreenLight = qv;
		BlueLight = v;
		break;
	case 4:
		RedLight = tv;
		GreenLight = pv;
		BlueLight = v;
		break;
	case 5:
		RedLight = v;
		GreenLight = pv;
		BlueLight = qv;
		break;
	}
	ledGrid.setPixelColor(pixel, RedLight, GreenLight, BlueLight);
}


class digitalSmoothing {

////////////smooth function
//based on https://playground.arduino.cc/Main/DigitalSmooth/
public:

#define filterSamples 31   // filterSamples should  be an odd number, no smaller than 3. 17 is a good balance
	//Increase for more smoothness but also more logarithmic

	int sensSmoothArray[filterSamples];   // array for holding raw sensor values

	int smooth(int input) {

		int j, k, temp, top, bottom;
		long total;
		static int i;
		static int sorted[filterSamples];
		boolean done;

		i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
		sensSmoothArray[i] = input;                 // input new data into the oldest slot

		// Serial.print("raw = ");

		for (j = 0; j < filterSamples; j++) { // transfer data array into anther array for sorting and averaging
		sorted[j] = sensSmoothArray[j];
		}

		done = 0;                // flag to know when we're done sorting
		while (done != 1) {      // simple swap sort, sorts numbers from lowest to highest
		done = 1;
		for (j = 0; j < (filterSamples - 1); j++) {
				if (sorted[j] > sorted[j + 1]) {    // numbers are out of order - swap
					temp = sorted[j + 1];
					sorted [j + 1] =  sorted[j] ;
					sorted [j] = temp;
					done = 0;
				}
			}
		}

		// throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
		bottom = max(((filterSamples * 15)  / 100), 1);
		top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
		k = 0;
		total = 0;
		for ( j = bottom; j < top; j++) {
		total += sorted[j];  // total remaining indices
			k++;
		}

		return total / k;    // divide by number of samples
	}
};



//https://playground.arduino.cc/Main/Fscale/ I reordered the input values from this example and changed the range

// fscale is like map but it allows you to shape the response from logarithmic to exponential. Chonky to snappy
// Shape is -1.0 for exponential, 0 for linear, 1.0 for Log
// fscale(input, from low,from high,to low,to high,shape) function is at bottom of the code

float fscale( float inputValue, float originalMin, float originalMax,
              float newBegin, float newEnd, float curve) {

	float OriginalRange = 0;
	float NewRange = 0;
	float zeroRefCurVal = 0;
	float normalizedCurVal = 0;
	float rangedValue = 0;
	boolean invFlag = 0;


	// condition curve parameter
	// limit range

	if (curve > 1) curve = 1;
	if (curve < -1) curve = -1;

	curve = (curve * -1.0) ; // - invert
	curve = pow(10, curve); // convert linear scale into logarithmic exponent for other pow function

	// Check for out of range inputValues
	if (inputValue < originalMin) {
		inputValue = originalMin;
	}
	if (inputValue > originalMax) {
		inputValue = originalMax;
	}

	// Zero Reference the values
	OriginalRange = originalMax - originalMin;

	if (newEnd > newBegin) {
		NewRange = newEnd - newBegin;
	}
	else
	{
		NewRange = newBegin - newEnd;
		invFlag = 1;
	}

	zeroRefCurVal = inputValue - originalMin;
	normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float


	// Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
	if (originalMin > originalMax ) {
		return 0;
	}

	if (invFlag == 0) {
		rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

	}
	else     // invert the ranges
	{
		rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
	}

	return rangedValue;
}