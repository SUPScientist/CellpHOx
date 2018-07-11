# GData_ReadLoop2

## Creation Details
Developed by PJB in June 2018.

Tested on the bench with Particle.publish() sending dummy data to the cloud (i.e., this was not tested in the field or even with a peripheral SeapHOx).

04_GData_ReadLoop2 takes 03_GData_ReadLoop and improves the timing. 03_GData_ReadLoop calculates sleep time every loop by getting a rough elapsed time for setup() and loop(). It gets start time in setup() and subtracts that from an end time at the end of loop() to calculate elapsed time. This elapsed time is subtracted from the desired sampling periodicity to calculate sleep time.

However, Particle Electrons go through a hidden startup process in order to connect to cell towers. This process can seemingly take minutes and occurs before setup() so this time is not accounted for in the calculation of sleep time. As a result, the Electron wakes up to sample at intervals that are meaningfully longer than the defined wakeup interval.

04_GData_ReadLoop2 seeks to solve this timing issue by getting the current local time from the cloud every time it connects and setting its sleep time to wake up the same amount of time after the hour (or any defined user interval).
