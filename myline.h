#ifndef MYLINE_H
#define MYLINE_H
#include <iostream>

using namespace std;

class MyLine
{
    public:

    int id_; //id
    int k_; //angle [0-360)
    int l_; //length

    MyLine(int id, int k, int l);
    bool operator<(const MyLine &A);
    void print();
    void setK(int k);
    void setL(int l);

};

#endif // MYLINE_H
