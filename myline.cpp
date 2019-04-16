#include "myline.h"

MyLine::MyLine(int id, int k, int l)
{
    id_=id;
    k_=k;
    l_=l;
}

bool MyLine::operator<(const MyLine &A)
{
    return k_<A.k_;
}
void MyLine::setK(int k)
{
    k_=k;
}

void MyLine::setL(int l)
{
    l_=l;
}

void MyLine::print()
{
    cout<<"id: "<<id_<<endl;
    cout<<"angle: "<<k_<<endl;
    cout<<"length: "<<l_<<endl;
}

