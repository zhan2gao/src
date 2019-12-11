#!/usr/bin/env python
import rospy
from nav_msgs.msg import Odometry
from gazebo_msgs.srv import GetModelState,GetModelStateRequest
from std_msgs.msg import Header
import geometry_msgs.msg
import tf2_ros
import tf_conversions
rospy.init_node("odom_pub")

odom_pub =  rospy.Publisher('/odom',Odometry)

rospy.wait_for_service('/gazebo/get_model_state')

get_model_srv = rospy.ServiceProxy('/gazebo/get_model_state',GetModelState)

odom = Odometry()
header = Header()
header.frame_id = '/odom'

model = GetModelStateRequest()
model.model_name = 'mecanum'

r = rospy.Rate(10)
def tf_broadcast(msg):
    br = tf2_ros.TransformBroadcaster()
    t = geometry_msgs.msg.TransformStamped()
    
    t.header.stamp = rospy.Time.now()
    t.header.frame_id = "odom"
    t.child_frame_id = "base_link"
    t.transform.translation.x = msg.pose.position.x
    t.transform.translation.y = msg.pose.position.y
    t.transform.translation.z = 0.0
    t.transform.rotation.x = msg.pose.orientation.x
    t.transform.rotation.y = msg.pose.orientation.y
    t.transform.rotation.z = msg.pose.orientation.z
    t.transform.rotation.w = msg.pose.orientation.w
    br.sendTransform(t)



while not rospy.is_shutdown():

    result = get_model_srv(model)
    tf_broadcast(result)
    odom.pose.pose = result.pose
    odom.twist.twist = result.twist
    header.stamp = rospy.Time.now()
    odom.header = header
    
    odom_pub.publish(odom) 
    r.sleep()