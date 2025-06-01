#include <FastLED.h>

#define LED_PIN_1 0
#define LED_PIN_2 1
#define NUM_LEDS 20
#define BRIGHTNESS 128

#define PWM_INPUT_PIN 2 // PB2 (pin 7 on ATTiny85)

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

// Animation state
uint8_t hue = 0;
uint8_t pulseBrightness = 0;
uint16_t position = 0;
unsigned long prevMillis = 0;
const uint8_t pulseSpeed = 5;
const uint8_t scrollSpeed = 2;

// PWM input
volatile uint16_t pwmWidth = 0;
volatile bool newPulse = false;
uint8_t appState = 0;

// --- Setup ---
void setup() {
  pinMode(PWM_INPUT_PIN, INPUT);
  FastLED.addLeds<WS2812B, LED_PIN_1, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN_2, GRB>(leds2, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Timer1 setup
  TCCR1 = 0;
  GTCCR |= (1 << PSR1);
  TCCR1 |= (1 << CS10); // No prescaler

  // Enable PCINT on PB2
  GIMSK |= (1 << PCIE);
  PCMSK |= (1 << PCINT2);
  sei(); // Enable global interrupts
}

// --- Loop ---
void loop() {
  if (newPulse) {
    noInterrupts();
    uint16_t pulse = pwmWidth;
    newPulse = false;
    interrupts();
    appState = getAppState(pulse);
  }

  switch (appState) {
    case 0: allOff(); break;
    case 1: purplePulse(); break;
    case 2: rainbowScroll(); break;
    case 3: policeStrobe(); break;
  }
}

// --- PWM to App State Mapping ---
uint8_t getAppState(uint16_t pwm) {
  if (pwm < 1250) return 0;
  if (pwm < 1500) return 1;
  if (pwm < 1750) return 2;
  return 3;
}

// --- Animations ---
void allOff() {
  fill_solid(leds1, NUM_LEDS, CRGB::Black);
  fill_solid(leds2, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(30);
}

void purplePulse() {
  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis >= 30) {
    prevMillis = currentMillis;
    static uint8_t pulseAngle = 0;
    pulseBrightness = sin8(pulseAngle) / 2 + 127;
    pulseAngle += pulseSpeed;

    fill_solid(leds1, NUM_LEDS, CHSV(200, 255, pulseBrightness));
    fill_solid(leds2, NUM_LEDS, CHSV(200, 255, pulseBrightness));
    FastLED.show();
  }
}

void rainbowScroll() {
  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis >= 30) {
    prevMillis = currentMillis;
    static uint8_t pulseAngle = 0;
    pulseBrightness = sin8(pulseAngle) / 2 + 127;
    pulseAngle += pulseSpeed;

    position = (position + scrollSpeed) % (NUM_LEDS * 2);

    for (int i = 0; i < NUM_LEDS; i++) {
      uint8_t shiftedPos1 = (i + position) % (NUM_LEDS * 2);
      uint8_t shiftedPos2 = (NUM_LEDS * 2 - i + position) % (NUM_LEDS * 2);

      uint8_t hue1 = map(shiftedPos1, 0, NUM_LEDS * 2, 0, 255);
      uint8_t hue2 = map(shiftedPos2, 0, NUM_LEDS * 2, 0, 255);

      leds1[i] = CHSV(hue + hue1, 255, pulseBrightness);
      leds2[i] = CHSV(hue + hue2, 255, pulseBrightness);
    }

    hue++;
    FastLED.show();
  }
}

void policeStrobe() {
  static uint8_t phase = 0;
  static unsigned long lastUpdate = 0;
  static bool isRed = true;
  static bool doWhiteBurst = false;

  unsigned long now = millis();

  const uint16_t flashOn = 75;
  const uint16_t flashOff = 50;
  const uint16_t pause = 100;
  const uint16_t whiteBurstTime = 100;

  if (doWhiteBurst) {
    if (now - lastUpdate < whiteBurstTime) {
      fill_solid(leds1, NUM_LEDS, CRGB::White);
      fill_solid(leds2, NUM_LEDS, CRGB::White);
      FastLED.show();
      return;
    } else {
      doWhiteBurst = false;
      phase = 0;
      isRed = true;
    }
  }

  if (phase < 6) {
    if (phase % 2 == 0 && now - lastUpdate >= flashOff) {
      fill_solid(leds1, NUM_LEDS, isRed ? CRGB::Red : CRGB::Blue);
      fill_solid(leds2, NUM_LEDS, isRed ? CRGB::Red : CRGB::Blue);
      FastLED.show();
      lastUpdate = now;
      phase++;
    } else if (phase % 2 == 1 && now - lastUpdate >= flashOn) {
      fill_solid(leds1, NUM_LEDS, CRGB::Black);
      fill_solid(leds2, NUM_LEDS, CRGB::Black);
      FastLED.show();
      lastUpdate = now;
      phase++;
    }
  } else {
    if (now - lastUpdate >= pause) {
      if (!isRed) {
        doWhiteBurst = true; // After blue, trigger white burst
        lastUpdate = now;
      } else {
        phase = 0;
        isRed = !isRed;
        lastUpdate = now;
      }
    }
  }
}

// --- PWM Interrupt (PCINT0) ---
ISR(PCINT0_vect) {
  static bool lastState = LOW;
  bool currentState = PINB & (1 << PWM_INPUT_PIN);

  if (currentState && !lastState) {
    TCNT1 = 0;
  } else if (!currentState && lastState) {
    pwmWidth = TCNT1 / 8; // 8MHz clock, convert to µs
    newPulse = true;
  }

  lastState = currentState;
}
