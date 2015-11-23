#include "ros/ros.h"
#include "std_msgs/String.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>


std::string incoming_reply;
void incomingReply(const std_msgs::String& data_reply);

int main(int argc, char **argv){

	
	std_msgs::String voice_cmd;
	const char * message_reply;
	
	ros::init(argc, argv, "tcp_bridge");
	ros::NodeHandle n;
	ros::Publisher pub_voice = n.advertise<std_msgs::String>("art_vrd/voice_data", 1024);
	ros::Subscriber sub_incoming_reply = n.subscribe("art_vrd/incoming_reply", 1024, incomingReply);
	
	// ################# Intializing Socket	#################  
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[512];
    int port_number = 4444;
     
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
        printf("Could not create socket");
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_number);
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
		perror("bind failed. Error");
        return 1;
    }   
    listen(socket_desc , 3);
    c = sizeof(struct sockaddr_in);
    // ################# Intializing Socket	#################  
		 
	while(ros::ok()){
		puts("Waiting for incoming connections...");
		
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0){
			
			perror("accept failed");
			return 1;
		}
		puts("Connection accepted");
		 

		while(ros::ok()){
			read_size = recv(client_sock , client_message , 512 , 0);
		
			client_message[read_size] = '\0';
			voice_cmd.data = client_message;
			pub_voice.publish(voice_cmd);			
			incoming_reply = "NULL";
			
			puts("Waiting reply");
			while(read_size != -1 && read_size != 0 && incoming_reply == "NULL" && ros::ok()){
				ros::spinOnce();			
			}
			puts("Reply accepted");
			
			message_reply = incoming_reply.c_str();
			send(client_sock , message_reply , strlen(message_reply) , 0);
			
			incoming_reply = "NULL";
			
			if(read_size == 0){
				puts("Client disconnected");
				break;
			}
			else if(read_size == -1){
				perror("recv failed");
				break;
			}
			
			ros::spinOnce();
		}
		close(client_sock);
        sleep(1);
	}
	close(socket_desc);
     
    return 0;
}

void incomingReply(const std_msgs::String& data_reply)
{
	incoming_reply = data_reply.data;
}