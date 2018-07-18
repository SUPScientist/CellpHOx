/*
 * Project 05_glast_GPS
 * Description:
 * Author: PJB
 * Date: 9-July-2018
 */

 //
#include "TinyGPS++.h"
#include "Serial5/Serial5.h"
SYSTEM_MODE(MANUAL); // For testing only; COMMENT OUT for deployment
// SYSTEM_MODE(AUTOMATIC); // For deployment; comment out for testing
SYSTEM_THREAD(ENABLED);

// Forward declaration
void parseGPS(); // for GPS polling/parsing
void publishSeapHOxFake(); // pretend there's a peripheral SeapHOx even if there isn't
void publishSeapHOx(); // send data string to cloud
void parseSeapHOx(); // parse response from SeapHOx of "ts" "gdata" or "glast" command from Electron

// Set up globals and constants and structs
const char *eventName = "CpHOx";
FuelGauge batteryMonitor; // keep an eye on Particle's battery
const unsigned long TIMEOUT_SEAPHOX_MS = 5000; // Max wait time for SeapHOx response
const unsigned long SLEEP_TIME_SEC = 1800; // Deep sleep time
const unsigned long SERIAL_PERIOD_MS = 180000;

String s;

struct SeapHOx {
  int Sample_Number;     //0
  char *Board_Date;       //1
  char *Board_Time;         //2
  float Main_Batt_Volt;   //3 yes V
  float V_Therm;          //4
  float V_FET_INT;        //5
  float V_FET_EXT;        //6
  float isolated_power_Volt ;//7
  float Controller_Temp;//8
  float Durafet_Temp;   //9 yes C
  float V_Pressure;     //10
  float pHINT;          //11 yes pH
  float pHEXT;            //12 yes pH
  float Counter_Leak;   //13
  float Substrate_Leak;   //14
  float Optode_Model;   //15
  float Optode_SN;      //16
  float O2 ;           //17 yes [µM]
  float O2_Saturation ; //18
  float Optode_Temp;    //19
  float Dphase;         //20
  float Bphase;         //21
  float Rphase;         //22
  float Bamp;           //23
  float Bpot;           //24
  float Ramp;           //25
  float Raw_Temp;       //26
  float SBE37_Temp;     //27 yes C
  float SBE37_Cond;     //28 yes cm^^-1
  float SBE37_Salinity; //29 yes s
  float SBE37_Date;     //30
  float SBE37_Time;     //31
};
SeapHOx SeapHOx_Cell;

// The TinyGPS++ object
TinyGPSPlus gps;
unsigned long lastSerial = 0;
unsigned long lastPublish = 0;
unsigned long startFix = 0;
bool gettingFix = false;

// Start here upon wakeup
void setup() {
  Cellular.off(); // COMMENT FOR REAL TEST

  // To print to screen over USB-Serial
  Serial.begin(9600);

  // SeapHOx
  // Wait for a line to arrive
  Serial5.setTimeout(TIMEOUT_SEAPHOX_MS);

  // GPS
  // Settings D6 LOW powers up the GPS module
  // The GPS module on the AssetTracker is connected to Serial1 and D6
  Serial1.begin(9600);
  pinMode(D6, OUTPUT);
  digitalWrite(D6, LOW);
}

// Give SeapHOx arbitrary wake character then "glast" command. Then listen for response.
void loop() {

  // Clean out any residual junk in buffer and restart serial port
  Serial5.end();
  delay(1000);
  Serial5.begin(115200);
  delay(500);

  // SeapHOx Sampling
  // Get data in file after current file pointer
  Serial5.println("glast");

  // Read response
  s = Serial5.readString();			// read response
  String s2 = s.replace("Error.txt f_read error: FR_OK\r\n", "");
  const char* s_args = s2.c_str();
  char* each_var = strtok(strdup(s_args), "\t");
  Serial.println(s_args);

  // Parse response
  // parseSeapHOx(each_var); // UNCOMMENT FOR REAL TEST

  // GPS Sampling
  while (Serial1.available() > 0) {
    // Serial.write(Serial1.read());
    if (gps.encode(Serial1.read())) {
      displayInfo();
    }
  }

  // Publish to cloud
  publishSeapHOxFake(); // REMOVE "FAKE" FOR REAL TEST

  // Calculate sleep time
  int nextSampleMin = 5; // sample at 5 past the hour
  int currentHour = Time.hour();
  int currentSecond = Time.now()%86400; // in UTC

  // Calculate seconds since midnight of next sample
  int nextSampleSec = (currentHour+1)*60*60+nextSampleMin*60; // sample at this time
  int secondsToSleep = nextSampleSec - currentSecond; // UNCOMMENT for deployment

  // int secondsToSleep = 180; // COMMENT OUT during deployment
  Serial.printf("Sleep for %d seconds\n", secondsToSleep);
  // System.sleep(SLEEP_MODE_DEEP, secondsToSleep);

  delay(180000); // COMMENT OUT during deployment
}

void parseSeapHOx(char* new_var){
 	// Parse SeapHOx response
 	int count = 0;
 	char* parsed[300];

 	while (new_var != NULL) {
 		parsed[count] = new_var;
 		count++;
 		// Serial.println(new_var);
 		new_var = strtok(NULL, " \t");
 	}

 	if (parsed[0][8] == '#') {
 		SeapHOx_Cell.Board_Date 			= parsed[1];
 		SeapHOx_Cell.Board_Time 			= parsed[2];
 		SeapHOx_Cell.Main_Batt_Volt   = strtof(parsed[3], NULL);  //3 yes V
 		SeapHOx_Cell.V_Therm          = strtof(parsed[4], NULL);
 		SeapHOx_Cell.V_FET_INT        = strtof(parsed[5], NULL);
 		SeapHOx_Cell.V_FET_EXT        = strtof(parsed[6], NULL);
 		SeapHOx_Cell.Durafet_Temp     = strtof(parsed[9], NULL); //9 yes C
 		SeapHOx_Cell.V_Pressure       = strtof(parsed[10], NULL);
 		SeapHOx_Cell.pHINT            = strtof(parsed[11], NULL); //11 yes pH
 		SeapHOx_Cell.pHEXT            = strtof(parsed[12], NULL); //12 yes pH
 		SeapHOx_Cell.O2               = strtof(parsed[17], NULL); //17 yes [µM]
 		SeapHOx_Cell.O2_Saturation    = strtof(parsed[18], NULL);
 		SeapHOx_Cell.Optode_Temp      = strtof(parsed[19], NULL);
 		SeapHOx_Cell.SBE37_Temp       = strtof(parsed[20], NULL); //27 yes C
 		SeapHOx_Cell.SBE37_Cond       = strtof(parsed[21], NULL); //28 yes cm^^-1
 		SeapHOx_Cell.SBE37_Salinity   = strtof(parsed[22], NULL); //29 yes s

    Serial.printf("\nParsed SeapHOX \n Date-time %s-%s\n Main_Batt_Volt %.5f\n V_Therm %.5f\n V_FET_INT %.5f\n V_FET_EXT %.5f\n Durafet_Temp %2.5f\n V_Pressure %.5f\n pHINT %.5f\n pHEXT %.5f\n O2 %.5f\n O2_Saturation  %.5f\n Optode_Temp %.5f\n SBE37_Temp %.5f\n SBE37_Cond %.5f\n SBE37_Salinity %.5f\n",
    							SeapHOx_Cell.Board_Date,
    							SeapHOx_Cell.Board_Time,
    							SeapHOx_Cell.Main_Batt_Volt ,
                  SeapHOx_Cell.V_Therm ,
                  SeapHOx_Cell.V_FET_INT,
                  SeapHOx_Cell.V_FET_EXT ,
                  SeapHOx_Cell.Durafet_Temp ,
                  SeapHOx_Cell.V_Pressure,
                  SeapHOx_Cell.pHINT,
                  SeapHOx_Cell.pHEXT,
                  SeapHOx_Cell.O2,
                  SeapHOx_Cell.O2_Saturation,
                  SeapHOx_Cell.Optode_Temp,
                  SeapHOx_Cell.SBE37_Temp,
                  SeapHOx_Cell.SBE37_Cond,
                  SeapHOx_Cell.SBE37_Salinity
                );
  }
}

void displayInfo()
{
	if (millis() - lastSerial >= SERIAL_PERIOD_MS) {
		lastSerial = millis();

		char buf[128];
		if (gps.location.isValid()) {
			snprintf(buf, sizeof(buf), "%f,%f,%f", gps.location.lat(), gps.location.lng(), gps.altitude.meters());
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

		// if (Particle.connected()) {
		// 	if (millis() - lastPublish >= PUBLISH_PERIOD) {
		// 		lastPublish = millis();
		// 		Particle.publish("gps", buf);
		// 	}
		// }
	}

}

void publishSeapHOxFake(void){
 	char data[100];
 	int count_NC = 0;
 	float cellVoltage = batteryMonitor.getVCell();
 	float stateOfCharge = batteryMonitor.getSoC();

 	snprintf(data, sizeof(data), "Sampling! It's been %d seconds since 1-Jan-1970.", Time.now());

 	Serial.println(data); // did we make it into here?

 	// if (Particle.connected() == false) {
 	// 	Serial.println("Wasn't Connected");
 	// 	Particle.connect();
 	// }

 // 	Particle.publish(eventName, data, 60, PRIVATE);
 	delay(4000); // wait for publish to go out before sleeping
}
 // void publishSeapHOx(void){
 // 	char data[100];
 // 	int count_NC = 0;
 // 	float cellVoltage = batteryMonitor.getVCell();
 // 	float stateOfCharge = batteryMonitor.getSoC();
 //
 // 	// Should perhaps put this and Serial.print(data) into if statement (like parseSeapHOx, above)
 // 	snprintf(data, sizeof(data), "%s,%s,%.3f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.02f,%.02f,%f,%f",
 // 					 SeapHOx_Cell.Board_Date,
 // 					 SeapHOx_Cell.Board_Time,
 // 					 SeapHOx_Cell.Main_Batt_Volt ,
 // 					 SeapHOx_Cell.V_FET_INT,
 // 					 SeapHOx_Cell.V_FET_EXT ,
 // 					 SeapHOx_Cell.V_Pressure,
 // 					 SeapHOx_Cell.pHINT,
 // 					 SeapHOx_Cell.O2,
 // 					 SeapHOx_Cell.SBE37_Temp,
 // 					 SeapHOx_Cell.SBE37_Salinity,
 // 				 	 cellVoltage,
 // 					 stateOfCharge,
  //          gps.location.lat(),
  //          gps.location.lng()
  //         );
 //
 // 	Serial.println(data); // did we make it into here?
 //
 // 	// if (Particle.connected() == false) {
 // 	// 	Serial.println("Wasn't Connected");
 // 	// 	Particle.connect();
 // 	// }
 //
 // 	Particle.publish(eventName, data, 60, PRIVATE);
 // 	delay(4000); // wait for publish to go out before sleeping
 // }

 // TODO: add GPS (?)

 // TODO: add logic to force connection attempt to stop after designated time

 // TODO: Add particle function for TS