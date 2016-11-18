#include <stdio.h>

#include <Fever/Sample.h>

int main(void) {
    printf("Get three: %d\n", fvr::sample::getThree());
    printf("Get four: %d\n", fvr::sample::getFour(nullptr));

    return 0;
}
