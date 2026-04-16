# Assignment 1.1.4

## Overview
Our node subscribes to /image and checks for green pixels in the image.  
If green is detected, the center of mass (CoM) of all green pixels is calculated.  
The position of the green area is then published on topic /where_green as a string.

A pixel is considered green when the green value is higher than both red and blue by our manually set tolerance value of 50.

## Inputs:
```bash
/image
Type: sensor_msgs::msg::Image
```

## Outputs:
```bash
/where_green
Type: std_msgs::msg::String

```
## Parameters:
```bash
tolerance : Sets the tolerance used to classify a pixel as green (green must be higher than red and blue by this amount). By default, this is set to 50. 
```

## How to run
Start camera first:
```bash
ros2 run image_tools cam2image --ros-args -p depth:=1 -p history:=keep_last
```

Run green checker node:
```bash
ros2 run green_checker green_checker
```

## Verifying

With the following command one can verify the position of the detected green:
```bash
ros2 topic echo /where_green
```
The output will be:  
"The green is at: X,Y"
