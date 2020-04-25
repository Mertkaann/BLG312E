// C++ program to find the prime numbers
// between a given interval

#include <bits/stdc++.h>

#define mem(a, b) (2000 + ((b - a) / 1000 + 1) * 82)
//2000 + ((b - a) / 1000 + 1) * 82
using namespace std;

int main() {
    int ii = -1;
    int falses = 0;
    // Declare the variables
    int a, b, i, j, flag;
    int count = 0;
    // Ask user to enter lower value of interval
    cout << "Enter lower bound of the interval: ";
    cin >> a;  // Take input

    // Ask user to enter upper value of interval
    cout << "\nEnter upper bound of the interval: ";
    cin >> b;  // Take input

    // Print display message
    cout << "\nPrime numbers between "
         << a << " and " << b << " are: ";

    // Traverse each number in the interval
    // with the help of for loop
    for (i = a; i <= b; i++) {
        // Skip 0 and 1 as they are
        // niether prime nor composite
        if (i == 1 || i == 0)
            continue;

        // flag variable to tell
        // if i is prime or not
        flag = 1;

        for (j = 2; j <= i / 2; ++j) {
            if (i % j == 0) {
                flag = 0;
                break;
            }
        }

        // flag = 1 means i is prime
        // and flag = 0 means i is not prime
        if (flag == 1) {
            count++;
            //cout << i << " ";
        }
        if (i % 10000 == 0) {
            cout << count << " " << mem(a, i) << " " << falses << endl;
        }
        if (mem(a, i) <= count) {
            falses++;
        }
    }
    cout << "\n\n"
         << count << " " << b - a << " " << mem(a, b) << "\n\n";
    cout << falses << endl;
    return 0;
}

//((b - a) / 1000 + 1) * 150
//2000 + ((b - a) / 1000 + 1) * 82