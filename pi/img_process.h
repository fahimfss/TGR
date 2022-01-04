#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <unistd.h>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>

const double PI = 3.14159265359;
const double RIGHT_MULTIPLYER = 1.3;
const double LEFT_MULTIPLYER = 1.3;

const int PIXEL_ACTIVATED_VALUE = 128;

const int DIR_RIGHT = 1;
const int DIR_LEFT = 2;

const int UNDEFINED = -10000000;

const int speed = 50;

const int CENTER_X_INDEX = 0;
const int CENTER_Y_INDEX = 1;
const int CENTER_NO_INDEX = 2;

const int LEFT_SPEED_INDEX = 0;
const int RIGHT_SPEED_INDEX = 1;
const int ANGLE_INDEX = 2;
const int DIR_INDEX = 3;

int first_center = -1;

int temp = 1;

struct point{
    int x;
    int y;
};

void print_pixel_val3D(const cv::Mat& img, int x, int y){
    std::cout<<"Pixel value at : " << x << ", " <<y << ": ("; 
    std::cout<< (int)img.at<cv::Vec3b>(x, y)[0] << ", " << (int)img.at<cv::Vec3b>(x, y)[1] << ", " << (int)img.at<cv::Vec3b>(x, y)[2] << ")\n"; 
}

// Takes an image as parameter and detects line (yellow) in the image
// Returns the centers (at most 20) of the path
std::vector<std::vector<int>> get_path_centers(const cv::Mat& img){
    int img_width = img.cols;
    int img_height = img.rows / 2;
    cv::Rect roi;
    roi.x = 0;
    roi.y = img_height;
    roi.width = img_width;
    roi.height = img_height;

    // Bottom half of the image will be used. Crop the image to that part only
    cv::Mat cimg = img(roi);
    cv::Mat hsvImg;
    // Convert BGR image to HSV 
    cv::cvtColor(cimg, hsvImg, cv::COLOR_BGR2HSV);
    
    cv::Mat1b mask;
    // Convert to BnW (single channel) image by detecting the yellow color line
    cv::inRange(hsvImg, cv::Scalar(15, 45, 70), cv::Scalar(40, 180, 230), mask);

    // Only for the first time, create a histogram of the image to initialize the center point for the boxes
    std::vector<int> hist(img_width, 0);
    if(first_center == -1){ 
        int max_val = 0;
        for(int i = 0; i < img_height; i++){
            for(int j = 0; j < img_width; j++){
                if(mask.at<uchar>(i, j) > PIXEL_ACTIVATED_VALUE){
                    hist[j]++;
                }
            }
        }
        for(int i = 0; i < hist.size(); i++){
            if(hist[i] > max_val){
                max_val = hist[i];
                first_center = i;
            }
        }
    }

    int curr_center = first_center; 

    int box_half_width = (img_width / 16);
    int box_width = box_half_width * 2;
    int box_height = img_height / 20;

    std::vector<point>points(20);

    // Using windows to detect the paths. 
    // Average of the activated pixel's x and y values will be the center of a window
    for(int i = img_height - 1, center_no = 1; i >= 0; i-= box_height, center_no++){
        int top = i - box_height + 1;
        int left = std::max(0, curr_center - box_half_width);
        int right = std::min(left + box_width, img_width);

        // increase the size of the first window
        if(i == img_height - 1){
            left = std::max(left - 40, 0); 
            right = std::min(right + 40, img_width);
        }

        int count = 0;
        int sumX = 0;
        int sumY = 0;

        for(int j = i; j >= top; j--){
            // check if we need to update right limit
            if(right < img_width){
                for(int k = right - 5; k < right; k++){
                    if(mask.at<uchar>(j, k) > PIXEL_ACTIVATED_VALUE){
                        right = std::min(right + (box_half_width/2), img_width);
                    }
                }
            }

            for(int k = left; k < right; k++){
                if(mask.at<uchar>(j, k) > PIXEL_ACTIVATED_VALUE){
                    // check if we need to update left limit
                    if(k - left < 5 && left > 0) {
                        left = std::max(left - (box_half_width / 2), 0);
                        k = left - 1;
                        continue;
                    }

                    count++;
                    sumX+= j;
                    sumY+= k;
                }
            }
        }

        if(count != 0){
            int avgX = sumX / count;
            int avgY = sumY / count;
            points[center_no - 1] = {avgX, avgY};
            curr_center = avgY;
        }
        else{
            points[center_no - 1] = {UNDEFINED, UNDEFINED};
        }
    }

    std::vector<std::vector<int>> centers;

    for(int i = 0; i < points.size(); i++){
        if(points[i].x != UNDEFINED){
            int x = points[i].x;
            int y = points[i].y;
            std::vector<int> v(3);
            v[CENTER_X_INDEX] = x ;
            v[CENTER_Y_INDEX] = y;
            v[CENTER_NO_INDEX] = i;
            centers.push_back(v);
        }
    }

    if(centers.size() > 0) first_center = centers[0][CENTER_Y_INDEX];

    return centers;
}

void set_color(cv::Vec3b &color, int val){
    color[0] = val;
    color[1] = val;
    color[2] = val;
}

void put_centers_in_image(cv::Mat& img, const std::vector<std::vector<int>>& centers, int height_offset, int width_offset){
    for(int i = 0; i < centers.size(); i++){
        int x = centers[i][CENTER_X_INDEX];
        int y = centers[i][CENTER_Y_INDEX];
        set_color(img.at<cv::Vec3b>(x + height_offset, y + width_offset), 255);
        set_color(img.at<cv::Vec3b>(x - 1 + height_offset, y + width_offset), 255);
        set_color(img.at<cv::Vec3b>(x + 1 + height_offset, y + width_offset), 255);
        set_color(img.at<cv::Vec3b>(x + height_offset, y - 1 + width_offset), 255);
        set_color(img.at<cv::Vec3b>(x + height_offset, y + 1 + width_offset), 255);
    }
}

double angle(point a, point o, point b) {
    double ang = (fabs(atan2(a.y - o.y, a.x - o.x) - atan2(b.y - o.y, b.x - o.x)) * 180) / PI;
    return ang; 
}

int in_100(int val){
    if(val < -100) return -100;
    if(val > 100) return 100;
    return val;
}

// Converts the angle of the centers to motor power
// This is not the most efficient way, but that's left for the RL part
std::vector<int> get_powers_from_ange(double angle, int reduce){
    int dir = 1;
    if(angle > 90){
        angle = 360 - angle;
        dir = 2;
    }

    std::vector<int> ret(4);
    int cur_speed = std::max(speed - reduce, 20);

    int speedLeft, speedRight;

    if (dir == DIR_RIGHT){
        speedLeft = in_100(cur_speed + (angle  * LEFT_MULTIPLYER));
        speedRight = in_100(cur_speed - (angle * RIGHT_MULTIPLYER));
    }
    else{
        speedLeft = in_100(cur_speed - (angle  * LEFT_MULTIPLYER));
        speedRight = in_100(cur_speed + (angle * RIGHT_MULTIPLYER));
    }

    ret[LEFT_SPEED_INDEX] = speedLeft;
    ret[RIGHT_SPEED_INDEX] = speedRight;
    ret[ANGLE_INDEX] = (int)angle;
    ret[DIR_INDEX] = dir;

    return ret;
}


// Using the centers, returns motor left power, motor right power, angle and direction
std::vector<int> parse_centers(std::vector<std::vector<int>>& centers, int img_width, int img_height){
    if(centers.size() == 0) {
        std::vector<int> ret(4);
        ret[LEFT_SPEED_INDEX] = UNDEFINED;
        ret[RIGHT_SPEED_INDEX] = UNDEFINED;
        ret[ANGLE_INDEX] = UNDEFINED;
        ret[DIR_INDEX] = UNDEFINED;
 
        return ret;
    }
    int pref_last_index = std::max(0, (int)centers.size() - 5);
    int cx = 0, cy = 0, cnt = 0;
    for(int i = pref_last_index; i < centers.size(); i++){
        cx += centers[i][CENTER_X_INDEX];
        cy += centers[i][CENTER_Y_INDEX];
        cnt++;
    }
    cx /= cnt;
    cy /= cnt;
    int img_center = img_width / 2;

    point p1 = {0, img_center};
    point p2 = {img_height - 1, img_center};
    point p3 = {cx, cy};
    point p4 = {centers[centers.size() - 1][CENTER_X_INDEX], centers[centers.size() - 1][CENTER_Y_INDEX]};

    double ang1 = angle(p1, p2, p3);
    int ang2 = angle(p1, p2, p4);
    if(ang2 > 90) ang2 = 360 - ang2;

    return get_powers_from_ange(ang1, ang2 - (ang2 / 2));
}