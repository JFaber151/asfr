#include "Template20Sim.hpp"

Template20Sim::Template20Sim(uint write_decimator_freq, uint monitor_freq) :
    XenoFrt20Sim(write_decimator_freq, monitor_freq, file, &data_to_be_logged),
    file(1,"./xrf2_logging/TEMPLATE","bin"), // change template to your project name
    controller()
{
     printf("%s: Constructing rampio\n", __FUNCTION__);
    
    // To infinite run the controller, uncomment line below
    controller.SetFinishTime(0.0);
}

Template20Sim::~Template20Sim()
{
    
}

int Template20Sim::initialising()
{
    // Set physical and cyber system up for use in a 
    // Return 1 to go to initialised state

    evl_printf("Hello from initialising\n");      // Do something

    // The logger has to be initialised at only once
    logger.initialise();
    // The FPGA has to be initialised at least once
    ico_io.init();

    return 1;
}

int Template20Sim::initialised()
{
    // Keep the physical syste in a state to be used in the run state
    // Call start() or return 1 to go to run state

    evl_printf("Hello from initialised\n");       // Do something
    
    return 1;
}

int Template20Sim::run()
{
    // Start logger
    logger.start();                             
    monitor.printf("Hello from run\n");  

    //ros message to xenomai
    u[0] = ros_msg.left_wheel_vel;
    u[1] = ros_msg.right_wheel_vel;

    //We pass the values directly here, since we dont have to implement the controler in 3.2 yet
    y[0] = u[0];
    y[1] = u[1];

    //xenomai message to ros we dont really do anything with it yet
    xeno_msg.left_wheel_pwm = y[0];
    xeno_msg.right_wheel_pwm = y[1];

    //calculate pwm from input we have and send to FPGA
    //with input positive values it will drive forwards
    actuate_data.pwm1 = 2047 * -(y[0]/100);
    actuate_data.pwm2 = 2047 * (y[1]/100);

    monitor.printf("U: %f, %f\n", u[0], u[1]);
    monitor.printf("Y: %f, %f\n", y[0], y[1]);

    if(controller.IsFinished())
        return 1;

    return 0;
}

int Template20Sim::stopping()
{
    // Bring the physical system to a stop and set it in a state that the system can be deactivated
    // Return 1 to go to stopped state
    logger.stop();                                // Stop logger
    evl_printf("Hello from stopping\n");          // Do something

    return 1;
}

int Template20Sim::stopped()
{
    // A steady state in which the system can be deactivated whitout harming the physical system

    monitor.printf("Hello from stopping\n");          // Do something

    return 0;
}

int Template20Sim::pausing()
{
    // Bring the physical system to a stop as fast as possible without causing harm to the physical system

    evl_printf("Hello from pausing\n");           // Do something
    return 1 ;
}

int Template20Sim::paused()
{
    // Keep the physical system in the current physical state

    monitor.printf("Hello from paused\n");            // Do something
    return 0;
}

int Template20Sim::error()
{
    // Error detected in the system 
    // Can go to error if the previous state returns 1 from every other state function but initialising 

    monitor.printf("Hello from error\n");             // Do something

    return 0;
}
