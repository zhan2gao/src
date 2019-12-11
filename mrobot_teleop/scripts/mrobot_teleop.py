#!/usr/bin/env python
# -*- coding: utf-8 -*-
import rospy
from geometry_msgs.msg import Twist
import sys, select, termios, tty

msg = """
Control mrobot!
---------------------------
Moving around:
   q    w    e
   a    s    d

"""


def getKey():
    tty.setraw(sys.stdin.fileno())
    rlist, _, _ = select.select([sys.stdin], [], [], 0.1)
    if rlist:
        key = sys.stdin.read(1)
    else:
        key = ''
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    return key


if __name__=="__main__":
    settings = termios.tcgetattr(sys.stdin)
    
    rospy.init_node('mrobot_teleop')
    pub = rospy.Publisher('/cmd_vel', Twist, queue_size=5)

    try:
        print msg

        while(1):
            key = getKey()
            # 运动控制方向键（1：正方向，-1负方向）
            twist = Twist()
        # move forward

            if key == '':
                twist.linear.x = 0; 
                twist.linear.y = 0;
                twist.linear.z = 0
                twist.angular.x = 0; 
                twist.angular.y = 0; 
                twist.angular.z = 0
                pub.publish(twist)


            if key == 'w':
                twist.linear.x = 0.6; 
                twist.linear.y = 0;
                twist.linear.z = 0
                twist.angular.x = 0; 
                twist.angular.y = 0; 
                twist.angular.z = 0
                pub.publish(twist)
            # move back
            if key == 's':
                twist.linear.x = -0.3; 
                twist.linear.y = 0;
                twist.linear.z = 0
                twist.angular.x = 0; 
                twist.angular.y = 0; 
                twist.angular.z = 0
                pub.publish(twist)

            # move left
            if key == 'a':
                twist.linear.x = 0; 
                twist.linear.y = 0.3;
                twist.linear.z = 0
                twist.angular.x = 0; 
                twist.angular.y = 0; 
                twist.angular.z = 0
                pub.publish(twist)
        # move right
            if key == 'd':
                twist.linear.x = 0; 
                twist.linear.y = -0.3;
                twist.linear.z = 0
                twist.angular.x = 0; 
                twist.angular.y = 0; 
                twist.angular.z = 0
                pub.publish(twist)    

        #  left spin
            if key == 'q':
                twist.linear.x = 0; 
                twist.linear.y = 0;
                twist.linear.z = 0
                twist.angular.x = 0; 
                twist.angular.y = 0; 
                twist.angular.z = 1.2
                pub.publish(twist)    
        # spin
            if key == 'e':
                twist.linear.x = 0; 
                twist.linear.y = 0;
                twist.linear.z = 0
                twist.angular.x = 0; 
                twist.angular.y = 0; 
                twist.angular.z = -1.2
                pub.publish(twist)  

    except:
        pass

    finally:
        twist = Twist()
        twist.linear.x = 0; twist.linear.y = 0; twist.linear.z = 0
        twist.angular.x = 0; twist.angular.y = 0; twist.angular.z = 0
        pub.publish(twist)

    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
