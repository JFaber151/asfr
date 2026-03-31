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
        ],
        remappings=[
            ( "/output/motor_cmd" , "/input/motor_cmd")
        ]
    )
    
    cam = Node(
        package="cam2image_vm2ros",
        executable="cam2image",
        name="cam",
        parameters=[
            {"rotate": True},
        ]
    )

    relbot_simulator = Node(
        package="relbot_simulator",
        executable="relbot_simulator",
        name="relbot_sim",
        parameters=[
            {"throttle_rate": 500.0},
        ],

    )
    relbot_turtle = Node(
        package="turtlesim",
        executable="turtlesim_node",
        name="robotturtle"
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
    )
    
    return LaunchDescription([
        cam,
        adapter_node,
        relbot_simulator,
        relbot_turtle,
        relbot_setpoints,
        green_checker,
    ])