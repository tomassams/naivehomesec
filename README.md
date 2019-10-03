# Naive home security
Exam assignment course PG5500 Embedded Systems

This is a home exam solution made for the course PG5500 Embedded Systems at Kristiania University College.

## Assignment
The assignment given was to make an Internet connected "thing" with some sort of value, using Particle's Photon.

### Results
I ended up making some sort of home security / alarm system. It consists of mainly the Particle Photon, an ultrasonic sensor, a buzzer-speaker, an RFID-reader and two LEDs. The device is attached to the wall and the ultrasonic sensor pointed towards the doorway. Whenever the door opens, it will trigger the alarm. The alarm ringing is shown by having the speaker beeping as well as the LED light flashing red. To deactivate the alarm, you have to use a recognized RFID-card. I hard-coded two UID's that I had from cards at home, one being my student ID card. I also hooked it up to IFTTT to send me notifications whenever the alarm was triggered or deactivated.

Of course this isnt "truly" secure, as e.g. the RFID card UID can be overwritten and there isnt any encryption. Also you can just tear the device down from the wall and unplug it from the power source. But it still works as a basic concept.

### Demo and screenshots

<img src="video/demo.gif" alt="Screenshot" width="300">

<img src="screenshots/ifttt_alarm_state.PNG" alt="Screenshot" width="200"> <img src="screenshots/ifttt_fail_safe_offline_varsel.PNG" alt="Screenshot" width="200">

<img src="screenshots/ifttt_1.PNG" alt="Screenshot" width="200"> <img src="screenshots/ifttt_1.PNG" alt="Screenshot" width="200"> <img src="screenshots/ifttt_1.PNG" alt="Screenshot" width="200">
