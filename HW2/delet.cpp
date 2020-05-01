// C++ program to find the prime numbers
// between a given interval

#include <bits/stdc++.h>

#define mem(a, b) (2000 + ((b - a) / 1000 + 1) * 82)
int* ShmPTR;
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
    int memory = mem(a, b);
    ShmPTR = (int*)malloc(memory * sizeof(pthread_t));
    // Traverse each number in the interval
    // with the help of for loop
    int cur = 0;
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
            ShmPTR[cur++] = i;
        }

        if (mem(a, i) <= count) {
            falses++;
        }
        if (i % 10000 == 0)
            ;  //cout << "false: " << falses << " " << i << endl;
    }
    cout << "\n\ncounted "
         << count << " " << b - a << " " << mem(a, b) << "\n\n";
    cout << falses << endl;

    for (int i = 0; i < memory; i++) {
        if (ShmPTR[i] != 0)
            printf("%d, ", ShmPTR[i]);
    }
    cout << "counted " << count << endl;
    return 0;
}

//((b - a) / 1000 + 1) * 150
//2000 + ((b - a) / 1000 + 1) * 82