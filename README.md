# Self Leveling Quadcopter
[Video of the Drone Flying](https://youtu.be/ctLI9JnbgVA)

![](ImagesAndVideos/FrontController.jpg)
>Front of the controller

![](ImagesAndVideos/BackController.jpg)
>Back of the controller

![](ImagesAndVideos/TopDrone.jpg)
>Topview of the drone

Sorry for the mess of Electrical Tape I ran out of Plexi Glass and Screws. Also the frame cracked so I put Electrical Tape over the screws for the motors just in case)

I have writen 2 versions of the code Manual and SelfLevel. Manual you use the joystick to change the amount of power going to each motor. Self Level you use the joystick to change the tilt angle and the arduino does all the leveling for you.

## Getting Started
These instructions will get you a flying drone.

### Prerequisites

#### Parts List
  * 2 Arduinos 
  * 3 10k Resistors
  * 1 10k Potentiometer
  * 1 KY-023 Analog Joystick
  * 3 Push Buttons
  * 1 MPU6050 Gyroscope and Accelerometer
  * 1 Wireless Receivers
  * 1 Wireless Transmitters
  * 4 BLDC Motors 
  * 4 Motor Controllers 
  * 1 3S 40C 2200mah Lipo Battery
  * 1 9v Battery
  * 1 Plexiglass Drone Frame
  * 1 Plexiglass Controller Frame

#### Steps
 1. Using The drawing provided cut out the drone frame, controller frame, 4 brackets for the motor, and 4 spacers.
 1. Wire up the controller and drone using the schematics provided.
 1. Upolad the code to the arduino. (there are 2 folders Manual and SelfLevel)
 1. Start flying.

### Installing
Download the source code.
```bash
git clone https://github.com/AbdelhalemKassara/Self_Leveling_Quadcopter.git
```

## Built With 
C++
