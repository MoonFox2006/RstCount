#include <Arduino.h>

const uint8_t LED_PIN = LED_BUILTIN;
const bool LED_LEVEL = LOW;

const uint32_t WAIT_TIME = 2000; // 2 sec.
const uint8_t RST_MAX = 3;

const uint16_t RTC_SIGN = 0xABCD;

static uint8_t getRstCount() {
  uint32_t rtcdata;

  ESP.rtcUserMemoryRead(0, &rtcdata, sizeof(rtcdata));
  if (((uint16_t)(rtcdata >> 16) == RTC_SIGN) && ((uint8_t)~((rtcdata >> 8) & 0xFF) == (rtcdata & 0xFF)))
    return rtcdata & 0xFF;
  return 0;
}

static void setRstCount(uint8_t value) {
  uint32_t rtcdata;

  rtcdata = (RTC_SIGN << 16) | ((uint8_t)(~value) << 8) | value;
  ESP.rtcUserMemoryWrite(0, &rtcdata, sizeof(rtcdata));
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ! LED_LEVEL);

  uint8_t rstCount = getRstCount();

  Serial.print(rstCount);
  Serial.println(F(" reset(s)"));
  if (rstCount++ >= RST_MAX) {
    setRstCount(0);
    Serial.println(F("Bingo!"));
    Serial.flush();
    ESP.deepSleep(0);
  } else {
    uint32_t start;

    setRstCount(rstCount);
    Serial.println(F("Restart board next times to deep sleep..."));
    start = millis();
    while (millis() - start < WAIT_TIME) {
      digitalWrite(LED_PIN, ! digitalRead(LED_PIN));
      delay(250);
    }
    digitalWrite(LED_PIN, ! LED_LEVEL);
    setRstCount(0);
  }
}

void loop() {
  digitalWrite(LED_PIN, ! digitalRead(LED_PIN));
  delay(500);
}
