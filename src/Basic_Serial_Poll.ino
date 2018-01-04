/*
 * Project Basic_Serial_Poll
 * Description: Send "ts" command to SeapHOx and listen to response
 * Author: PJB
 * Date: 2018-01-03
 */

// Turn cell modem off for now
SYSTEM_MODE(MANUAL);

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
  Serial1.print("a");         // arbitrary
  delay(500);                   // let SeapHOx wake up
  Serial1.println("ts");        // take sample
  delay(500);                   // wait for response

	String s = Serial1.readString();
	Serial.printlnf("got %s", s.c_str());

  delay(60000);                 // repeat in a minute; TODO: change to be a clean minute
	Serial.println("REPEAT");
}
