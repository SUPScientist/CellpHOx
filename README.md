# CellpHOx
Particle Electron projects to poll a Scripps SeapHOx

Proper GitHub use would probably include branches, not new directories. I will try to teach myself to use branches in the near future but wanted to post this repo as it was. Here are some descriptions of the subdirectories.

01_ and 02_ were test directories for serial communication using SeapHOx's `gdata` command. 01_ and 02_ worked sporadically for serial polling (01_ didn't ever try to publish to Particle cloud; 02_ did). A `gdata` command to SeapHOx sends back all samples collected by SeapHOx since last `gdata` command.

03_ incorporates a new `glast` command given to the SeapHOx which responds with only the last sample (not all samples since last `gdata` command). 04_ is based on 03_ but makes a marked improvement to timing. 04_GData_ReadLoop2 takes 03_GData_ReadLoop and improves the timing. 03_GData_ReadLoop calculates sleep time every loop by getting a rough elapsed time for setup() and loop(). It gets start time in setup() and subtracts that from an end time at the end of loop() to calculate elapsed time. This elapsed time is subtracted from the desired sampling periodicity to calculate sleep time.

However, Particle Electrons go through a "hidden" startup process (that is, it occurs in system firmware prior to calling `setup()` in order to connect to cell towers. This process can seemingly take minutes and occurs before setup() so this time is not accounted for in the calculation of sleep time. As a result, the Electron wakes up to sample at intervals that are meaningfully longer than the defined wakeup interval.

04_GData_ReadLoop2 seeks to solve this timing issue by getting the current local time from the cloud every time it connects and setting its sleep time to wake up the same amount of time after the hour (or any defined user interval).

05_glast_GPS attempts to improve 04_ with the addition of GPS polling using the [Particle Asset Tracker 2](https://www.particle.io/products/hardware/asset-tracker/) but I couldn't get GPS fixes so I started stripping away functionality in GPS_only_test and GPSandCell_test directories.

06_glast_noGPS works as intended for polling the SeapHOx at hourly intervals and transmitting data. It removes the GPS functionality which was too slow and power hungry for this application where we just want Particle Electron and SeapHOx to be awake for as short as necessary to transmit data from SeapHOx to Electron and Electron to cloud.

07_ and 08_ are tests of specific functionality which are not needed for future development.

09_CellpHOx_Vipul contains updates by Vipul Lakhani to 06_glast_noGPS intended to add error handling, different sampling interval possibilities, a command line interface (under development), and other features. 
