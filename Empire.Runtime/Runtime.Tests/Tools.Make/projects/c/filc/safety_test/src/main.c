/* Test that filc catches calling a function pointer with a wrong address.
 *
 * Expected runtime output (exit non-zero):
 *   filc safety error: cannot access pointer as function with ptr != aux ...
 *   filc panic: thwarted a futile attempt to violate memory safety.
 */
#include <stdfil.h>
#include <stdlib.h>

static void foo(void) {
}

int main() {
    void (*my_foo)(void) = (void(*)(void))((char*)foo + 42);
    my_foo();
    return 0;
}
