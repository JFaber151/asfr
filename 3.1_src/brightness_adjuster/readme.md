# Assignment 1.1.2/1.1.3

## Overview
Our node subscirbes to /image and calculates an average brightness. Based on a threshhold it is determined if the light is on or off. The on or off value os then published on topic /light_on as a bool.

## Inputs:
```bash
/image
Type: sensor_msgs::msg::Image
```
## Outputs:
```bash
/light_on
Type: std_msgs::msg::Bool
```
## Parameters:
```bash
threshold : Sets the brightness threshold used to determine whether the light is considered on or off.
```

## How to run
Start camera:
```bash
ros2 run image_tools cam2image --ros-args -p depth:=1 -p history:=keep_last
```

Run brightness node:
```bash
ros2 run brightness_adjuster brightness_node
```

## Verifying
With the following command one can varify the bool value:
```bash
ros2 topic info /light_on
````
> If False -> Light is off <br>
> If True -> Light is on
