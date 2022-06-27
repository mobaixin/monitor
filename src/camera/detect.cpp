#include <opencv2\opencv.hpp>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;

int MINAREA = 100;
Mat frame;
Mat fgMaskMOG2;
Mat maskContour;
Ptr<BackgroundSubtractorMOG2> pMOG2;
char keyboard;
int FRAME = 0;
string b = ".jpg";

void processVideo(char *videoFilename);
void drawRect(Mat MOG2, Mat frame);

//int main()
//{

//    namedWindow("Frame");
//    namedWindow("None");
//    namedWindow("Mask");

//    pMOG2 = createBackgroundSubtractorMOG2(50, 100.0, true);

//    char *videoFilename = "0";
//    processVideo(videoFilename);

//    destroyAllWindows();
//    return 0;
//}

void processVideo(char *videoFilename)
{
    VideoCapture capture(0);

    cout << "cam opened: " << capture.isOpened() << endl;
    keyboard = 0;
    int count = 0;
    string status = "";

    while (keyboard != 'q' && keyboard != 27)
    {
        if (!capture.read(frame))
        {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        if (count < 100)
        {
            status = "resetting, " + to_string(pMOG2->getHistory()) + " . ";
            pMOG2->apply(frame, fgMaskMOG2);
            count++;
        }
        else
        {
            status = "detecting ";
            pMOG2->apply(frame, fgMaskMOG2, 0);
        }

        stringstream ss;
        rectangle(frame, cv::Point(10, 2), cv::Point(100, 20),
                  cv::Scalar(255, 255, 255), -1);
        ss << capture.get(CAP_PROP_POS_FRAMES);
        string frameNumberString = status + ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
        imshow("Frame", frame);

        threshold(fgMaskMOG2, fgMaskMOG2, 10, 255, cv::THRESH_BINARY);
        imshow("Mask", fgMaskMOG2);

        drawRect(fgMaskMOG2, frame);
        keyboard = (char)waitKey(30);
        if (keyboard == ' ')
        {
            cout << "reset background" << endl;
            count = 0;
            pMOG2->clear();
        }
    }
    capture.release();
}

void drawRect(Mat MOG2, Mat frame)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(MOG2, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
    Mat imageContours = Mat::zeros(MOG2.size(), CV_8UC1);
    Mat Contours = Mat::zeros(MOG2.size(), CV_8UC1);
    vector<vector<Point>> approxPoint(contours.size());
    for (int i = 0; i < contours.size(); i++)
    {

        if (contourArea(contours[i]) > MINAREA)
        {
            RotatedRect rectPoint = minAreaRect(contours[i]);
            // cout << contourArea(contours[i]) << endl;
            Point2f fourPoint2f[4];
            rectPoint.points(fourPoint2f);
            int minX = 0;

            for (int j = 0; j < 4; j++)
            {
                if (fourPoint2f[j].x > minX)
                    minX = fourPoint2f[j].x;
            }
            if (minX > 0.7 * frame.cols)
            {
                continue;
            }

            int maxX = 0;
            for (int j = 0; j < 4; j++)
            {
                if (fourPoint2f[j].x > maxX)
                    maxX = fourPoint2f[j].x;
            }
            if (maxX < 0.3 * frame.cols)
            {
                continue;
            }

            for (int i = 0; i < 3; i++)
            {
                line(frame, fourPoint2f[i], fourPoint2f[i + 1], Scalar(0, 0, 255), 1);
            }
            line(frame, fourPoint2f[0], fourPoint2f[3], Scalar(0, 0, 255), 1);
        }
    }
    imshow("None", frame);
    FRAME++;
    stringstream ss;
    ss << FRAME << b;
}
