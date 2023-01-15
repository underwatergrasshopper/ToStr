#include <stdio.h>
#include <windows.h>

#include "ToStr.h"

int main() {
    puts(ToStr("", "").c_str());

    enum { LEN = 16 };
    char buffer[LEN];
    memset(buffer, '.', LEN);

    int length = snprintf(buffer, 8, "%s", "1234567");

    printf("%d\n", length);
    printf("%.*s\n", 16, buffer);


    return 0;
}
