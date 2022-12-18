#include "matrix.hpp"
#include "bits/stdc++.h"

#define ll long long
using namespace std;

string *explore = new string[]
        {
                "Th", "mor", "earn ", "t C", "th", "re ig", "t I f",
                "Th", " you v", "muc", "for ", "r dedic", "eachin", "is cou"
        };

string *perseverance = new string[]
        {
                "e ", "e I l", "abou", "++, ", "e mo", "noran", "eel",
                "ank", "ery ", "h ", "you", "ation in t", "g th", "rse!"
        };

#define CS_205 (CS + _205)
int main()
{
    matrix<string> CS(2, 7, explore);
    matrix<string> _205(2, 7, perseverance);
    cout << CS_205<<endl;
}