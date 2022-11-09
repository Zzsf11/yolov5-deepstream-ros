#include <ros/ros.h>

int main(int argc, char **argv)
{
	ros::init(argc, argv, "yolov5_model");
	ros::NodeHandle n;
    system("deepstream-app -c /home/nvidia/Desktop/DeepStream-Yolo/deepstream_app_config.txt");

    while(1);
    return 0;
}