#include <cmath>
int main() {
    volatile double x = 0;
    for (;;) {
        x += std::sin(x);
    }
    return 0;
}