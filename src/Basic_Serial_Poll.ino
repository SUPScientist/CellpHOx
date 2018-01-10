/*
 * Project Basic_Serial_Poll
 * Description: Send "ts" command to SeapHOx and listen to response
 * Author: PJB
 * Date: 2018-01-03
 */


int counter = 0;
int max3232_switch = A1; // DIO to turn on/off MAX3232 chip
SYSTEM_MODE(MANUAL); // Turn cell modem off for now

const unsigned long SEND_INTERVAL_MS = 30000; // How often we poll SeapHOx in millis
const unsigned long TIMEOUT_SEAPHOX_MS = 60000; // Max wait time for SeapHOx response
const unsigned long SLEEP_TIME_SEC = 30; // Deep sleep time

unsigned long lastSend = -1*SEND_INTERVAL_MS; // Use for wait timing; starting negative forces it to work on first loop
char* new_var;
String s;

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

	digitalWrite(max3232_switch, HIGH); // turn on MAX3232
	delay(500);
  Serial1.print("a");         	// arbitrary wake char
  delay(500);                   // let SeapHOx wake up
  Serial1.println("ts");        // take sample
	Serial1.flush();

	Serial.println("Taking sample now...");
	Serial.flush();

	Serial.println(Serial1.readString());
	Serial.flush();

	Serial.printlnf("Loop %i; REPEAT\n", counter);
	Serial.flush();
	counter++;

	// Turn off microcontroller and cellular.
	// Reset after seconds.
	// Ultra low power usage.
	System.sleep(SLEEP_MODE_DEEP, SLEEP_TIME_SEC);

}


// s = Serial1.readString();			// read response
//
// // Parse out SeapHOx response; TODO: clean up malloc issues from using strdup by using free()
// const char* s_args = s.c_str();
// new_var = strtok(strdup(s_args), "\t");
// while (new_var != NULL) {
// 	Serial.println(new_var);
// 	new_var = strtok(NULL, " \t");
// }
