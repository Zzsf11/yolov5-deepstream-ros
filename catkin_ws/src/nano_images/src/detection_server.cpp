#include <ros/ros.h>
#include <nano_images/BoundingBox.h>
#include <nano_images/detection_server.h>

int string_to_int(string s)
{
 stringstream ss;
 ss << s;
 int i;
 ss >> i;
 return i;
}

int outof0(string str)
{
 int i = 0;
 while (str[i] == '0')
 {
     i += 1;
 }
 str.erase(0, i);
 int num = string_to_int(str);
 return num;
}



int main(int argc, char **argv)
{
	ros::init(argc, argv, "detection_server");
	ros::NodeHandle n;
	ros::Publisher Boundingbox_info_pub  = n.advertise<nano_images::BoundingBox>("/BoundingBox_info",10);
	ros::Rate loop_rate(10);
	
	int count =0;


	
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == listenfd) {
		printf("create socket error");
		return -1;
	}


	// bind port 
	struct sockaddr_in bindaddr;
	bindaddr.sin_family = AF_INET;
	bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bindaddr.sin_port = htons(3000);
	if(-1 == bind(listenfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr))) {
		printf("bind error");
		return -1;
	}

	// start listen
	if (listen(listenfd, 2) == -1) {
		printf("listem error");
		return -1;
	}



 	while(ros::ok){

		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);

		// accept connection
		int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
		if (clientfd != -1) {         	
			char recvBuf[511] = {};
			
			// receive data
			int ret = recv(clientfd, recvBuf, sizeof(recvBuf), 0);
			string result(recvBuf);
			int number_detection = outof0(result.substr(2, 4));

			if (ret > 0) {
				// printf("recv data from client, data: %s\n", recvBuf);
	
     			vector<int> possible_indexes;
     			vector<float> possible_locations;

				nano_images::BoundingBox BoundingBox_msg;

				if (number_detection)
					{
						string flag = "BB";
						int position = 0;
						int i = 1;
						while ((position = result.find(flag, position)) != string::npos)
						{
							possible_indexes.push_back(position);
							position++;
						}
						//cout << possible_indexes.size() << endl;
						for (int i = 0; i < possible_indexes.size(); i++)
						{
							possible_locations.push_back((float)(outof0(result.substr(possible_indexes[i] + 2, 4))));
							//根据返回的矩形框计算质心位置
							int X0 = (int)(outof0(result.substr(possible_indexes[i] + 6, 4)) + outof0(result.substr(possible_indexes[i] + 14, 4)) / 2);
							int Y0 = (int)(outof0(result.substr(possible_indexes[i] + 10, 4)) + outof0(result.substr(possible_indexes[i] + 18, 4)) / 2);
							//int X0 = 1;  //test
							//int Y0 = 2;
							BoundingBox_msg.id = i;
							BoundingBox_msg.x_center = X0;
							BoundingBox_msg.y_center = Y0;
							Boundingbox_info_pub.publish(BoundingBox_msg);
						}

						
	
					}


				//send data
				ret = send(clientfd, "get it", strlen("get it"), 0);
				if (ret != strlen("get it")) {
					printf("send data error.");
				}
				
			} else {
				printf("recv data error.");
			}
			



		//ROS_INFO("keyibahaoxiang");
		close(clientfd);
		memset(recvBuf, 0, 255);
		loop_rate.sleep();
		

		}


	}
	close(listenfd);
	return 0;
		

}



