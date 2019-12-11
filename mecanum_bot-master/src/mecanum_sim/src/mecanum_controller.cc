#include <boost/bind.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>
#include <stdio.h>
#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <geometry_msgs/Twist.h>
#include <iostream>

// These need to be pulled out to parameters...
const float WHEEL_RAD = 0.1016; // meters
const float WHEELBASE = 0.466725; // meters
const float TRACK = 0.2667; // meters

namespace gazebo
{
  class Mecanum : public ModelPlugin
  {
    public: void Load(physics::ModelPtr _parent, sdf::ElementPtr /*_sdf*/)
    {
      // Store the pointer to the model
      this->model = _parent;

      mbl = 0;
      mbr = 0;
      mfl = 0;
      mfr = 0;

      mRosnode.reset(new ros::NodeHandle(""));
      velocity_pub = mRosnode->advertise<geometry_msgs::Twist>("velocity", 1000);
      mfl_sub = mRosnode->subscribe("front_left",10,&Mecanum::fl_cb, this);
      mfr_sub = mRosnode->subscribe("front_right",10,&Mecanum::fr_cb, this);
      mbl_sub = mRosnode->subscribe("back_left",10,&Mecanum::bl_cb, this);
      mbr_sub = mRosnode->subscribe("back_right",10,&Mecanum::br_cb, this);
      cmd_vel = mRosnode->subscribe("cmd_vel",10,&Mecanum::set_vel, this);


      // Listen to the update event. This event is broadcast every
      // simulation iteration.
      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
        boost::bind(&Mecanum::UpdateChild, this, _1));
      }

      public: void UpdateChild(const common::UpdateInfo & /*_info*/)
      {
        float r = WHEEL_RAD;
        float l1 = TRACK; // left -> right
        float l2 = WHEELBASE; // front -> back
        float l = 1.0/(2*(l1+l2));
        float x = r*(mfl/4.0 + mfr/4.0 + mbl/4.0 + mbr/4.0);
        float y = r*(-mfl/4.0 + mfr/4.0 + mbl/4.0 - mbr/4.0);
        
        float rot = r*(-l*mfl + l*mfr - l*mbl + l*mbr);
        math::Pose pose = this->model->GetWorldPose();
        float yaw = pose.rot.GetYaw();
        geometry_msgs::Twist msg;
        /* msg.linear.x = x * cosf(yaw) - y * sinf(yaw);
        msg.linear.y = y * cosf(yaw) + x * sinf(yaw);
        msg.linear.z = 0;
        msg.angular.x = 0;
        msg.angular.y = 0;
        msg.angular.z = rot; */
        msg.linear.x = linear_vel.x*cosf(yaw) - linear_vel.y*sinf(yaw);
        msg.linear.y = linear_vel.y*cosf(yaw) + linear_vel.x*sinf(yaw);
        msg.linear.z = 0;
        msg.angular.x = 0;
        msg.angular.y = 0;
        msg.angular.z = angular_vel.z;
        velocity_pub.publish(msg);
        
        /* this->model->SetLinearVel(math::Vector3(
          x * cosf(yaw) - y * sinf(yaw),
          y * cosf(yaw) + x * sinf(yaw),
          0));
        this->model->SetAngularVel(math::Vector3(0, 0, rot)); */
        math::Vector3 bot_linear_vel;
        bot_linear_vel.x = linear_vel.x*cosf(yaw) - linear_vel.y*sinf(yaw);
        bot_linear_vel.y = linear_vel.y*cosf(yaw) + linear_vel.x*sinf(yaw);
        bot_linear_vel.z = 0;
        math::Vector3 bot_angular_vel;
        bot_angular_vel.z = angular_vel.z;
        bot_angular_vel.x = 0;
        bot_angular_vel.y = 0;

        this->model->SetLinearVel(bot_linear_vel);
        this->model->SetAngularVel(bot_angular_vel);

        
        }

        public: void fl_cb(const std_msgs::Float32::ConstPtr & cmd_msg)
        {
          mfl = cmd_msg->data;
        }
        public: void fr_cb(const std_msgs::Float32::ConstPtr & cmd_msg)
        {
          mfr = cmd_msg->data;
        }
        public: void bl_cb(const std_msgs::Float32::ConstPtr & cmd_msg)
        {
          mbl = cmd_msg->data;
        }
        public: void br_cb(const std_msgs::Float32::ConstPtr & cmd_msg)
        {
          mbr = cmd_msg->data;
        }
        public: void set_vel(const geometry_msgs::Twist::ConstPtr & twist_msg)
        {
          linear_vel = math::Vector3(twist_msg->linear.x,twist_msg->linear.y,twist_msg->linear.z);
          angular_vel = math::Vector3(twist_msg->angular.x,twist_msg->angular.y,twist_msg->angular.z);

        }

      private:
        // Pointer to the model
        physics::ModelPtr model;

        // Pointer to the update event connection
        event::ConnectionPtr updateConnection;

        boost::shared_ptr<ros::NodeHandle> mRosnode;

        float mfl;
        float mfr;
        float mbl;
        float mbr;
        math::Vector3 linear_vel;
        math::Vector3 angular_vel;
        ros::Publisher velocity_pub;
        ros::Subscriber mfl_sub;
        ros::Subscriber mfr_sub;
        ros::Subscriber mbl_sub;
        ros::Subscriber mbr_sub;
        ros::Subscriber cmd_vel;

      };

      // Register this plugin with the simulator
      GZ_REGISTER_MODEL_PLUGIN(Mecanum)
    }
