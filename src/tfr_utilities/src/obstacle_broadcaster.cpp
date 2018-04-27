#include <obstacle_broadcaster.h>

/*
 * Initializes the broadcaster and data structures
 * */
ObstacleBroadcaster::ObstacleBroadcaster(
        ros::NodeHandle& n, 
        const std::string &point_frame, 
        const std::string &parent_frame, 
        const std::string &service,
        const double &d,
        const double &h) : 

    node{n}, 
    broadcaster_frame{point_frame},
    map_frame{parent_frame}, 
    service_name{service},
    diameter{d},
    height{h},
    point_set{false}
{
    server = node.advertiseService(service_name,
            &ObstacleBroadcaster::localizePoint, this);
    transform.header.frame_id = map_frame;
    transform.child_frame_id = broadcaster_frame;
    transform.transform.rotation.w = 1;
    /* NOTE we work with the pcl::PointCloud class and broadcast it as a
     * sensor_msgs::PointCloud2, this is intentional, and in fact the way it is
     * designed to be used in ROS
     * */
    cloud_publisher = node.advertise<sensor_msgs::PointCloud2>("obstacle_cloud", 5);
}

/*
 * Broadcasts point across the transform network
 * If the point has been set it will also broadcast the obstacle
 * */
void ObstacleBroadcaster::broadcast()
{
    transform.header.stamp = ros::Time::now();
    broadcaster.sendTransform(transform);
    if (point_set)
    {
        pcl::PointCloud<pcl::PointXYZ> cloud;
        generateCloud(cloud);
        cloud_publisher.publish(cloud);
    }
}

/*
 * Gives the point a new origin
 * */
bool ObstacleBroadcaster::localizePoint(tfr_msgs::PoseSrv::Request &request,
        tfr_msgs::PoseSrv::Response &resonse)
{
    transform.transform.translation.x = request.pose.pose.position.x;
    transform.transform.translation.y = request.pose.pose.position.y;
    transform.transform.translation.z = height;
    transform.transform.rotation.x = request.pose.pose.orientation.x;
    transform.transform.rotation.y = request.pose.pose.orientation.y;
    transform.transform.rotation.z = request.pose.pose.orientation.z;
    transform.transform.rotation.w = request.pose.pose.orientation.w;
    point_set = true;
    transform.header.stamp = ros::Time::now();
    broadcaster.sendTransform(transform);
    ros::Duration(0.1).sleep();
    return true;
}

/*
 *  Generates a circular pointcloud levitating above the ground around the hole,
 *  used to make sure the robot, never ever falls in.
 * */
void ObstacleBroadcaster::generateCloud(pcl::PointCloud<pcl::PointXYZ> &cloud)
{
    double PI = 3.14159;
    cloud.header.frame_id = broadcaster_frame;
    cloud.width    = 16;
    cloud.height   = 1;
    cloud.is_dense = true;
    double radius = diameter/2, offset = 0.0;
    for (int i = 0;  i < 16; i++, offset += PI/8) 
        cloud.points.push_back(pcl::PointXYZ( radius*cos(offset),
                    radius*sin(offset), 0));

}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "point_broadcaster");
    ros::NodeHandle n;

    //get parameters
    std::string point_frame{}, parent_frame{}, service_name{};
    double hz{}, diameter{}, height{};

    ros::param::param<std::string>("~parent_frame", parent_frame, "");
    ros::param::param<std::string>("~point_frame", point_frame, "");
    ros::param::param<std::string>("~service_name", service_name, "");
    ros::param::param<double>("~hz", hz, 5.0 );
    ros::param::param<double>("~diameter", diameter, 0.0 );
    ros::param::param<double>("~height", height, -.16 );

    ObstacleBroadcaster broadcaster{n, point_frame, parent_frame, service_name,
        diameter, height};

    //wait to be called, then broadcast
    ros::Rate rate(hz);
    while(ros::ok())
    {
        broadcaster.broadcast();
        ros::spinOnce();
        rate.sleep();
    }
}
