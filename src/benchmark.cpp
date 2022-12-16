#include "matrix.hpp"
#include "bits/stdc++.h"

using namespace std;

int main()
{
    int *s = new int[]{0, 1, 2, 3, 4,
                       5, 6, 7, 8, 9,
                       10, 11, 12, 13, 14,
                       15, 16, 17, 18, 19,
                       20, 21, 22, 23, 24};
    matrix<int> p(5, 5, s);
    cout << p.at(3, 3) << endl;
    matrix<int> sub(p, 2, 3, 4, 5);
    p(4, 5) = 6;
    cout << sub.at(3, 3)<<endl;
}