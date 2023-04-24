# Group 3 big D project 2023
This is a project made at the SUTD-NP-SST Big-D Camp in 2023. This project was made in 3 days, from 31 January to 2 February

## Problem

The problem we want to tackle is the safety of lift users in emergencies such as earthquakes.

According to research from the University of Colorado Boulder, it is estimated that a magnitude 7 earthquake may cause 22,000 people to be trapped between floors in 4,500 lifts that lack emergency power. Therefore, a system which can inform and save people stuck in lifts during earthquakes will be crucial.

## Concept

When the accelerometer and gyroscope detects irregular movement of the lift due to a possible earthquake, it will move the user to the nearest floor of the building, stop the lift and open the door, displaying a message that an earthquake is occurring. This will prompt the user to leave quickly, without being trapped in the lift due to reasons like power outages.

## System design

The accelerometer will track the vertical speed of the lift and monitor it by storing it. While the gyroscope will do the same for the angular velocity. When an extreme value is obtained, the motors will move the user to the next level and halt. The LCD screen will show a warning message and the speaker will tell the users to leave the lift.

<img width="1134" alt="Screenshot 2023-04-24 at 8 16 23 PM" src="https://user-images.githubusercontent.com/90018110/234023544-dde24153-1728-4b84-82ae-1c659a37f117.png">

### Materials

1. **Accelerometer:** to track and monitor up and down velocity of lift
2. **Gyroscope:** to track angular velocity to monitor shakiness of lift
3. **PIR sensor:** determine whether there is someone in the lift
4. **DC Motor:** to represent the activated pulley system of the lift turning on or off
5. **Servo Motor:** to represent the lift doors opening or closing
6. **LCD screen:** inform lift user that the lift will stop and to leave the lift
7. **Buzzer:** inform lift user to leave the lift

<img width="606" alt="Screenshot 2023-04-24 at 9 27 22 PM" src="https://user-images.githubusercontent.com/90018110/234023727-0009e448-35be-4610-8ae0-8d4fb0f6e3b0.png">

https://user-images.githubusercontent.com/90018110/234023374-b2351cab-46f8-429d-a9ba-eb7d3a88173c.mp4

