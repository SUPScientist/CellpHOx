# GPSandCell_test

A Particle project named GPSandCell_test

This isn't technically a fork of https://github.com/rickkas7/AssetTrackerRK as I used Particle Dev's desktop IDE to retrieve the code but it is based off of that. Specifically, it's based off of https://github.com/rickkas7/AssetTrackerRK/blob/master/examples/3_WakeOnMove/3_WakeOnMove.ino.

I removed the LIS3DH references and calls as I don't need accelerations for my application, only GPS + a separate peripheral instrument not yet incorporated in this project. My goal is to be able to poll GPS, poll the peripheral instrument, then go to sleep for a defined period of time. I will use SLEEP_MODE_DEEP as intervals will be longer than the 
