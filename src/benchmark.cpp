#include "matrix.hpp"
#include "bits/stdc++.h"

using namespace std;

int Round(float t)
{
    int k = t;
    float p = t - k;
    return (k >= 0.5) + k;
}

int main()
{
//    int *s = new int[]{0, 1, 2, 3, 4,
//                       5, 6, 7, 8, 9,
//                       10, 11, 12, 13, 14,
//                       15, 16, 17, 18, 19,
//                       20, 21, 22, 23, 24};
//    matrix<int> p(5, 5, s);
//    cout << p.at(3, 3) << endl;
//    matrix<int> sub(p, 2, 3, 4, 5);
//    p(4, 5) = 6;
//    matrix<int> q = p.clone();
//    cout << sub.at(3, 3) << endl;
//    cout << q(2, 4, 1) << endl;
//    cout << p.id << " " << sub.id << " " << q.id << endl;
//    sub.copy_to(q);

    float *mulch = new float[]
            {
                    0, 1, 2, 3,
                    4, 5, 6, 7,
                    8, 9, 10, 11,
                    12, 13, 14, 15,
                    0.0, 0.1, 0.2, 0.3,
                    0.4, 0.5, 0.6, 0.7,
                    0.8, 0.9, 0.10, 0.11,
                    0.12, 0.13, 0.14, 0.15,
                    1.0, 1.1, 1.2, 1.3,
                    1.4, 1.5, 1.6, 1.7,
                    1.8, 1.9, 1.10, 1.11,
                    1.12, 1.13, 1.14, 1.15,
            };
    matrix<float> m(4, 4, mulch, 3);
    matrix<int> sp1 = m.split_channel(1).convert_to(Round);

    matrix<float> sp2 = m.split_channel(2);
    matrix<float> sp3 = m.split_channel(3);
    sp3=sp3+sp1;
    sp3=sp3*sp2;
    matrix<double>sp4(4,4,-0.5);
    sp3=(sp3*sp4).convert_to<float>();
    for(int i=1;i<=4;i++)
    {
        for(int j=1;j<=4;j++)
        {
            cout<<sp3(i,j)<<"\t";
        }
        cout<<endl;
    }
}