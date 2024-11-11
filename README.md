# RFID_Attendence-Firmware
There are many methods to track the attendance of an individual. A few years ago, when Radio Frequency Identification (RFID) electronics were still in their adolescence stages, the time stamping technique was greatly followed. As software programming and technology progressed, especially in electronics, they gave way to a much more secure and reliable arrangement known as RFID Attendance System.

The RFID-based attendance system here is developed on the ESP8266 platform, with a PHP + Javascript backend. It can be used in places such as schools, colleges, factories, and private organizations to register attendance of students, teachers, employees, etc. It can be set to automatically tabulate their working hours as well.

When the person with the correct RFID card swipes  their RFID tag, their arrival time will be logged by the system. Usually, when the same person swipes his/her RFID tag again, the system will log it as him/her leaving.


#System Overview
We have used the NodeMCU ESP8266 development board with MF-RC522 Module to send the card UID to our PHP Web application, which stores data in the website's database. Basically, the admin plays an important role in the management of this system.


#Logic Flow
[Flowchart here]


#Code Review
[Mention main components of the code]


#Deploy Yourself
The RFID-based attendance system is developed to be very easy to setup on your server....
