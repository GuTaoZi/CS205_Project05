#include "matrix.hpp"
#include "bits/stdc++.h"

using namespace std;

int Round(float t)
{
    int k = t;
    float p = t - k;
    return ((p >= 0.5) ? 1 : 0) + k;
}

int Round(double t)
{
    int k = t;
    float p = t - k;
    return ((p >= 0.5) ? 1 : 0) + k;
}

bool larger_than(int o1, double o2)
{
    return ((double) o1) > o2;
}

int main()
{
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
    sp3 += sp1;
    matrix<double> sp4(4, 4, 1);
    sp4 = sp4.unary_calc(Round);
    sp4.fill(1, 2, 4, 4, 0);
    sp3 = sp1* sp4;
    sp3=sp3.transpose();
    for (int i = 1; i <= 4; i++)
    {
        for (int j = 1; j <= 4; j++)
        {
            cout << sp3(i, j) << "\t";
        }
        cout << endl;
    }
}