/*
 * Project Basic_Serial_Poll
 * Description: Send "ts" command to SeapHOx and listen to response
 * Author: PJB
 * Date: 2018-01-03
 */

const char *eventName = "CpHOx";
FuelGauge batteryMonitor; // keep an eye on Particle's battery
int max3232_switch = A1; // DIO to turn on/off MAX3232 chip
int start_time;
int exec_time_sec;

SYSTEM_MODE(AUTOMATIC); // Turn cell modem off for now

const unsigned long TIMEOUT_SEAPHOX_MS = 5000; // Max wait time for SeapHOx response
const unsigned long SLEEP_TIME_SEC = 1800; // Deep sleep time

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

void setup() {
	// Cellular.off();

	start_time = millis(); // for calculating sleep time below

	// Sets switch pin to output and turns on
	// pinMode(max3232_switch, OUTPUT);
	// digitalWrite(max3232_switch, HIGH); // turn on MAX3232

  // To print to screen over USB-Serial
	Serial.begin(9600);

	// Wait for a line to arrive, max this out for now
	Serial1.setTimeout(TIMEOUT_SEAPHOX_MS);

}

// Give SeapHOx arbitrary wake character then "ts" command. Then listen for response.
void loop() {

	// Clean out any residual junk in buffer and restart serial port
	Serial1.end();
	delay(1000);
	Serial1.begin(115200);
	delay(500);

	// Get data in file after current file pointer
	// Serial1.print("a");
	// delay(500);
	Serial1.println("glast");

	// Read response
	s = Serial1.readString();			// read response
	String s2 = s.replace("Error.txt f_read error: FR_OK\r\n", "");
	const char* s_args = s2.c_str();
	char* each_var = strtok(strdup(s_args), "\t");
	Serial.println(s_args);

	// Parse response
	parseSeapHOx(each_var);

	// Publish to cloud
	publishSeapHOx();

	// Turn off microcontroller and cellular
	exec_time_sec = (millis()-start_time)/1000;
	System.sleep(SLEEP_MODE_DEEP, SLEEP_TIME_SEC-exec_time_sec);
	// delay(180000);
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

void publishSeapHOx(void){
	char data[100];
	int count_NC = 0;
	float cellVoltage = batteryMonitor.getVCell();
	float stateOfCharge = batteryMonitor.getSoC();

	// Should perhaps put this and Serial.print(data) into if statement (like parseSeapHOx, above)
	snprintf(data, sizeof(data), "%s,%s,%.3f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.02f,%.02f",
					 SeapHOx_Cell.Board_Date,
					 SeapHOx_Cell.Board_Time,
					 SeapHOx_Cell.Main_Batt_Volt ,
					 SeapHOx_Cell.V_FET_INT,
					 SeapHOx_Cell.V_FET_EXT ,
					 SeapHOx_Cell.V_Pressure,
					 SeapHOx_Cell.pHINT,
					 SeapHOx_Cell.O2,
					 SeapHOx_Cell.SBE37_Temp,
					 SeapHOx_Cell.SBE37_Salinity,
				 	 cellVoltage,
					 stateOfCharge);
					 // Also want seapHOx time/date and GPS

	Serial.println(data); // did we make it into here?

	// if (Particle.connected() == false) {
	// 	Serial.println("Wasn't Connected");
	// 	Particle.connect();
	// }

	Particle.publish(eventName, data, 60, PRIVATE);
	delay(4000); // wait for publish to go out before sleeping
}
// TODO: add GPS (?), see if
// we can reduce TIMEOUT_SEAPHOX_MS as it may not need to be that long
// maybe publish exec_time_sec
