#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <string> 
#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "pca9685.c"
#include "pca9685.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <chrono>

#include "threadsafe_stack.h"
#include "img_process.h"

using namespace std; 
using namespace cv;

const int NUM_FRAMES = 500;

const int VID_HEIGHT = 240;
const int VID_WIDTH = 320; 
const bool SHOW_INFO = true;
const bool SAVE_IMG = false;
const string DEEFAULT_IMG_SAVE_PATH = "images/run3/";

const int MOTOR_LEFT = 1;
const int MOTOR_RIGHT = 2;
const int LIGHT = 3;
const int DIRECTION_FORWARD = 1;
const int DIRECTION_BACKWARD = 2;

const int MOTOR_AIN1 = 24;
const int MOTOR_AIN2 = 25;
const int MOTOR_STBY = 23;
const int MOTOR_BIN1 = 22;
const int MOTOR_BIN2 = 21;

const int MOTOR_PWMA = 0;
const int MOTOR_PWMB = 1;

#define PIN_BASE 300
#define MAX_PWM 4096
#define PWM_UNIT 41
#define HERTZ 1000

tsafe_stack<Mat> mat_stack;

int frame = 1;
bool stop = false;

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

    return 1;
}

void resetPins(){
    pwmWrite(PIN_BASE + 16, 0);
    digitalWrite(MOTOR_AIN1, LOW);
    digitalWrite(MOTOR_AIN2, LOW);
    digitalWrite(MOTOR_BIN1, LOW);
    digitalWrite(MOTOR_BIN2, LOW);
    digitalWrite(MOTOR_STBY, LOW);
}

void read_cam(){
    Mat image;
    VideoCapture cap(0);

    // videoCapture.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('M', 'J', 'P', 'G') );
    cap.set(CAP_PROP_FRAME_HEIGHT, VID_HEIGHT);
    cap.set(CAP_PROP_FRAME_WIDTH, VID_WIDTH);
    cap.set(CAP_PROP_AUTO_EXPOSURE, 0.25);
    cap.set(CAP_PROP_EXPOSURE, 80); 
    cap.set(CAP_PROP_FORMAT, CV_8UC1);
    cap.set(CAP_PROP_FPS, 40);

    if(!cap.isOpened()) {
        cout << "Failed to open video capture" << endl;
        return;
    }
    usleep(3*1000*1000);

    int nFPS = cap.get(CAP_PROP_FPS);
    cout << "Loaded FPS : " << nFPS << "\n";
    
    while(!stop){
        // cap >> input;
        // resize(input, image, Size(VID_WIDTH/2, VID_HEIGHT/2));
        cap>>image;
        mat_stack.push(image);

        if(frame >= NUM_FRAMES) break;
    }
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

void process_img_action(){
    setup();
    resetPins();

    Mat img;
    int av_count = 0;
    double avg = 0;
    while(true){
        mat_stack.wait_and_pop_all(img);
        auto t_start = std::chrono::high_resolution_clock::now();
        vector<vector<int>> centers = get_path_centers(img);
        vector<int> power_angle_data = parse_centers(centers, img.cols, img.rows / 2);
        auto t_end = std::chrono::high_resolution_clock::now();
        double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
        
        // cout<<count<<", L: " <<pwr[0] << "   R: " << pwr[1] << "    Angle: " << pwr[2] << "    Dir: " << pwr[3] << endl;
        if(power_angle_data[LEFT_SPEED_INDEX] == UNDEFINED || power_angle_data[RIGHT_SPEED_INDEX] == UNDEFINED){
            resetPins();
            stop = true;

            if(SAVE_IMG){
                string no = to_string(frame + 10000);
                string path = DEEFAULT_IMG_SAVE_PATH + no + ".png";
                put_centers_in_image(img, centers, VID_HEIGHT / 2,0);
                imwrite(path, img);
            }

            if(SHOW_INFO){
                cout << frame << "  ";
                cout << "L: " << power_angle_data[LEFT_SPEED_INDEX] << "    R: "  << power_angle_data[RIGHT_SPEED_INDEX] 
                << "    Ang: "  << power_angle_data[ANGLE_INDEX] << "    Dir: " << power_angle_data[DIR_INDEX] 
                << "    Num Centers: " << centers.size() <<endl;
            }
            break;
        }
        int ldir = DIRECTION_FORWARD, rdir = DIRECTION_FORWARD;
        int lpwr = power_angle_data[LEFT_SPEED_INDEX];
        int rpwr = power_angle_data[RIGHT_SPEED_INDEX];
        if(lpwr < 0) {
            ldir = DIRECTION_BACKWARD;
            lpwr *= -1;
        }
        if(rpwr < 0) {
            rdir = DIRECTION_BACKWARD;
            rpwr *= -1;
        }

        move(MOTOR_LEFT, ldir, lpwr);
        move(MOTOR_RIGHT, rdir, rpwr);

        if(SHOW_INFO){
            cout << frame << ",  Elapsed time ms: "<<elapsed_time_ms<< "    ";
            cout << "L: " << power_angle_data[LEFT_SPEED_INDEX] << "    R: "  << power_angle_data[RIGHT_SPEED_INDEX] 
            << "    Ang: "  << power_angle_data[ANGLE_INDEX] << "    Dir: " << power_angle_data[DIR_INDEX] 
            << "    Num Centers: " << centers.size() <<endl;
        }
        
        if(SAVE_IMG){
            string no = to_string(frame + 10000);
            string path = DEEFAULT_IMG_SAVE_PATH + no + ".png";
            put_centers_in_image(img, centers, VID_HEIGHT / 2, 0);
            imwrite(path, img);
        }

        if(SHOW_INFO && frame % 20 == 0) {
            cout << "Avg elapsed time ms: " << (avg / av_count) <<endl;
            av_count = 0;
            avg = 0;
        }
        if(frame > 1){
            avg += elapsed_time_ms;
            av_count++;
        }
        frame++;
        if(frame >= NUM_FRAMES){
            resetPins();
            break;
        } 
    }
}

int main(){
    thread cam_thread(read_cam);
    thread process_thread(process_img_action); 
 
    cam_thread.join();
    process_thread.join();
}