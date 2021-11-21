# TGR
TGR is a Raspberry Pi (4B) based 4wd robot, created for real world ML/RL based projects. Currently the robot can be controlled using an Android App. I am currently planning and working on projects involving ML and RL using the robot.


![PXL_20211118_114955822](https://user-images.githubusercontent.com/8725869/142424417-8b1f653d-f736-4e0d-97b7-70e0500c1b19.jpg)


## TGR ShowOff!
TGR has tank like controlling: To move left, the right wheels are given power and to move right, the left wheels are given power. Here's TGR running around:  

![GIF1_Small](https://user-images.githubusercontent.com/8725869/142747312-289b3cfb-657c-4469-bd19-6f613eb6e0ab.gif)

The robot is controlled using an Android app. The top part of the following GIF contains the recorded screen of the Android app during controlling the robot. The bottom part is a realtime video of the robot.  

![GIF2_Small](https://user-images.githubusercontent.com/8725869/142747483-952f0df2-c9a4-43c7-842d-24a17b76e6d2.gif)

High res videos can be found [here](https://drive.google.com/file/d/16X5udqJc99JmQJwUT7lmcn4zDLDHTN4-/view?usp=sharing) and [here](https://drive.google.com/file/d/16Yq4cvY4MGfM3svSmrKjyQYgUQQEKkC-/view?usp=sharing) (Change the quality to 1080p).

## Hardware
In this section, the details about the components/materials/tools and the conections will be provided.  
Note: URL with an asterisk means that, the URL is of a local Bangladeshi website, who does not ship to other countries.  

### Components 
**Electrical components**
| Sl | Name | Number | URL |
| -- | -- | -- | -- |
| 1 | Raspberry PI 4B, 4gb  | 1 | [URL](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/) |
| 2 | Raspberry Pi Camera Module V2 | 1 | [URL](https://thepihut.com/products/raspberry-pi-camera-module) |
| 3 | Battery (Power Bank, 20000mAh) | 1 | [URL](https://www.baseus.co/baseus-45w-20000mah-mulight-digital-display-quick-charge-power-bank-20000mah) |
| 4 | Motor driver (TB6612FNG) | 2 | [URL](https://www.sparkfun.com/products/14451) |
| 5 | PWM Controller (PCA9685) | 1 | [URL](https://learn.adafruit.com/16-channel-pwm-servo-driver?view=all) |
| 6 | 12v 400rpm Geared Motor | 4 | [\*URL](https://bdspeedytech.com/index.php?route=product/product&path=16&product_id=561&limit=100) |
| 7 | 12v DC Fan | 1 | [\*URL](https://www.daraz.com.bd/products/i148372106-s1074332484.html) |
| 8 | 12v Led lights | 2 | [\*URL](https://udvabony.com/product/high-power-white-3w-12v-cob-led-chip-028-mod/) |
| 9 | QC2.0/3.0 decoy trigger board | 1 | [URL](https://www.aliexpress.com/item/1005001493092842.html) |
| 10 | USB 2.0 Jack A Type | 2 | [URL](https://www.aliexpress.com/item/1923995230.html) |
| 11 | Jumper wires | - | [URL](https://www.amazon.com/EDGELEC-Breadboard-Optional-Assorted-Multicolored/dp/B07GD2BWPY/) |
| 12 | Type-c cable | 1 | [URL](https://www.aliexpress.com/item/1005001874325159.html) |
  
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
It might not be obvious why I am using this particular power bank or the fan. The power bank has multiple outputs and is powerful enough to run the PI and the components. Also, it supports both 5v and 12v output. 5v is needed to power the PI, the motor drivers and the PWM controller. The 12v output can be achived using the usb QC2.0/3.0 decoy trigger. The motors, the lights and the fan require 12v power. The 12v fan serves two purposes. It cools the PI CPU and also draws power from the usb port so that the power bank does not turn off the usb port. Without it, the power bank will turn the usb port off and the motors/lights will not get any power. 

### Connections
Following diagram shows the connections between the PI, motor driver, PWM controller and the power bank. 12v power comes from the usb QC2.0/3.0 decoy trigger output. The Wiring PI pinouts are different than the Broadcom GPIO pinouts. [Here's](https://pi4j.com/1.4/images/pi4j-rpi-4b-pinout.png) a high res image of the WiringPI pinout.

![Pinout](https://user-images.githubusercontent.com/8725869/142750641-029285a5-23f4-439b-a7d0-1141ea3aa2bb.png)
(Click the image for better resolution)

## Software




