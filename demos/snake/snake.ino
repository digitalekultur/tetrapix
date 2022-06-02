#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN        6
#define NUMPIXELS 143
#define LED_BRIGHTNESS 15     // general brightness of the LEDs from 0-255
#define DEMO_DELAY     100    // general delay of the demo (in milliseconds)

Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t ledBaseColors[] = {
    ring.Color(LED_BRIGHTNESS, 0, 0),                           // red
    ring.Color(0, LED_BRIGHTNESS, 0),                           // green
    ring.Color(0, 0, LED_BRIGHTNESS),                           // blue
    ring.Color(LED_BRIGHTNESS, LED_BRIGHTNESS, 0),              // yellow
    ring.Color(LED_BRIGHTNESS, 0, LED_BRIGHTNESS),              // purple
    ring.Color(0, LED_BRIGHTNESS, LED_BRIGHTNESS),              // cyan
    ring.Color(LED_BRIGHTNESS, LED_BRIGHTNESS, LED_BRIGHTNESS)  // white
  };

void setup()
{
  ring.begin();
  randomSeed(analogRead(0));
}

void loop()
{
  resetRing();
  delay(3000);

  simpleCircling();
  tailCircling();
  pairCircling();
  colorWipe();
  randomFlashes();
  flash();
  randomColors();
  rainbow();
  alternatingColors();
  alternateLeds();
  pendulum();
  fire();
}



void fire()
{
  int generation=0;
  byte i=0;
  uint32_t ledColor;
  uint8_t r, g, b;

  for(generation=0; generation<150; generation++)
  {
    // set some hotspots
    if (generation % 5 == 0) {
      for(i=0; i<NUMPIXELS; i++) {
        if (random(0, 100) > 60) {
          ring.setPixelColor(i, ring.Color(LED_BRIGHTNESS, (uint8_t)LED_BRIGHTNESS/5*3, 0));
        }
      }
    }

    // fadeout the colored LEDs
    for(i=0; i<NUMPIXELS; i++) {
      ledColor = ring.getPixelColor(i);

      r = (uint8_t)(ledColor >> 16);
      g = (uint8_t)(ledColor >> 8);
      if (r > 0) {
        r -= max(3, (uint8_t)LED_BRIGHTNESS/5);
        r = max(0, r);
      }
      if (g > 0) {
        g -= max(3, (uint8_t)LED_BRIGHTNESS/5);
        g = max(0, g);
      }
      ring.setPixelColor(i, ring.Color(r, g, 0));
    }

    ring.show();
    delay(DEMO_DELAY);
  }
}

void pendulum()
{
  byte generation = 5;
  int i=0, h=(int)NUMPIXELS/2;
  uint32_t ledColor = ring.Color(LED_BRIGHTNESS, (uint8_t)LED_BRIGHTNESS/5*3, 0);

  do {
    for(i=0; i<h; i++) {
      ring.setPixelColor(i, ledColor);
      ring.show();
      delay(getSpeed(i));
      ring.setPixelColor(i, ring.Color(0, 0, 0));
      ring.show();
    }
    for(i=h-1; i>=0; i--) {
      ring.setPixelColor(i, ledColor);
      ring.show();
      delay(getSpeed(i));
      ring.setPixelColor(i, ring.Color(0, 0, 0));
      ring.show();
    }
    for(i=NUMPIXELS-1; i>h; i--) {
      ring.setPixelColor(i, ledColor);
      ring.show();
      delay(getSpeed(NUMPIXELS-i));
      ring.setPixelColor(i, ring.Color(0, 0, 0));
      ring.show();
    }
    for(i=h+1; i<NUMPIXELS; i++) {
      ring.setPixelColor(i, ledColor);
      ring.show();
      delay(getSpeed(NUMPIXELS-i));
      ring.setPixelColor(i, ring.Color(0, 0, 0));
      ring.show();
    }

    generation--;
  } while(generation > 0);
}

int getSpeed(int i)
{
  return (int)(DEMO_DELAY/3) + 5 * i*i;
}



void alternateLeds()
{
  for (byte colorIndex=0; colorIndex<sizeof(ledBaseColors)/4-1; colorIndex++) {
    for(byte mod=2; mod<=6; mod++) {
      for(byte generation=0; generation<6; generation++) {
        for(byte i=0; i<NUMPIXELS; i++) {
          if (i%mod==generation%mod) {
            ring.setPixelColor(i, ledBaseColors[colorIndex]);
          } else {
            ring.setPixelColor(i, ring.Color(0, 0, 0));
          }
        }
        ring.show();
        delay(DEMO_DELAY);
      }
    }
  }
  resetRing();
}

void alternatingColors()
{
  for (byte colorIndex=0; colorIndex<sizeof(ledBaseColors)/4-1; colorIndex++) {
    for(byte generation=0; generation<5; generation++) {
      for(byte i=0; i<NUMPIXELS; i++) {
        if (i%2==generation%2) {
          ring.setPixelColor(i, ledBaseColors[colorIndex]);
        } else {
          ring.setPixelColor(i, ledBaseColors[colorIndex+1]);
        }
      }
      ring.show();
      delay(DEMO_DELAY * 3);
    }
  }
}

void rainbow()
{
  uint8_t r, g, b;

  for(float offset = 0.0; offset<PI*5; offset+=0.4) {
    r = (uint8_t)((sin(offset*1.0) + 1)/2 * LED_BRIGHTNESS);
    g = (uint8_t)((sin(offset*1.5) + 1)/2 * LED_BRIGHTNESS);
    b = (uint8_t)((sin(offset*2.0) + 1)/2 * LED_BRIGHTNESS);

    for(byte i=0; i<NUMPIXELS; i++) {
        ring.setPixelColor(i, ring.Color(r, g, b));
    }
    ring.show();
    delay(DEMO_DELAY);
  }
}

void randomColors()
{
  uint8_t r, g, b;

  for (byte generation=0; generation<75; generation++) {
    for(byte i=0; i<NUMPIXELS; i++) {
        r = random(0, LED_BRIGHTNESS);
        g = random(0, LED_BRIGHTNESS);
        b = random(0, LED_BRIGHTNESS);
        ring.setPixelColor(i, ring.Color(r, g, b));
    }
    ring.show();
    delay(DEMO_DELAY);
  }
}

void flash()
{
  for (byte generation=0; generation<35; generation++) {
    resetRing();
    delay(DEMO_DELAY);
    for(byte i=0; i<NUMPIXELS; i++) {
        uint32_t ledColor = ring.Color(LED_BRIGHTNESS, LED_BRIGHTNESS, LED_BRIGHTNESS);
        ring.setPixelColor(i, ledColor);
    }
    ring.show();
    delay(10);
  }
}

void randomFlashes()
{
  for (byte generation=0; generation<75; generation++) {
    resetRing();
    delay(DEMO_DELAY);
    for(byte i=0; i<NUMPIXELS; i++) {
        if (random(0, 100) > 90) {
          ring.setPixelColor(i, ring.Color(LED_BRIGHTNESS, LED_BRIGHTNESS, LED_BRIGHTNESS));
        }
    }
    ring.show();
    delay(10);
  }
}

void colorWipe()
{
  for (byte colorIndex=0; colorIndex<sizeof(ledBaseColors)/4; colorIndex++) {
    for(byte i=0; i<NUMPIXELS; i++) {
      ring.setPixelColor(i, ledBaseColors[colorIndex]);
      ring.show();
      delay(DEMO_DELAY);
    }
  }
}

void pairCircling()
{
  for (byte colorIndex=0; colorIndex<sizeof(ledBaseColors)/4-1; colorIndex++) {
    for(byte i=0; i<NUMPIXELS; i++) {
        ring.setPixelColor(i, ledBaseColors[colorIndex]);
        ring.setPixelColor(NUMPIXELS-i, ledBaseColors[colorIndex+1]);
        ring.show();
        delay(DEMO_DELAY);
        ring.setPixelColor(i, ring.Color(0, 0, 0));
        ring.setPixelColor(NUMPIXELS-i, ring.Color(0, 0, 0));
        ring.show();
    }
  }
}

void tailCircling()
{
  uint32_t color;
  uint8_t r, g, b;
  byte tailLength = 5;

  for (byte colorIndex=0; colorIndex<sizeof(ledBaseColors)/4; colorIndex++) {
    for(byte i=0; i<NUMPIXELS; i++) {
      for(byte i=0; i<NUMPIXELS; i++) {
        color = ring.getPixelColor(i);

        r = (uint8_t)(color >> 16);
        g = (uint8_t)(color >> 8);
        b = (uint8_t)color;
        if (r > 0) {
          r -= max(3, (uint8_t)LED_BRIGHTNESS/tailLength);
          r = max(0, r);
        }
        if (g > 0) {
          g -= max(3, (uint8_t)LED_BRIGHTNESS/tailLength);
          g = max(0, g);
        }
        if (b > 0) {
          b -= max(3, (uint8_t)LED_BRIGHTNESS/tailLength);
          b = max(0, b);
        }
        ring.setPixelColor(i, ring.Color(r, g, b));
      }

      ring.setPixelColor(i, ledBaseColors[colorIndex]);
      ring.show();
      delay(DEMO_DELAY);
    }
  }
}

void simpleCircling()
{
  for (byte colorIndex=0; colorIndex<sizeof(ledBaseColors)/4; colorIndex++) {
    for(byte i=0; i<NUMPIXELS; i++) {
      ring.setPixelColor(i, ledBaseColors[colorIndex]);
      ring.show();
      delay(DEMO_DELAY);
      ring.setPixelColor(i, ring.Color(0, 0, 0));
      ring.show();
    }
  }
}

void resetRing()
{
    for(byte i=0; i<NUMPIXELS; i++) {
        ring.setPixelColor(i, ring.Color(0, 0, 0));
        ring.show();
    }
}
