#include "clock.h"
#include<vector>
Clock::Clock(string path_img)
{
    img_=imread(path_img);

    //pre-processing
    Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, 0, 4, 0, 0, -1, 0);
    filter2D(img_, tem_, CV_8UC3, kernel);
    blur(tem_,tem_,Size(4,4));

    //post-processing
    threshold(tem_,tem_, 245, 255, CV_THRESH_BINARY);//segmentation
    Canny(tem_, tem_, 10, 140, 3);//canny detection

    Mat kernel2 = getStructuringElement(MORPH_RECT, Size(5, 1));
   // morphologyEx(tem_, tem_, CV_MOP_CLOSE, kernel2);

    cvtColor(tem_, res_, CV_GRAY2BGR);//to Color
    hour_=0;
    min_=0;
    sec_=45;

}

int Clock::dcmp(double x)
{
    if(fabs(x)<eps)
        return 0;
    else
        return x<0 ? -1:1;
}

double Clock::Dot(Point A,Point B)
{
    return A.x*B.x+A.y*B.y;
}

double Clock::Length(Point A)
{
    return sqrt(Dot(A,A));
}

double Clock::Cross(Point A,Point B)
{
    return A.x*B.y-A.y*B.x;
}

double Clock::Angle(Point A,Point B)
{
    return acos(Dot(A,B)/Length(A)/Length(B));
}

double Clock::DistanceToLine(Point P,Point A,Point B)
{
    Point v1=B-A,v2=P-A;
    return fabs(Cross(v1,v2))/Length(v1);
}

double Clock::DistancetoSegment(Point P,Point A,Point B)
{
        if(A==B)return Length(P-A);
        Point v1=B-A,v2=P-A,v3=P-B;
        if(dcmp(Dot(v1,v2))<0)return  Length(v2);
        else if(dcmp(Dot(v1,v3))>0)return Length(v3);
        else return fabs(Cross(v1,v2))/Length(v1);
}

void Clock::dispay()
{
    imshow("img",img_);
    imshow("tem",tem_);
    imshow("res",res_);
}

void Clock::traitement()
{

    // Circle
    std::vector<Vec3f> circles;
    HoughCircles(tem_,circles,CV_HOUGH_GRADIENT,2,50,200,100,90,130);
    // Find the circles by HoughCircles

    cout<<circles.size()<<endl;

    int pos=0;
    int max=-1;
    for(size_t i = 0; i < circles.size(); i++ )
    {
        Vec3f f=circles[i];
        if(f[2]>max && f[0]+f[2]<tem_.rows && f[0]-f[2]>=0 && f[1]+f[2]<tem_.cols && f[1]-f[2]>0)
        {
            max=f[2];
            pos=i;
        }
    }
    Point center(circles[pos][0],circles[pos][1]);//center
    int radius= circles[pos][2];//redius
    circle(res_,center,radius,Scalar(255),2);

    // Lines
    vector<MyLine> list_MyLine;
    vector<Vec4i> lines2;
    HoughLinesP(tem_, lines2, 1.12, CV_PI/180, 41, 41, 7 );
    cout<<lines2.size()<<endl;

    for( size_t i = 0; i < lines2.size(); i++ )
    {

        Vec4i l = lines2[i];
        Point A(l[0], l[1]),B(l[2], l[3]);
        if(DistancetoSegment(center,A,B)<30)//conserve the lines whose distance to center is smaller than 30
        {
            bool down=(A.y+B.y-2*center.y>0);
            if(A.x==B.x){// when slop is infinity
                list_MyLine.push_back(MyLine(i,90+(down?180:0),Length(Point(A.x-B.x,A.y-B.y))));
            }else if(A.y==B.y){// when slop is 0
                list_MyLine.push_back(MyLine(i,A.x+B.x-2*center.x>0 ? 0:180,Length(Point(A.x-B.x,A.y-B.y))));
            }else{
                if(down){
                    if(A.x+B.x<2*center.x) //180-270
                        list_MyLine.push_back(MyLine(i,360-atan2(A.y-B.y,A.x-B.x)*180/PI,Length(Point(A.x-B.x,A.y-B.y))));
                    else //270-360
                        list_MyLine.push_back(MyLine(i,360-atan2(B.y-A.y,B.x-A.x)*180/PI,Length(Point(A.x-B.x,A.y-B.y))));
                }else{
                    if(A.x+B.x<2*center.x) //90-180
                        list_MyLine.push_back(MyLine(i,abs(atan2(A.y-B.y,A.x-B.x)*180/PI),Length(Point(A.x-B.x,A.y-B.y))));
                    else //0-90
                        list_MyLine.push_back(MyLine(i,abs(atan2(B.y-A.y,B.x-A.x)*180/PI),Length(Point(A.x-B.x,A.y-B.y))));
                }
            }
            line(res_,A,B, Scalar(0,150,i*40+40), 2, CV_AA);

        }

    }

    list<MyLine> list_MyLine1;
    for(int j=0;j<list_MyLine.size();j++)
    {

        list_MyLine[j].print();
        if(list_MyLine[j].l_<86)
        {
            if(list_MyLine[j].k_>360)
                list_MyLine[j].setK(list_MyLine[j].k_-180);
            if((list_MyLine[j].k_<=185&&list_MyLine[j].k_>=176))
                list_MyLine[j].setL(list_MyLine[j].l_+50);
            list_MyLine1.push_back(list_MyLine[j]);
         }
    }



    // Calculation
    int now_k,pre_k=720; //recent line's angle and last line's angle
    int num=0; //number of lines (2 or 3)
    int Du[5]={0};//angles of 3 lines
    int Le[5]={0};//Le[i]=Le_ping[i]*0.2+le_max[i]*0.8;
    int Le_ping[5]={0};//lengths of 3 lines (mean)
    int Le_max[5]={0};//lengths of 3 lines(max)
    int t_num=0;//

    MyLine now_Line(0,0,0);
    list_MyLine1.push_back(MyLine(99,888,0));
    list_MyLine1.sort();
    while(!list_MyLine1.empty())
    {
        now_Line=list_MyLine1.front();
        now_k=now_Line.k_;
        if(abs(now_k-pre_k)>8)//if these 2 lines' angle is bigger than 8°
        {
            if(num!=0){//mean of this group
                Du[num-1]/=t_num;
                Le_ping[num-1]/=t_num;
                Le[num-1]=Le_ping[num-1]*0.2+Le_max[num-1]*0.8;
            }
            if(now_k==888)break;//margin
            t_num=0;
            num++;
            cout<<"---------------------------\n";
        }
        t_num++;
        Du[num-1]+=now_Line.k_;
        Le_ping[num-1]+=now_Line.l_;
        if(now_Line.l_>Le_max[num-1])Le_max[num-1]=now_Line.l_;
        now_Line.print();
        list_MyLine1.pop_front();
        pre_k=now_k;
    }
    cout<<"---------------------------\n\n";

    cout<<"---------------------------\n";
    int t;
    for(int i=0;i<num-1;i++){
        for(int j=i+1;j<num;j++){
            if(Le[i]>Le[j]){
                t=Le[i],Le[i]=Le[j],Le[j]=t;
                t=Du[i],Du[i]=Du[j],Du[j]=t;
            }
        }
    }
    char s[3][10]={"hour  :","minute:","second:"};
    for(int i=0;i<num;i++)
    {
        cout<<"si: "<<s[i]<<endl;
        cout<<"angle i: "<<Du[i]<<endl;
        cout<<"length i: "<<Le[i]<<endl;
    }
    cout<<"---------------------------\n";
    hour_=(360-Du[0]+90)%360/30;
    min_=(360-Du[1]+90)%360/6;
    if(num==2)
        cout<<"time is: "<<hour_<<" : "<<min_<<endl;
    else if(num==3)
    {
        sec_=(360-Du[2]+90)%360/6;
        cout<<"time is: "<<hour_<<" : "<<min_<<" : "<<sec_<<endl;
    }


}

int Clock::getHour()
{
    return hour_;
}
int Clock::getMin()
{
    return min_;
}
int Clock::getSec()
{
    return sec_;
}
