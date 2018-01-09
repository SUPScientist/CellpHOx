/*
 * Project Basic_Serial_Poll
 * Description: Send "ts" command to SeapHOx and listen to response
 * Author: PJB
 * Date: 2018-01-03
 */


int counter = 0;
int max3232_switch = A1; // DIO to turn on/off MAX3232 chip
SYSTEM_MODE(MANUAL); // Turn cell modem off for now
const unsigned long SEND_INTERVAL_MS = 60000; // How often we poll SeapHOx in millis
const unsigned long TIMEOUT_SEAPHOX_MS = 20000; // Max wait time for SeapHOx response
unsigned long lastSend = -1*SEND_INTERVAL_MS; // Use for wait timing; starting negative forces it to work on first loop
char* new_var;

void setup() {
	Cellular.off();

	// Sets switch pin to output and defaults to off
	pinMode(max3232_switch, OUTPUT);
	digitalWrite(max3232_switch, LOW);

  // To print to screen over USB-Serial
	Serial.begin(19200);

  // To communicate with peripheral (SeapHOx is peripheral to Particle)
	Serial1.begin(115200);

	// Wait for a line to arrive, max this out for now
	Serial1.setTimeout(TIMEOUT_SEAPHOX_MS);

  // Wait a few seconds then let us know program is running
  delay(5000);
  Serial.println("Beginning to poll");
}

// Give SeapHOx arbitrary wake character then "ts" command. Then listen for response.
void loop() {
	// Serial.println(millis()); // for debugging only

	// When we hit timing interval, send wake then "ts" and listen
	if (millis() - lastSend >= SEND_INTERVAL_MS) {
		lastSend = millis(); // don't put anything above this to keep loop timing tight

		digitalWrite(max3232_switch, HIGH); // turn on MAX3232
		delay(500);
	  Serial1.print("a");         	// arbitrary wake char
	  delay(500);                   // let SeapHOx wake up
	  Serial1.println("ts");        // take sample
		Serial.println("Taking sample now...");

		String s = Serial1.readString();
		const char* s_args = s.c_str();
		Serial.printlnf("got %s", s.c_str()); // s.c_str() gives args of s

		// Parse out SeapHOx response; TODO: clean up malloc issues from using strdup by using free()
		new_var = strtok(strdup(s_args), "\t");
		while (new_var != NULL) {
			Serial.println(new_var);
			new_var = strtok(NULL, " \t");
		}

		delay(500);
		digitalWrite(max3232_switch, LOW); // turn off MAX3232


		Serial.printlnf("Loop %i; REPEAT\n", counter);
		counter++;
  }

	// TODO: change delay to deep sleep
	delay(100); // in case it matters that otherwise it'll just spin at clock speed

}
