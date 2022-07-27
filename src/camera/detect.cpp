/*
#include <opencv2/opencv.hpp>
#include <iostream>


void imageSubtract(cv::Mat& image1, cv::Mat& image2)
{
    if ((image1.rows != image2.rows) || (image1.cols != image2.cols))
    {
        if (image1.rows > image2.rows)
        {
            resize(image1, image1, image2.size(), 0, 0, cv::INTER_LINEAR);
        }
        else if (image1.rows < image2.rows)
        {
            resize(image2, image2, image1.size(), 0, 0, cv::INTER_LINEAR);
        }
    }

    cv::Mat image1_gary, image2_gary;
    if (image1.channels() != 1)
    {
        cvtColor(image1, image1_gary, cv::COLOR_BGR2GRAY);
    }
    if (image2.channels() != 1)
    {
        cvtColor(image2, image2_gary, cv::COLOR_BGR2GRAY);
    }

    cv::Mat frameDifference, absFrameDifferece;
    cv::Mat previousGrayFrame = image2_gary.clone();
    //图1锟斤拷图2
    subtract(image1_gary, image2_gary, frameDifference, cv::Mat(), CV_16SC1);

    //取锟斤拷锟斤拷值
    absFrameDifferece = abs(frameDifference);

    //位锟斤拷母谋锟�
    absFrameDifferece.convertTo(absFrameDifferece, CV_8UC1, 1, 0);
    cv::imshow("absFrameDifferece", absFrameDifferece);
    cv::Mat segmentation;

    //锟斤拷值锟斤拷锟斤拷锟斤拷锟斤拷一锟斤拷锟杰关硷拷锟斤拷要锟斤拷锟矫讹拷值锟斤拷锟斤拷值锟斤拷
    cv::threshold(absFrameDifferece, segmentation, 100, 255, cv::THRESH_BINARY);

    //锟斤拷值锟剿诧拷
    cv::medianBlur(segmentation, segmentation, 3);

    //锟斤拷态学锟斤拷锟斤拷(锟斤拷锟斤拷锟斤拷锟斤拷)
    //锟斤拷态学锟斤拷锟斤拷锟矫碉拷锟斤拷锟斤拷锟斤拷
    cv::Mat morphologyKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));
    cv::morphologyEx(segmentation, segmentation, cv::MORPH_CLOSE, morphologyKernel, cv::Point(-1, -1), 2, cv::BORDER_REPLICATE);

    //锟斤拷示锟斤拷值锟斤拷图片
    cv::imshow("segmentation", segmentation);

    //锟揭边斤拷
    std::vector< std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    findContours(segmentation, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));//CV_RETR_TREE
    std::vector< std::vector<cv::Point> > contours_poly(contours.size());

    std::vector<cv::Rect> boundRect;
    boundRect.clear();

    for (int index = 0; index < contours.size(); index++)
    {
        cv::approxPolyDP(cv::Mat(contours[index]), contours_poly[index], 3, true);
        cv::Rect rect = boundingRect(cv::Mat(contours_poly[index]));
        cv::rectangle(image2, rect, cv::Scalar(0, 255, 0), 2);
    }
    cv::imshow("效锟斤拷图", image2);
    cv::waitKey(0);

}



int main(int, char**) {
    // 锟斤拷1锟斤拷图锟斤拷锟斤拷耄拷锟阶硷拷锟�
    // 锟斤拷取图锟斤拷
    cv::Mat im1 = cv::imread("C:\\Users\\acai\\Desktop\\test_opencv\\imgs\\00.jpg");
    cv::Mat im2 = cv::imread("C:\\Users\\acai\\Desktop\\test_opencv\\imgs\\11.jpg");
    //cv::Mat im3 = cv::imread("C:\\Users\\acai\\Desktop\\test_opencv\\imgs\\img_move_with_object.jpg");

    // 锟斤拷图锟斤拷转锟斤拷为锟揭度ｏ拷
    cv::Mat im1_gray, im2_gray;
    cv::cvtColor(im1, im1_gray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(im2, im2_gray, cv::COLOR_BGR2GRAY);
    // 锟斤拷锟斤拷锟剿讹拷模锟斤拷
    const int warp_mode = cv::MOTION_EUCLIDEAN;
    // MOTION_TRANSLATION  平锟斤拷
    // MOTION_EUCLIDEAN 欧锟斤拷锟斤拷锟�
    // MOTION_AFFINE 锟斤拷锟斤拷
    // MOTION_HOMOGRAPHY 锟斤拷应锟斤拷
    // 锟斤拷锟斤拷锟剿讹拷模锟斤拷锟斤拷锟斤拷2x3锟斤拷3x3锟斤拷锟轿撅拷锟斤拷
    cv::Mat warp_matrix;
    // 锟斤拷始锟斤拷锟斤拷锟斤拷锟皆斤拷锟叫憋拷识
    if (warp_mode == cv::MOTION_HOMOGRAPHY) {
        warp_matrix = cv::Mat::eye(3, 3, CV_32F);
    }
    else {
        warp_matrix = cv::Mat::eye(2, 3, CV_32F);
    }

    // 指锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
    int number_of_iterations = 50;

    // 指锟斤拷锟斤拷锟轿碉拷锟斤拷之锟斤拷锟斤拷锟较碉拷锟斤拷锟斤拷锟斤拷锟斤拷锟街�
    double termination_eps = 1e-10;

    // 锟斤拷锟斤拷锟斤拷止锟斤拷锟斤拷
    cv::TermCriteria criteria(cv::TermCriteria::COUNT +
        cv::TermCriteria::EPS,
        number_of_iterations,
        termination_eps);

    //锟斤拷锟斤拷ECC锟姐法锟斤拷锟斤拷锟斤拷娲拷锟絯arp_matrix锟叫★拷
    cv::findTransformECC(
        im1_gray,
        im2_gray,
        warp_matrix,
        warp_mode,
        criteria
    );
    // 锟斤拷疟锟斤拷蔚锟酵硷拷锟�
    cv::Mat im2_aligned;
    cv::warpAffine(im2, im2_aligned, warp_matrix, im1.size(), cv::INTER_LINEAR + cv::WARP_INVERSE_MAP);

    // 锟斤拷示锟斤拷锟秸斤拷锟�
    cv::imshow("Image 1", im1);
    cv::imshow("Image 2", im2);
    cv::imshow("Image 2 Aligned", im2_aligned);

    //锟斤拷2锟斤拷图锟斤拷平锟斤拷 锟斤拷锟斤拷斯锟剿诧拷锟斤拷
    int kernel_length = 3;
    cv::GaussianBlur(im2_aligned, im2_aligned, cv::Size(kernel_length, kernel_length), 0, 0);
    //cv::imshow("Image 2 Aligned GaussianBlur", im2_aligned);

    //锟斤拷3锟斤拷图锟斤拷去血锟斤拷
    cv::waitKey(0);
    int w = im1.cols;
    int h = im1.rows;
    float rate = 0.2;

    cv::Rect rect((int)(rate*w), (int)(rate * h), w - (int)(rate * w), h - (int)(rate * h));

    imageSubtract(im1(rect), im2_aligned(rect));

    return 0;

}
*/

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

#include <opencv2\opencv.hpp>

using namespace cv;

//int main()
//{
//    Mat src = imread(R"(D:/image/page01.png)", 1);  // replace image path

//    vector<vector<Point>> mask_area;
//    vector<Point> mask_points;
//    mask_points.push_back(Point(100, 200));
//    mask_points.push_back(Point(500, 300));
//    mask_points.push_back(Point(520, 460));
//    mask_points.push_back(Point(300, 420));
//    mask_points.push_back(Point(200, 330));
//    mask_points.push_back(Point(100, 250));
//    mask_area.push_back(mask_points);

//    vector<vector<Point>> mask_area2;
//    vector<Point> mask_points2;
//    mask_points2.push_back(Point(600, 200));
//    mask_points2.push_back(Point(1000, 300));
//    mask_points2.push_back(Point(1020, 460));
//    mask_points2.push_back(Point(800, 420));
//    mask_points2.push_back(Point(700, 330));
//    mask_points2.push_back(Point(600, 250));
//    mask_area2.push_back(mask_points2);

//    imshow("init", src);
//    polylines(src, mask_area, 1, Scalar(0, 0, 0));
//    polylines(src, mask_area2, 1, Scalar(0, 0, 0));
//    imshow("resource", src);
//    cv::Mat mask, dst;

//    src.copyTo(mask);
//    mask.setTo(cv::Scalar::all(0));
//    fillPoly(mask, mask_area, Scalar(255, 255, 255));
//    fillPoly(mask, mask_area2, Scalar(255, 255, 255));
//    imshow("mask", mask);
//    src.copyTo(dst, mask);
//    imshow("dst", dst);

//    waitKey();

//    return 0;
//}


//int main()
//{

//    namedWindow("Frame");
//    namedWindow("移动目标跟踪");
//    namedWindow("Mask");

//    pMOG2 = createBackgroundSubtractorMOG2(50, 200.0, true);

//    char *videoFilename = "0";
//    processVideo(videoFilename);

//    destroyAllWindows();
//    return 0;
//}

/*
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
*/

///*
void processVideo(char* videoFilename)
{
    VideoCapture capture(0);

    cout << "cam opened: " << capture.isOpened() << endl;

    cv::Mat tmp;
    cv::Mat frame_bg = cv::imread("D:/image/page021.png");
    cv::Mat frame_fg = cv::imread("D:/image/page032.png");

    vector<vector<Point>> mask_area;
    vector<Point> mask_points;
    mask_points.push_back(Point(100, 200));
    mask_points.push_back(Point(500, 300));
    mask_points.push_back(Point(520, 460));
    mask_points.push_back(Point(300, 420));
    mask_points.push_back(Point(200, 330));
    mask_points.push_back(Point(100, 250));
    mask_area.push_back(mask_points);

    vector<vector<Point>> mask_area2;
    vector<Point> mask_points2;
    mask_points2.push_back(Point(600, 200));
    mask_points2.push_back(Point(1000, 300));
    mask_points2.push_back(Point(1020, 460));
    mask_points2.push_back(Point(800, 420));
    mask_points2.push_back(Point(700, 330));
    mask_points2.push_back(Point(600, 250));
    mask_area2.push_back(mask_points2);

//    imshow("init", src);
    polylines(frame_bg, mask_area, 1, Scalar(0, 0, 0));
    polylines(frame_bg, mask_area2, 1, Scalar(0, 0, 0));
    rectangle(frame_bg, Point(900, 100), Point(1200, 500), Scalar(0, 0, 0));
    circle(frame_bg, Point(300, 200), 50, Scalar(0, 0, 0));
//    imshow("resource", frame_bg);
    cv::Mat mask, dst;

    frame_bg.copyTo(mask);
    mask.setTo(cv::Scalar::all(0));
    fillPoly(mask, mask_area, Scalar(255, 255, 255));
    fillPoly(mask, mask_area2, Scalar(255, 255, 255));
    rectangle(mask, Point(900, 100), Point(1000, 200), Scalar(255, 255, 255), -1, 8, 0);
    circle(mask, Point(300, 200), 50, Scalar(255, 255, 255), -1, 8, 0);
//    imshow("mask", mask);
    frame_bg.copyTo(dst, mask);
    imshow("frame bg dst", dst);
    frame_bg = dst.clone();


//    imshow("init", src);
    polylines(frame_fg, mask_area, 1, Scalar(0, 0, 0));
    polylines(frame_fg, mask_area2, 1, Scalar(0, 0, 0));
    rectangle(frame_bg, Point(900, 100), Point(1200, 500), Scalar(0, 0, 0));
    circle(frame_bg, Point(300, 200), 50, Scalar(0, 0, 0));
////    imshow("resource", frame_bg);
    cv::Mat mask2, dst2;

    frame_fg.copyTo(mask2);
    mask2.setTo(cv::Scalar::all(0));
    fillPoly(mask2, mask_area, Scalar(255, 255, 255));
    fillPoly(mask2, mask_area2, Scalar(255, 255, 255));
    rectangle(mask2, Point(900, 100), Point(1000, 200), Scalar(255, 255, 255), -1, 8, 0);
    circle(mask2, Point(300, 200), 50, Scalar(255, 255, 255), -1, 8, 0);
////    imshow("mask", mask);
    frame_fg.copyTo(dst2, mask2);
    imshow("frame fg dst", dst2);
    frame_fg = dst2.clone();


    keyboard = 0;
    int count = 0;
    string status = "";

    while (keyboard != 'q' && keyboard != 27)
    {
        if (count < 100)
        {
            frame = frame_bg.clone();
            status = "resetting, " + to_string(pMOG2->getHistory()) + " . ";
            pMOG2->apply(frame, fgMaskMOG2);
            count++;
        }
        else
        {
            frame = frame_fg.clone();
            status = "detecting ";
            pMOG2->apply(frame, fgMaskMOG2, 0);
        }

        stringstream ss;
        rectangle(frame, cv::Point(10, 2), cv::Point(100, 20),
            cv::Scalar(255, 255, 255), -1);
        ss << "0";
        string frameNumberString = status + ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
            FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
        imshow("Frame", frame);

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

}
//*/

void drawRect(Mat MOG2, Mat frame)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(MOG2, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
    Mat imageContours = Mat::zeros(MOG2.size(), CV_8UC1);
    Mat Contours = Mat::zeros(MOG2.size(), CV_8UC1);
    vector<vector<Point>> approxPoint(contours.size());
    for (int i = 0; i < int(contours.size()); i++)
    {

        if (contourArea(contours[i]) > MINAREA)
        {
            RotatedRect rectPoint = minAreaRect(contours[i]);
            // cout << contourArea(contours[i]) << endl;
            Point2f fourPoint2f[4];
            rectPoint.points(fourPoint2f);
//            int minX = 0;

//            for (int j = 0; j < 4; j++)
//            {
//                if (fourPoint2f[j].x > minX)
//                    minX = fourPoint2f[j].x;
//            }
//            if (minX > 0.7 * frame.cols)
//            {
//                continue;
//            }

//            int maxX = 0;
//            for (int j = 0; j < 4; j++)
//            {
//                if (fourPoint2f[j].x > maxX)
//                    maxX = fourPoint2f[j].x;
//            }
//            if (maxX < 0.3 * frame.cols)
//            {
//                continue;
//            }

            for (int i = 0; i < 3; i++)
            {
                line(frame, fourPoint2f[i], fourPoint2f[i + 1], Scalar(0, 0, 255), 1);
            }
            line(frame, fourPoint2f[0], fourPoint2f[3], Scalar(0, 0, 255), 1);

//            for (int i = 0; i < 4; i++) {
//                cout << fourPoint2f[i].x << " " << fourPoint2f[i].y << endl;
//            }
//            cout << endl;
        }
    }
    imshow("移动目标跟踪", frame);
    FRAME++;
    stringstream ss;
    ss << FRAME << b;
//    imwrite(ss.str(), frame);
}
