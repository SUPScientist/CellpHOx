# 03_GData_ReadLoop

## Creation Details
Developed by PJB in April 2018

## Goal
Send a "glast" command from Particle Electron to SeapHOx

## Usage
This code was deployed on a Particle Electron in Carlsbad Aquafarm/Agua Hedionda Lagoon
Particle Electron 3, notebook page 44
Deployment 1: 2018/04/17

## Notes
Deployed 2018/04/17. Deployment is ongoing as of 2018/07/09 but Electron hasn't been heard from since 2018/05/30.

## Integrations (Webhooks)
### Thingspeak
Data were sent to Thingspeak.com in two channels:
1) https://thingspeak.com/channels/445034/ which was the "raw" string sent from this firmware directly to the Particle Cloud and
2) https://thingspeak.com/channels/445047/ which was created purely in Thingspeak, taking channel 445034's results and parsing them using a Thingspeak-integrated MATLAB analysis.

This webhook looked like:
`{
    "event": "CpHOx",
    "deviceID": "260042000d51363034323832",
    "url": "https://api.thingspeak.com/update",
    "requestType": "POST",
    "noDefaults": true,
    "rejectUnauthorized": true,
    "form": {
        "api_key": "IGIU8M4Z8SNTDAAD",
        "field1": "{{PARTICLE_EVENT_VALUE}}"
    }
}`

### Google Sheets
Data were also sent to a Google Sheet following gusgonnet's directions [here](https://www.hackster.io/gusgonnet/pushing-data-to-google-docs-02f9c4).
This webhook looked like:
`{
    "event": "CpHOx",
    "deviceID": "260042000d51363034323832",
    "url": "https://script.google.com/macros/s/AKfycbw7lgCvNS1N_zNqsp-X4wPkGM6va3RQxp53L-r2WTS2yVaXeCU/exec",
    "requestType": "POST",
    "noDefaults": true,
    "rejectUnauthorized": true,
    "form": {
        "name": "{{PARTICLE_EVENT_VALUE}}"
    }
}`
Those data were scraped, parsed, and plotted using a Jupyter notebook [here](Dropbox/Self-Calibrating%20SeapHOx/Analysis/PJB-20180420_ScrapeGoogleSheet.ipynb)
