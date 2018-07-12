#include "Particle.h"
#include "TinyGPS++.h"


// Example of Wake On Move with the AssetTracker and the Electron
//
// Official project location:
// https://github.com/rickkas7/LIS3DH


// System threading is required for this project
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

// Global objects
FuelGauge batteryMonitor;
TinyGPSPlus gps;

// Forward declaration
void displayInfo();

// This is the name of the Particle event to publish for battery or movement detection events
// It is a private event.
const char *eventName = "pHOx";

// Various timing constants
const unsigned long MAX_TIME_TO_PUBLISH_MS = 60000; // Only stay awake for 60 seconds trying to connect to the cloud and publish
const unsigned long MAX_TIME_FOR_GPS_FIX_MS = 120000; // Only stay awake for 3 minutes trying to get a GPS fix
const unsigned long TIME_AFTER_PUBLISH_MS = 4000; // After publish, wait 4 seconds for data to go out
const unsigned long TIME_AFTER_BOOT_MS = 5000; // At boot, wait 5 seconds before going to sleep again (after coming online)
const unsigned long SERIAL_PERIOD_MS = 5000;

// Stuff for the finite state machine
enum State { PUBLISH_STATE, SLEEP_STATE, SLEEP_WAIT_STATE, BOOT_WAIT_STATE, GPS_WAIT_STATE };
State state = GPS_WAIT_STATE;

unsigned long stateTime = 0;
unsigned long lastSerial = 0;
unsigned long startFix = 0;
bool gettingFix = false;

float latitude = 0;
float longitude = 0;

void setup() {
  // Cellular.off();
  // Always start here when waking up
  state = GPS_WAIT_STATE;
  Serial.println("turn cell on and wait for GPS fix");

  Serial.begin(9600);

  // The GPS module on the AssetTracker is connected to Serial1 and D6
  Serial1.begin(9600);

  // Settings D6 LOW powers up the GPS module
  pinMode(D6, OUTPUT);
  digitalWrite(D6, LOW);
  startFix = millis();
  gettingFix = true;
}


void loop() {
  while (Serial1.available() > 0) {
  if (gps.encode(Serial1.read())) {
  displayInfo();
  }
  }

  switch(state) {
  case GPS_WAIT_STATE:

    Particle.connect();

  if (gps.location.isValid()) {
  // Got a GPS fix
      latitude = gps.location.lat();
      longitude = gps.location.lng();
  state = PUBLISH_STATE;
  break;
  }
  if (millis() - stateTime >= MAX_TIME_FOR_GPS_FIX_MS) {
  Serial.println("failed to get GPS fix");
      latitude = 0;
      longitude = 0;
  state = PUBLISH_STATE;
  break;

  }
  break;

  case PUBLISH_STATE:

  if (Particle.connected()) {
  // The publish data contains 3 comma-separated values:
  // whether movement was detected (1) or not (0) The not detected publish is used for battery status updates
  // cell voltage (decimal)
  // state of charge (decimal)
  char data[64];
  float cellVoltage = batteryMonitor.getVCell();
  float stateOfCharge = batteryMonitor.getSoC();
  snprintf(data, sizeof(data), "online: %.02f,%.02f,%f,%f",
  cellVoltage, stateOfCharge, latitude, longitude);

  Particle.publish(eventName, data, 60, PRIVATE);
  Serial.println(data);

  // Wait for the publish to go out
  stateTime = millis();
  state = SLEEP_WAIT_STATE;
  }
  else {
  // Haven't come online yet
  if (millis() - stateTime >= MAX_TIME_TO_PUBLISH_MS) {
  // Took too long to publish, just go to sleep
        char data[64];
        float cellVoltage = batteryMonitor.getVCell();
        float stateOfCharge = batteryMonitor.getSoC();
        snprintf(data, sizeof(data), "offline: %.02f,%.02f,%f,%f",
            cellVoltage, stateOfCharge, latitude, longitude);

          Serial.println(data);

  state = SLEEP_STATE;
  }
  }
  break;

  case SLEEP_WAIT_STATE:
  if (millis() - stateTime >= TIME_AFTER_PUBLISH_MS) {
  state = SLEEP_STATE;
  }
  break;

  case SLEEP_STATE:

    Serial.println("going to sleep");
    delay(500);

    // Sleep
    int secondsToSleep = 180;

    // Test delay vs. sleep
    // delay(180000);
    stateTime = millis();
    System.sleep(SLEEP_MODE_DEEP, secondsToSleep);

    // It'll only make it here if the sleep call doesn't work for some reason
    state = GPS_WAIT_STATE;
    break;

  }

}

void displayInfo()
{
  if (millis() - lastSerial >= SERIAL_PERIOD_MS) {
    lastSerial = millis();

    char buf[128];
    if (gps.location.isValid()) {
      snprintf(buf, sizeof(buf), "%f,%f", gps.location.lat(), gps.location.lng());
      if (gettingFix) {
        gettingFix = false;
        unsigned long elapsed = millis() - startFix;
        Serial.printlnf("%lu milliseconds to get GPS fix", elapsed);
      }
    }
    else {
      strcpy(buf, "no location");
      if (!gettingFix) {
        gettingFix = true;
        startFix = millis();
      }
    }
    Serial.println(buf);
  }
}
