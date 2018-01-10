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
const unsigned long TIMEOUT_SEAPHOX_MS = 20000; // Max wait time for SeapHOx response
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

}

// Give SeapHOx arbitrary wake character then "ts" command. Then listen for response.
void loop() {

	if(millis()-lastSend >= SEND_INTERVAL_MS){
		lastSend = millis();

		digitalWrite(max3232_switch, HIGH); // turn on MAX3232
		Serial1.end();	// clean out any residual junk in buffer
		delay(500);
		Serial1.begin(115200);

		// delay(500);
		// Serial1.print("a");         	// arbitrary wake char
		delay(500);
	  Serial1.println("ts");        // take sample

		// Parse out SeapHOx response; TODO: clean up malloc issues from using strdup by using free()
		s = Serial1.readString();			// read response
		const char* s_args = s.c_str();
		new_var = strtok(strdup(s_args), "\t");
		while (new_var != NULL) {
			Serial.println(new_var);
			new_var = strtok(NULL, " \t");
		}
		// delete s_args; // hopefully prevent memory leakage, also SOS
		// free((char*)s_args); // caused Elec to go into SOS mode!
		// free( (void*)(s_args) );

		delay(500);

		counter++;
	}

	digitalWrite(max3232_switch, LOW);
	// Turn off microcontroller and cellular.
	// Reset after seconds.
	// Ultra low power usage.
	// System.sleep(SLEEP_MODE_DEEP, SLEEP_TIME_SEC);

}
