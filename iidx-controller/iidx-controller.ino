#include "src/HID/LUFAConfig.h"
#include <LUFA.h>
#include <FastLED.h>
#include "src/HID/IIDXHID.h"
#include "src/IO/Buttons.h"
#include "src/IO/Encoder.h"
#include "src/IO/LEDs.h"
#include "src/Configuration.h"

#define LED_PIN 18
#define NUM_LEDS 20
#define LED_STRIP_VOLTAGE 5
#define LED_STRIP_MILLIAMPS 1200

CRGB leds[NUM_LEDS];
#define TT_COLOR CHSV(160, 200, 170)

static configuration_struct *config;
int encoderState = 0;

void setup() {
    initialise_configuration();
    initialise_buttons();
    initialise_encoder();
    initialise_leds();
    get_configuration(&config);

    delay(500);
    setup_hardware();
    GlobalInterruptEnable();

    FastLED.setMaxPowerInVoltsAndMilliamps(LED_STRIP_VOLTAGE, LED_STRIP_MILLIAMPS);
    FastLED.setCorrection(TypicalSMD5050);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(255);

    for (int i = 0; i < NUM_LEDS; i++)
        leds[i] = TT_COLOR;

    FastLED.show();
}

void loop() {
    HID_task();
    USB_USBTask();

    if (config->led_mode == 0 || config->led_mode == 2) {
        if ((millis() - get_last_led_update()) > 1200 || config->led_mode == 2) {
            // No HID LED update for 3s (or pure reactive mode): go reactive
            write_leds(get_button_state(), false);
        }
    }
    
    encoderState = get_encoder_state();
    uint8_t hue = (encoderState * 2) % 255;

    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(hue, 255, 255);
    }
    FastLED.show();
}



