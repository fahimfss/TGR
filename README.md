# TGR
TGR is a Raspberry Pi (4B) based 4wd robot created for real-world ML/RL-based projects. The robot can perform the line-following task using the camera image only, and the robot can be controlled using an Android App. I plan to implement projects involving ML and RL using the robot.**TGR form factor was updated on Jan 2022 to make it smaller, to include fisheye camera lens and to include new sets of lights.**

![COLLAGE](https://user-images.githubusercontent.com/8725869/147936054-ff628e37-ce7e-4e92-9f9a-834dbcb18979.jpg)

## Update Jan 2022: Image Based Line Following Using TGR
Here's TGR following a Yellow line:  
![TGR_Running_GIF](https://user-images.githubusercontent.com/8725869/148042514-fae0964a-9581-43a3-bc6f-f756eaffdb04.gif)

The original video can be found [here](https://drive.google.com/file/d/13p-KOvLZCS4jOOw-yA4AFciswgG9Aqvu/view?usp=sharing).

Here are the steps for the line following task:
![image](https://user-images.githubusercontent.com/8725869/148066894-95cebe3a-0192-4185-98ee-c20dfce1badf.png)
![image](https://user-images.githubusercontent.com/8725869/148066963-b79f445a-8506-4301-b9c8-dd101a2652df.png)
![image](https://user-images.githubusercontent.com/8725869/148067042-cb9e9255-16cd-4416-974c-1f40d1029837.png)


The [auto_runner.cpp](https://github.com/fahimfss/TGR/blob/main/pi/auto_runner.cpp) file contains the code for the line following task, which is run in the Raspberry PI. The code uses the following 2 header files: [img_process.h](https://github.com/fahimfss/TGR/blob/main/pi/img_process.h), [threadsafe_stack.h](https://github.com/fahimfss/TGR/blob/main/pi/threadsafe_stack.h). The image capture is set to 40 FPS, which takes about 25ms per image. Image processing takes about .75ms.   
The following command is used in the terminal for compiling auto_runner.cpp:
```
g++ -std=c++17 auto_runner.cpp `pkg-config opencv4 --cflags --libs`  -pthread -lwiringPi -o auto
```
The command for running auto_runner:
```
./auto
```

## TGR ShowOff!
TGR has tank-like controlling: To move left, the right motors are given power, and to move right, the left motors are given power. Here's TGR running around:  

![GIF1_Small](https://user-images.githubusercontent.com/8725869/142747312-289b3cfb-657c-4469-bd19-6f613eb6e0ab.gif)

The robot is controlled using an Android app. The top part of the following GIF contains the screen recording of the Android app while controlling the robot. 
The bottom part is a real-time video of the robot.  

![GIF2_Small](https://user-images.githubusercontent.com/8725869/142747483-952f0df2-c9a4-43c7-842d-24a17b76e6d2.gif)

High res videos can be found [here](https://drive.google.com/file/d/16X5udqJc99JmQJwUT7lmcn4zDLDHTN4-/view?usp=sharing) and [here](https://drive.google.com/file/d/16Yq4cvY4MGfM3svSmrKjyQYgUQQEKkC-/view?usp=sharing) (Change the quality to 1080p). For this demonstration, the previous form factor of TGR was used. 

## Hardware
In this section, details about the components/materials/tools and the connections will be provided.  
Note: URL with an asterisk means that the URL is of a local Bangladeshi website, who does not ship to other countries.  

### Components 
**Electrical components**
| Sl | Name | Number | URL |
| -- | -- | -- | -- |
| 1 | Raspberry PI 4B, 4gb  | 1 | [URL](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/) |
| 2 | Raspberry Pi Camera Module V2 | 1 | [URL](https://thepihut.com/products/raspberry-pi-camera-module) |
| 3 | Battery (Power Bank, 20000mAh) | 1 | [URL](https://www.baseus.co/baseus-45w-20000mah-mulight-digital-display-quick-charge-power-bank-20000mah) |
| 4 | Motor Driver (TB6612FNG) | 2 | [URL](https://www.sparkfun.com/products/14451) |
| 5 | PWM Controller (PCA9685) | 1 | [URL](https://learn.adafruit.com/16-channel-pwm-servo-driver?view=all) |
| 6 | 12v 400 RPM Geared Motor | 4 | [\*URL](https://bdspeedytech.com/index.php?route=product/product&path=16&product_id=561&limit=100) |
| 7 | 12v DC Fan | 1 | [\*URL](https://www.daraz.com.bd/products/i148372106-s1074332484.html) |
| 8 | 12v Led Light | 2 | [URL](https://www.aliexpress.com/item/32910685603.html) |
| 9 | QC2.0/3.0 Decoy Trigger Board | 1 | [URL](https://www.aliexpress.com/item/1005001493092842.html) |
| 10 | USB 2.0 Jack Connector A type | 2 | [URL](https://www.aliexpress.com/item/1923995230.html) |
| 11 | Jumper Wires | - | [URL](https://www.amazon.com/EDGELEC-Breadboard-Optional-Assorted-Multicolored/dp/B07GD2BWPY/) |
| 12 | Type-c Cable | 1 | [URL](https://www.aliexpress.com/item/1005001874325159.html) |
  
**Build Materials**   
| Sl | Name | Number |
| -- | -- | -- | 
| 1 | Depron foam board (2ft by 2ft, 5mm, Gray) | 1 |
| 2 | Screws ([5-20mm M2](https://www.aliexpress.com/item/1005001870799002.html) and [6-10mm M3](https://www.aliexpress.com/item/1005002208039147.html))  | - | 
| 3 | Super Glue | 1 | 
| 4 | [\*Wheels (85x38mm)](https://www.techshopbd.com/detail/2213/Cars_Tire_85x38mm_Wheel_techshop_bangladesh) and [\*Hexa connector (4mm)](https://www.techshopbd.com/detail/2573/Wheel_Hexa_Connector_4mm_techshop_bangladesh) | 4 | 
| 5 | Transparent acrylic sheet (1ft by 1ft, 3mm) | 1 | 

**Tools**  
| Sl | Name | 
| -- | -- |
| 1 | Anti cutter | 
| 2 | Steel scale |
| 3 | Electric screw driver (Tolsen) |
| 4 | Soldering iron and solder wire |
| 5 | Wire cutter |

**Comments on Hardware**  
It might not be obvious why I am using this particular power bank or the fan. The power bank has multiple outputs and is powerful enough to run the PI and the components. Also, it supports both 5v and 12v output. 5v is needed to power the PI, the motor drivers and the PWM controller. The 12v output can be achieved using the USB QC2.0/3.0 decoy trigger. The motors, the lights and the fan require 12v power. The 12v fan serves two purposes. It cools the PI CPU and also draws power from the USB port so that the power bank does not turn off the USB port. Without it, the power bank will turn the USB port off and the motors/lights will not get any power. 

### Connections
The following diagrams show the connections between the PI, the motor drivers, the PWM controller, the power bank, the motors and the lights. 12v power comes from the USB QC2.0/3.0 decoy trigger output. The Wiring PI pinouts are different than the Broadcom GPIO pinouts. [Here's](https://pi4j.com/1.4/images/pi4j-rpi-4b-pinout.png) a high res image of the WiringPI pinout.

![diagram1](https://user-images.githubusercontent.com/8725869/142764230-aeffeeb9-1adb-4083-b671-852753f1729a.png)

![diagram2](https://user-images.githubusercontent.com/8725869/142793479-09346576-127f-426c-a30b-de6d81cd5d00.png)

(Click the images for better resolution)

## Software
### Raspberry PI
The PI mainly runs a C++ program which is used for: 
* Receiving commands from the Android App for driving the motors
* Receiving commands for turning the lights on or off
* Sending the video data generated by the PI camera to the Android App
  
The [mobile_runner.cpp](https://github.com/fahimfss/TGR/blob/main/pi/mobile_runner.cpp) contains the code for these tasks. The C++ code uses the following libraries:
1. OpenCV for Raspberry PI ([I followed this article to install OpenCV, turned off GStreamer](https://qengineering.eu/install-opencv-4.5-on-raspberry-pi-4.html))
2. WiringPI ([Installation URL](http://wiringpi.com/wiringpi-updated-to-2-52-for-the-raspberry-pi-4b/))
3. PWM Controller (pca9685) Driver ([Installation URL](https://github.com/Reinbert/pca9685)) 

Note: The WiringPI is [deprecated](http://wiringpi.com/wiringpi-deprecated/). But, I will not update my project yet because the PWM Controller Driver still uses WiringPI. 

The following command is used in the terminal for compiling mobile_runner.cpp:
```
g++ mobile_runner.cpp -o mobile_runner `pkg-config opencv4 --cflags --libs`  -pthread -lwiringPi 
```
The command for running mobile_runner:
```
./mobile_runner
```

### Android App
The Android App is used for motor, light control and to see the video stream from the robot. The [TGR_Android](https://github.com/fahimfss/TGR/tree/main/TGR_Android) contains the Android App codes. 

The app contains two [JoystickViews](https://github.com/zerokol/JoystickView), one for controlling the left motors, other for the right motors. Changing the position of a JoystickView upwards will turn the corresponding motors in the forward direction, and changing the position downwards will turn the motors in the backward direction. The JoystickViews also control the motors' speed. Full upward position of the JoystickView will result in full forward power, half upward position will result in half forward power. It also contains a button, which can be used for turning the lights on/off. In the middle of the app, the PI camera's video stream can be seen. The video stream can be used to control the robot in real-time.   

![AppScreenShot](https://user-images.githubusercontent.com/8725869/142752881-af2b07d4-7ffe-42f7-9842-be7ad6ea8b90.jpg)

The Android App is connected with the mobile_runner program through sockets for sending and receiving data. To make that happen, the two devices are connected to the same wifi network. The Android App acts as server; the mobile_runner.cpp contains the ip address (line 23) of the Android device for connection. The ip address of the Android device was fixed in the wifi router settings. 

## Future Plans
* PI camera has only around [62.2 x 48.8 degrees](https://elinux.org/Rpi_Camera_Module#Technical_Parameters_.28v.2_board.29) FoV. I am considering a camera with a higher FoV.
* The robot's movement mechanisms are set in such a way so that it can be easily used in RL projects. I am currently working on implementing RL algorithms on the robot for performing simple tasks. 
* The overall connection system between the Android device and the Raspberry PI can be further improved.  
