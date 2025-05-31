#include <FastLED.h>

#define LED_PIN_1 0    // First LED strip data pin
#define LED_PIN_2 1    // Second LED strip data pin
#define NUM_LEDS 20    // LEDs per strip
#define BRIGHTNESS 128 // Max brightness (0-255); lowered to increase efficiency

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

// Animation control variables
uint8_t hue = 0;         // Starting hue
uint8_t pulseBrightness = 0; // Current brightness level
uint16_t position = 0;   // Rotation position
unsigned long prevMillis = 0;
const uint8_t pulseSpeed = 5; // Higher = slower pulsation
const uint8_t scrollSpeed = 2; // Higher = slower rotation

// Custom sine wave function for smooth pulsation
uint8_t sineWave(uint8_t angle) {
  return sin8(angle) / 2 + 127;
}

# FastLED initialization; if you're using something besides WS2812B strips, you'd note that here as long as they were supported by FastLED.
# GRB is the internal order of the different colored LEDs that compose each addressable diode.
# If everything else works properly but your colors are shifted inappropriately, try switching GRB to RGB.

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN_1, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN_2, GRB>(leds2, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Update animations every 30ms
  if(currentMillis - prevMillis >= 30) {
    prevMillis = currentMillis;

    // Update pulse brightness using sine wave
    static uint8_t pulseAngle = 0;
    pulseBrightness = sineWave(pulseAngle);
    pulseAngle += pulseSpeed;

    // Update rotation position
    position = (position + scrollSpeed) % (NUM_LEDS * 2);

    // Create rotating rainbow pattern
    for(int i = 0; i < NUM_LEDS; i++) {
      // Calculate shifted position for both strips
      uint8_t shiftedPos1 = (i + position) % (NUM_LEDS * 2);
      uint8_t shiftedPos2 = (NUM_LEDS * 2 - i + position) % (NUM_LEDS * 2);
      
      // Calculate hues
      uint8_t hue1 = map(shiftedPos1, 0, NUM_LEDS * 2, 0, 255);
      uint8_t hue2 = map(shiftedPos2, 0, NUM_LEDS * 2, 0, 255);
      
      // Apply colors with pulsating brightness
      leds1[i] = CHSV(hue + hue1, 255, pulseBrightness);
      leds2[i] = CHSV(hue + hue2, 255, pulseBrightness);
    }

    // Slowly shift overall hue
    hue += 1;

    FastLED.show();
  }
}
