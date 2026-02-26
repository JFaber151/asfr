# Brightness Adjuster Node

**Authors:** Joran Faber & Deborah Schrag <br>
**Course:** Advanced Software Development for Robotics  <br>
**University:** University of Twente <br>
**Year:** 2026 <br>
**Date:** 24.02.2026

## Overview
Our node subscirbes to /image and calculates an average brightness. Based on a threshhold it is determined if the light is on or off. The on or off value os then published on topic /light_on as a bool.


## Build instruction

```bash
colcon build
source install/setup.bash
```

## How to run

Start camera:
```bash
ros2 run image_tools cam2image --ros-args -p depth:=1 -p history:=keep_last
```

Run brightness node:

```bash
ros2 run brightness_adjuster brightness_adjuster
```

## Verifying

With the following command one can varify the bool value:
```bash
ros2 topic info /light_on
````
If False -> Light is off <br>
If True -> Light is on