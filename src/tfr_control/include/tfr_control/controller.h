/**
 * controller.h
 * 
 * This class is in charge of handling the physical hardware interface with
 * the robot itself, and is started by the controller_launcher node.
 */
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <hardware_interface/joint_command_interface.h>
#include <hardware_interface/joint_state_interface.h>
#include <hardware_interface/robot_hw.h>
#include <algorithm>

namespace tfr_control {
    enum class Joint 
    {
        LEFT_TREAD, 
        RIGHT_TREAD, 
        BIN, //TODO integrate bin
        TURNTABLE, 
        LOWER_ARM,
        UPPER_ARM, 
        SCOOP 
    };

    /**
     * The class registered with controller_manager that handles the interface
     * with all hardware components on the robot. See controller_launcher.cpp
     * for the usage of this class.
     * */
    class Controller : public hardware_interface::RobotHW
    {
    public:
        
        static const int CONTROLLER_COUNT = 7;

        Controller(bool fakes, const double lower_lim[CONTROLLER_COUNT],
                const double upper_lim[CONTROLLER_COUNT]);
        void read();
        void write();
    private:
        hardware_interface::VelocityJointInterface tread_velocity_interface;
        hardware_interface::JointStateInterface joint_state_interface;
        hardware_interface::EffortJointInterface joint_effort_interface;

        // Populated by controller_manager, read in by the write() method
        double command_values[CONTROLLER_COUNT]{};
        // Populated by sensors, read in by controller_manager
        double position_values[CONTROLLER_COUNT]{};
        // Populated by sensors, read in by controller_manager
        double velocity_values[CONTROLLER_COUNT]{};
        // Populated by sensors, read in by controller_manager
        double effort_values[CONTROLLER_COUNT]{};

        /**
         * If false, this will use the actual hardware values.
         * If true, this will use fake values generated for use in rviz.
         *  - Said fake values will consist of applying the command to the
         *    position and velocity values directly as opposed to using sensor
         *    input.
         */
        bool use_fake_values = false;

        // The time of the previous update cycle (update at the end of write)
        ros::Time prev_time;
        
        void registerJoint(std::string name, Joint joint);

        /**
         * Gets the time in seconds since the last update cycle
         */
        double getUpdateTime();

        // Holds the lower and upper limits of the URDF model joints
        const double *lower_limits, *upper_limits;
    };
}

#endif // CONTROLLER_H
