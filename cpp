#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>

void scanCallback(const sensor_msgs::LaserScan::ConstPtr &scan)
{
    // Extract the range and angle of all obstacles in front of the robot
    std::vector<float> ranges;
    std::vector<float> angles;
    for (unsigned int i = 0; i < scan->ranges.size(); ++i)
    {
        float angle = scan->angle_min + i * scan->angle_increment;
        // Scan angle in radians
        if (angle >= -M_PI / 36 && angle <= M_PI / 36) // -5 derajat
        {
            float range = scan->ranges[i];
            if (range > scan->range_min && range < scan->range_max)
            {
                ranges.push_back(range);
                angles.push_back(angle);
            }
        }
    }

    // Publish the obstacle distances and angles on the /obstacle_distances topic
    static ros::NodeHandle nh;
    static ros::Publisher pub = nh.advertise<sensor_msgs::PointCloud>("/obstacle_distances", 10);
    sensor_msgs::PointCloud msg;
    msg.header = scan->header;
    for (unsigned int i = 0; i < ranges.size(); ++i)
    {
        geometry_msgs::Point32 point;
        point.x = ranges[i] * cos(angles[i]);
        point.y = ranges[i] * sin(angles[i]);
        point.z = 0;
        msg.points.push_back(point);
    }
    pub.publish(msg);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "lidar_obstacle_detection_all");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe<sensor_msgs::LaserScan>("/scan", 10, scanCallback);
    ros::spin();
    return 0;
}