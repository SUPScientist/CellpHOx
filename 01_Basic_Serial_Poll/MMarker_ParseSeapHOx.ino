#include <PrintEx.h> // This is downloaded from Github
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stream.h>


struct SeapHOX {
  int Sample_Number;     //0
  int Board_Date;       //1
  int Board_Time;         //2
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
  float  O2 ;           //17 yes [µM]
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


SeapHOX SeapHOX_FOCE;
int SeapHOXGlobleTrigger_1 = 0;// 0 = do nothing, 1 = Start Sample, 2 = Stop Sample



elapsedMillis oneSecloop, Controlloop, tenSecloop, LoopTime; //Timer variables for the

int i, b = 0; // Multi use variable for loops and counters


void setup() {

  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial4.begin(115200);


  SeapHOXGlobleTrigger_1 = 1; //Wake the seapHOx 1





}

void loop(){ // The loop is time spliced into sections. Fast code that runs at max loop time ie checking the serial ports for data,
            // Control code that runes at the control loop time for the ph controller
            // Once a second for polling command variables

  //-----------------Fast Execute Code---------------//
  //Code that needs to execute each Loop
  //  if (Serial4.available()) {    // If anything comes in Serial1 (pins 0 & 1)
  //   Serial.write(Serial4.read());   // read it and send it out Serial (USB)
  //


  ReadSeapHOX1();

  //-----------------End Fast Execute Code-----------//

  //-----------------One Second Execute Code---------------//
  //Code that needs to execute each Second
  if (oneSecloop >= 1000) {
    LoopTime = 0;




    SeapHOX_1_Control(); // Send Commands to the SeapHOx based on the SeapHOXGlobleTrigger_1



    oneSecloop = oneSecloop - 1000;
    i = LoopTime;
    //   Serial.printf("oneSecloop %d \n", i);
  }
  //-----------------End Fast Execute Code-----------//



  //-----------------Control Execute Code---------------//
  //Code that needs to execute in the control Loop
  if (Controlloop >= 10000) {
    LoopTime = 0;
    //Insert Code
    SeapHOXGlobleTrigger_1 = 3; // Read the seaphox Data



    Controlloop = Controlloop - 10000;
    i = LoopTime;
    //Serial.printf("                      Control loop %d \n", i);
  }
  //-----------------End Control Execute Code-----------//


  //-----------------Ten Second Execute Code---------------//
  //Code that needs to execute ten seconds Loop
  if (tenSecloop >= 60000) {
    LoopTime = 0;
    //Insert Code




    tenSecloop = tenSecloop - 20000;
    i = LoopTime;

  }
  //-----------------End Ten Second Execute Code-----------//
}


//-----------------------------------------------------------------------
// Controls the messages sent to the SeapHOXs - Uses Globle Variable SeapHOXGlobleTrigger
// 0 = do nothing
// 1 = Start the seapHOX - Set this value to start
// 2 = Stop  the SeapHOX - Set this value to stop
// 3 = Get the data

void SeapHOX_1_Control() {
  char *Sleep[10] = {"\r", "stop\r", "\r", "4\r"};// Put the seaphox th sleep
  char *Wake[10] = { "\r", "\r", "2\r", "\r"};// Wake the seaphox
  char *Sample[10] =  {"\r", "ts\r", "gdata\r"};//Get the data
  static int b;    // general loop counter
  static int Run; //if run = 1 the take a sample


  if (SeapHOXGlobleTrigger_1 == 2) { //Stop the SeapHOX
    Run = 0;//Make Sure the run command is not issued
    if (b < 4 ) {
      Serial4.write(Sleep[b]);
      b++;
      if (b == 4) {
        b = 0;
        SeapHOXGlobleTrigger_1 = 0;
        Run = 0;

      }
    }
  }

  if (SeapHOXGlobleTrigger_1 == 1) { //Start the SeapHOX

    if (b < 3 ) {
      Serial4.write(Wake[b]);
      b++;
      if (b == 3) {
        b = 0;
        SeapHOXGlobleTrigger_1 = 0;
        Run = 1;
        Serial.println("Starting SeapHOX FOCE");


      }
    }
  }
  if (Run == 1 && SeapHOXGlobleTrigger_1 == 3) { //Get data the SeapHOX

    if (b < 4) {
      Serial4.write(Sample[b]);
      b++;
      if (b == 4) {
        b = 0;
        SeapHOXGlobleTrigger_1 = 0;

      }
    }
  }

}



//-----------------------------------------------------------------------
// Parses the strings for the SeapHOXs
void ReadSeapHOX1(void) {
  static char strParams[300] = "";
  char ParseString[300] = "";

  char *parsed[300];
  static int  count = 0;
  static int  testcount = 0;
  char CharSting[230]= "";
  int marker;
  char *ch;
  int dataready = 0;
  int b = 0;
  char *ParsedString;
  int i;

  static char tester;
  static char oldtester; //holds the last value of the read char-used to delete multiple space chars

  for(i=0;i<=230;i++){//this is critical to init the string does not work other wise!!!!
    CharSting[i] =' ';
  }
  if (Serial4.available() ) { // see if there is some data


    tester = Serial4.read(); // read the serial port


    if (tester == '#') { // see if the # which is the first char of the seabird data
      count++;

    }
    if (count > 0) {// Add chars to the a charactor arry as they come in
      strParams[count] = tester;
      count++;
      if (count > 238) { // do it to the end as sometimes Null not add
        count = 0;
        dataready = 1;

      }
    }
  }

  if (dataready == 1) { //if a full set of data is ready parse it

    for (i = 0; i < 240; i++) { // seabird sends a lot of formatting - extra tabs, nulls and spaces


      if (strParams[i] == 9) { //remove tabs 9 = tab ascii
        ParseString[count] = ' ';
        count++;

      }
      else {
        ParseString[count] = strParams[i];
        count++;
      }

    }


    count = 0; // reset the count
// The returne string is space delimited. There are extra spaces put in the string that change inconsistently this means simple counting cannot be used. Extra spaces need to be removed
    for ( i = 0; i < 229; i++) {
      if (ParseString[i] == 32 && oldtester == 32) {// remove additional spaces
        i++;
      }
      else {
        CharSting[count] = ParseString[i];
        count++;
      }
      oldtester = ParseString[i];
    }


    CharSting[0]='a'; // there is always a persistent null sent just change it to a char
    CharSting[230]='\0';//add the null on the end of the array to make it a proper string



    count = 0;
    dataready = 0;

   ch = strtok(CharSting, " ");
    while (ch != NULL) {
      parsed[count] = ch;
      count++;
      ch = strtok(NULL, " ");
    }

    if (parsed[0][1] == '#') {

      SeapHOX_FOCE.Main_Batt_Volt   = strtof(parsed[3], NULL);  //3 yes V
      SeapHOX_FOCE.V_Therm          = strtof(parsed[4], NULL);
      SeapHOX_FOCE.V_FET_INT        = strtof(parsed[5], NULL);
      SeapHOX_FOCE.V_FET_EXT        = strtof(parsed[6], NULL);
      SeapHOX_FOCE.Durafet_Temp     = strtof(parsed[9], NULL); //9 yes C
      SeapHOX_FOCE.V_Pressure       = strtof(parsed[10], NULL);
      SeapHOX_FOCE.pHINT            = strtof(parsed[11], NULL); //11 yes pH
      SeapHOX_FOCE.pHEXT            = strtof(parsed[12], NULL); //12 yes pH
      SeapHOX_FOCE.O2               = strtof(parsed[17], NULL); //17 yes [µM]
      SeapHOX_FOCE.O2_Saturation    = strtof(parsed[18], NULL);
      SeapHOX_FOCE.Optode_Temp      = strtof(parsed[19], NULL);
      SeapHOX_FOCE.SBE37_Temp       = strtof(parsed[27], NULL); //27 yes C
      SeapHOX_FOCE.SBE37_Cond       = strtof(parsed[28], NULL); //28 yes cm^^-1
      SeapHOX_FOCE.SBE37_Salinity   = strtof(parsed[29], NULL); //29 yes s


      Serial.printf("\nParsed SeapHOX FOCE \n Main_Batt_Volt %.5f\n V_Therm %.5f\n V_FET_INT %.5f\n V_FET_EXT %.5f\n Durafet_Temp %2.5f\n V_Pressure %.5f\n pHINT %.5f\n pHEXT %.5f\n O2 %.5f\n O2_Saturation  %.5f\n SeapHOX_FOCE.Optode_Temp %.5f\n SBE37_Temp %.5f\n SBE37_Cond %.5f\n SBE37_Salinity %.5f\n",
                    SeapHOX_FOCE.Main_Batt_Volt ,
                    SeapHOX_FOCE.V_Therm ,
                    SeapHOX_FOCE.V_FET_INT,
                    SeapHOX_FOCE.V_FET_EXT ,
                    SeapHOX_FOCE.Durafet_Temp ,
                    SeapHOX_FOCE.V_Pressure,
                    SeapHOX_FOCE.pHINT,
                    SeapHOX_FOCE.pHEXT,
                    SeapHOX_FOCE.O2,
                    SeapHOX_FOCE.O2_Saturation,
                    SeapHOX_FOCE.Optode_Temp,
                    SeapHOX_FOCE.SBE37_Temp,
                    SeapHOX_FOCE.SBE37_Cond,
                    SeapHOX_FOCE.SBE37_Salinity
                   );

      count = 0;


    }


  }
}
