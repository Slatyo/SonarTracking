# SonarTracking

![start](https://github.com/Slatyo/SonarTracking/assets/21217262/54750955-15da-4237-87ea-9f2cf5576bfe)

SonarTracking is just a working title for now.  
Experimenting with Ultrasonic sensors for underwater usage.  
Maybe we will add more sensors on the way.  
  
This project is just a way for me to learn my ways around microcontrollers and such things.  

### Testresults

<details>
  <summary>Test 1 - 20/03/2024 - 9 meters</summary>
  Example Pulse outoput:  
  
  <img src="resources/images/PULSE_DRAFT_1.jpeg" alt="PULSE_DRAFT" width="1000"/>
    
  First valid check roughly 9 meters.  
  The echo sounder we had on board showed similar values for this depth.  
  After roughly 10-12 meters it started to get unprecise and at a depth of 18 meters or more it stopped reliably working in the first test.  
  The video just shows the setup used on the boat.
  
  <img src="resources/images/9m.jpeg" alt="9m-test" width="350"/>  

  https://github.com/Slatyo/SonarTracking/assets/21217262/6796917c-54d1-4716-85a9-6f0faa388260
</details>

<details>
  <summary>Test 2 - 22/03/2024 - 20 meters</summary>  
  At roughly 20meters depth:  
  
  ![20m](https://github.com/Slatyo/SonarTracking/assets/21217262/f5e91c90-0611-429c-a63a-58fbe2c87f64)  
  We used the Garmin ECHOMAP Ultra 122sv to validate it:  
  
  ![garmin_20m](https://github.com/Slatyo/SonarTracking/assets/21217262/66a725a9-0d79-4b1e-82b0-0c00c28289bf)  

  For the next test I will try and get the sensor as stationary as possible to get stable readings.  
  With the current setup we will not achieve more then 20 meters depth under water due to the sensors limitation.  

  I am looking into other options but I want to try and keep the costs of the project manageable.  

  Further steps as of now will probably include something like adding a GPS Sensor, maybe rebuild the project and use the ESP8266 ESP-12F as a sender and rebuild the Arduino into a receiver?  
  
</details>




### Circuit
Work in progress
