from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument
from launch_ros.actions import Node
def generate_launch_description():
    adapter_node = Node(
        package="relbot_adapter",
        executable="relbot_adapter",
        name="relbot_adapter",
        parameters = [
            {"max_speed_mps" : 100.0},
            {"robotmode" : "real"}
        ],
        
    )
    
    cam = Node(
        package="cam2image_vm2ros",
        executable="cam2image",
        name="cam",
        parameters=[
            {"rotate": True},
        ]
    )


    green_checker = Node(
        package="green_checker",
        executable="green_checker_node",
        name="green_location",
    )

    relbot_setpoints = Node(
        package="relbot_setpoints",
        executable="relbot_setpoints_green",
        name="relbot_setpoints", 
        parameters = [
            {"base_velocity": 10},
            {"width": 640} 
        ]       
    )
    
    return LaunchDescription([
        cam,
        adapter_node,
        #relbot_turtle,
        relbot_setpoints,
        green_checker,
    ])