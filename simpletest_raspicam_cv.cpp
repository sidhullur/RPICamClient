#include <ctime>
#include <fstream>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std; 

#define IP "<IP_ADDRESS>"
#define PORT 8123

int main ( int argc,char **argv ) {
	int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
  
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, IP, &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if ((client_fd
         = connect(sock, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
	}
	raspicam::RaspiCam_Cv Camera;
	Camera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	Camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
	
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
	usleep(3000000);
	cv::Mat image;
	Camera.grab();
	int  imgSize = 640*480*3;
	while(true) {
		Camera.retrieve(image);
		send(sock, image.data, imgSize, 0);
		Camera.grab();
	}
	
	Camera.release();
	return 0;
}
