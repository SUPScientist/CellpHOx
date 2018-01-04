/*
 * Project Basic_Serial_Poll
 * Description: Send "ts" command to SeapHOx and listen to response
 * Author: PJB
 * Date: 2018-01-03
 */

SYSTEM_MODE(MANUAL); // Turn cell modem off for now
const unsigned long SEND_INTERVAL_MS = 30000; // How often we poll SeapHOx in millis
unsigned long lastSend = 0; // Use for wait timing


void setup() {
	Cellular.off();

  // To print to screen over USB-Serial
	Serial.begin(19200);

  // To communicate with peripheral (SeapHOx is peripheral to Particle)
	Serial1.begin(115200);

	// Wait up to 15 seconds for a line to arrive
	Serial1.setTimeout(15000);

  // Wait a few seconds then let us know program is running
  delay(5000);
  Serial.println("Beginning to poll");
}

// Give SeapHOx arbitrary wake character then "ts" command. Then listen for response.
void loop() {
	// When we hit timing interval, send wake then "ts" and listen
	if (millis() - lastSend >= SEND_INTERVAL_MS) {
		lastSend = millis();
	  Serial1.print("a");         // arbitrary
	  delay(500);                   // let SeapHOx wake up
	  Serial1.println("ts");        // take sample
	  delay(500);                   // wait for response (~ 10 seconds)

		String s = Serial1.readString();
		Serial.printlnf("got %s", s.c_str());
		Serial.println("REPEAT");
	}
	delay(100); // in case it matters that otherwise it'll just spin at clock speed
}
