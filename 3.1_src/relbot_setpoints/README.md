Package relbot_setpoints
-----------------------------------------------
### Description: 
This package creates setpoints to be sent to a relbot adapter

### Functionality:
This package creates a node that will create the setpoint velocities for the relbot's motors.
The `relbot_setpoints` node will send arbitrary values.
The `relbot_setpoints_green` node will send velocities that depend on the coordinates given by the 
`where_green` topic. This node relies on the width parameter.

### Inputs:
if the node being run is `relbot_setpoints_green`
`/where_green` 
        Type: std_msgs::msg::String


### Outputs:
`/input/left_motor/setpoint_vel`  
        Type: example_interfaces::msg::Float64

`/input/right_motor/setpoint_vel`  
        Type: example_interfaces::msg::Float64        

### Parameters:
if the node being run is `relbot_setpoints_green`
- `width` : Sets the width of the camera, so we can calculate how far to the side the green object is relative to the camera width. By default, this is set to 320 pixels

### Running the node:
Default behaviour for arbitrary setpoints:
`
ros2 run relbot_setpoints relbot_setpoints
`
Behaviour for when there is access to coordinates from a camera:
```
ros2 run relbot_setpoints relbot_setpoints_green --ros-args -p width:=<your_cam_pixel_width>
```