#include <iostream>
using namespace std;
int main() {
    int maxint = 200;
    int minint = 101;
    size_t num_of_groups = 3;
    int interval = maxint - minint;
    for (size_t i = 0; i < num_of_groups; i++) {
        std::pair<size_t, size_t> pair{
            minint + i * interval / num_of_groups,
            minint + (i + 1) * interval / num_of_groups};
        std::cout << "Group " << (i + 1) << ": [" << pair.first << "," << pair.second << ") - " << (pair.second - pair.first) << " elements." << std::endl;
    }
    return 0;
}