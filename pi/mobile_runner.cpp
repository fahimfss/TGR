#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string> 
#include <thread>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <vector>
#include <arpa/inet.h>

#include "pca9685.c"
#include "pca9685.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

using namespace std; 
using namespace cv;

const char *IP = "192.168.0.175";    // Mobile
// const char *IP = "192.168.0.221";    // Laptop
const int MOTOR_LEFT = 1;
const int MOTOR_RIGHT = 2;
const int LIGHT = 3;
const int DIRECTION_FORWARD = 1;
const int DIRECTION_BACKWARD = 2;

// 1640x1232
const int VID_HEIGHT = 480;
const int VID_WIDTH = 640;

const int PORT_DATA = 8999;
const int PORT_CAM = 9001; 


#define PIN_BASE 300
#define MAX_PWM 4096
#define PWM_UNIT 41
#define HERTZ 1000

const int MOTOR_AIN1 = 24;
const int MOTOR_AIN2 = 25;
const int MOTOR_STBY = 23;
const int MOTOR_BIN1 = 22;
const int MOTOR_BIN2 = 21;

const int MOTOR_PWMA = 0;
const int MOTOR_PWMB = 1;

const int LIGHT_AIN1 = 2;
const int LIGHT_STBY = 0;

const int LIGHT_PWMA = 2;

const int LIGHT_ON = 1;
const int LIGHT_OFF = 2;

int setup(){
    wiringPiSetup(); 

	int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
	if (fd < 0){
		printf("Error in setup\n");
		return fd;
	}
    pca9685PWMReset(fd);
    printf("Setup completed.\n");

    pinMode(MOTOR_AIN1, OUTPUT);
    pinMode(MOTOR_AIN2, OUTPUT);
    pinMode(MOTOR_BIN1, OUTPUT);
    pinMode(MOTOR_BIN2, OUTPUT);
    pinMode(MOTOR_STBY, OUTPUT);
    
    pinMode(LIGHT_AIN1, OUTPUT);
    pinMode(LIGHT_STBY, OUTPUT);

    return 1;
}

void resetPins(){
    pwmWrite(PIN_BASE + 16, 0);
    digitalWrite(MOTOR_AIN1, LOW);
    digitalWrite(MOTOR_AIN2, LOW);
    digitalWrite(MOTOR_BIN1, LOW);
    digitalWrite(MOTOR_BIN2, LOW);
    digitalWrite(MOTOR_STBY, LOW);

    digitalWrite(LIGHT_AIN1, LOW);
    digitalWrite(LIGHT_STBY, LOW);
}

void move(int motor, int direction, int power){
    if(motor == MOTOR_LEFT){
        if(direction == DIRECTION_FORWARD){
            digitalWrite(MOTOR_AIN1, HIGH);        
            digitalWrite(MOTOR_AIN2, LOW);
            digitalWrite(MOTOR_STBY, HIGH);

            int pwm = PWM_UNIT * power;
            if(pwm>MAX_PWM) pwm = MAX_PWM;

            pwmWrite(PIN_BASE + MOTOR_PWMA, pwm);
        }

        else if(direction == DIRECTION_BACKWARD){
            digitalWrite(MOTOR_AIN1, LOW);
            digitalWrite(MOTOR_AIN2, HIGH);

            digitalWrite(MOTOR_STBY, HIGH);

            int pwm = PWM_UNIT * power;
            if(pwm>MAX_PWM) pwm = MAX_PWM;

            pwmWrite(PIN_BASE + MOTOR_PWMA, pwm);
        }
    }
    else if(motor == MOTOR_RIGHT){
        if(direction == DIRECTION_FORWARD){     
            digitalWrite(MOTOR_BIN1, HIGH);
            digitalWrite(MOTOR_BIN2, LOW);

            digitalWrite(MOTOR_STBY, HIGH);

            int pwm = PWM_UNIT * power;
            if(pwm>MAX_PWM) pwm = MAX_PWM;

            pwmWrite(PIN_BASE + MOTOR_PWMB, pwm);
        }

        else if(direction == DIRECTION_BACKWARD){   
            digitalWrite(MOTOR_BIN1, LOW);
            digitalWrite(MOTOR_BIN2, HIGH);

            digitalWrite(MOTOR_STBY, HIGH);

            int pwm = PWM_UNIT * power;
            if(pwm>MAX_PWM) pwm = MAX_PWM;

            pwmWrite(PIN_BASE + MOTOR_PWMB, pwm);
        }
    }
}

int open_socket(int port){
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout<<"Port: "<<port<<", Socket creation error."<<endl;
        sleep(2);
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, IP, &serv_addr.sin_addr)<=0) {
        cout<<"Port: "<<port<<", Invalid address/ Address not supported."<<endl; 
        sleep(2);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        cout<<"Port: "<<port<<", Connection Failed."<<endl; 
        sleep(2);
        return -1;
    }

    cout<<"Port: "<<port<<" connected."<<endl;

    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    return sock;
}

void cam_control(int port){
    while(true){
        int sock = open_socket(port);
        if(sock == -1) continue;
        int valread;

        Mat input, image;
        VideoCapture cap(0);

        // videoCapture.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('M', 'J', 'P', 'G') );
        cap.set(CAP_PROP_FRAME_HEIGHT, VID_HEIGHT);
        cap.set(CAP_PROP_FRAME_WIDTH, VID_WIDTH);
        cap.set(CAP_PROP_AUTO_EXPOSURE, 0.25);
        cap.set(CAP_PROP_EXPOSURE, 80);

        if(!cap.isOpened()) {
            cout << "Failed to open video capture" << endl;
            return;
        }

        usleep(1*1000*1000);
        
        while(true){
            // cap >> input;
            // resize(input, image, Size(VID_WIDTH/2, VID_HEIGHT/2));

            cap>>image;

            vector<uchar> buf;
            vector<int> compression;
            compression.push_back(IMWRITE_JPEG_QUALITY);
            compression.push_back(50);
            // compression.push_back(CV_IMWRITE_PNG_COMPRESSION);
            // compression.push_back(9);
            imencode(".JPG", image, buf, compression);
            uint32_t converted_size = htonl(buf.size());
            int ret1 = send(sock, &converted_size, sizeof(converted_size), 0);
            int ret2 = send(sock, buf.data(), buf.size(), 0);

            if(ret1 <= 0 || ret2 <= 0){
                cout<<"Port: "<<port<<" disconnectd."<<endl;
                break;
            }
        }
    }
}

void motor_light_control(int port){
    while(true){
        int sock = open_socket(port);
        if(sock == -1) continue;
        char buffer[1024] = {0};
        int valread;

        while(true){
            int start = 0;
            int remain = 3;

            int disconnected = 0;

            while(remain > 0){
                valread = recv(sock, buffer + start, 1024 - start, 0);
                if(valread <= 0){
                    disconnected = 1;
                    cout<<"Port: "<<port<<" disconnectd."<<endl;
                    break;
                }

                start  += valread;
                remain -= valread; 
            }
            // for(int i = 0; i < 3; i++){
            //     cout<<(int)buffer[i]<<" ";
            // } 
            // cout<<endl;

            if(disconnected == 1) break;

            int code = (int) buffer[0];
            if(code == MOTOR_LEFT){ 
                int direction = (int)buffer[1];
                int power = (int)buffer[2];

                move(MOTOR_LEFT, direction, power);
            }
            else if (code == MOTOR_RIGHT){
                int direction = (int)buffer[1];
                int power = (int)buffer[2];

                move(MOTOR_RIGHT, direction, power);

            }
            else if (code == LIGHT){
                int status = (int)buffer[1];
                if(status == LIGHT_ON){
                    digitalWrite(LIGHT_AIN1, HIGH);
                    digitalWrite(LIGHT_STBY, HIGH);
                    pwmWrite(PIN_BASE + LIGHT_PWMA, 1024);
                }
                else if(status == LIGHT_OFF){
                    digitalWrite(LIGHT_AIN1, LOW);
                    digitalWrite(LIGHT_STBY, LOW);
                    pwmWrite(PIN_BASE + LIGHT_PWMA, 0);
                }
            }
        }
    }
}


int main(int argc, char const *argv[]){
    setup();
    resetPins();
    std::thread motor_light_thread(motor_light_control, PORT_DATA);
    std::thread cam_thread(cam_control, PORT_CAM); 
 
    cam_thread.join();
    motor_light_thread.join();
    return 0;
}
