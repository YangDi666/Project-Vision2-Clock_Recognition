#ifndef CLOCK_H
#define CLOCK_H
#include <QCoreApplication>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <list>
#include <string>
#include "myline.h"

#define eps 0.0000000001
#define PI acos(-1.0)

using namespace std;
using namespace cv;

class Clock
{
    Mat img_;
    Mat tem_;
    Mat res_;
    int hour_;
    int min_;
    int sec_;

public:
    Clock(string path_img);
    int getHour();
    int getMin();
    int getSec();

    int dcmp(double x);
    double Dot(Point A,Point B); //Doc production of vector
    double Length(Point A); //Vector length
    double Cross(Point A,Point B); //Cross production of vector
    double Angle(Point A,Point B); //Angle enter 2 vectors
    double DistanceToLine(Point P,Point A,Point B); //Distance from a point to a line
    double DistancetoSegment(Point P,Point A,Point B);

    void dispay();
    void traitement();
};

#endif // CLOCK_H
