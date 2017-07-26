#include <stdio.h>

void do_fun(int i) {
    int tm = i/0;
}
int main() {
    int i=3;

    do_fun(i);
    return 0;
}
