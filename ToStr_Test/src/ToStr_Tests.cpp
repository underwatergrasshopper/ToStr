#include "ToStr_Tests.h"

#include "ToStr.h"

#include <stdio.h>
#include <windows.h>

#include <TrivialTestKit.h>

void TestBasic() {
    // ToStr(0); // TODO: To test this make _Test.exe FATAL_ERROR
    // if (IsFlag("FATAL_ERROR") {
    //      system("_Test.exe ZERO_FORMAT > log\test_fatal_error.txt");
    //      TTK_ASSERT(Load() == expected)
    //      system("_Test.exe WRONG_ENCODING");

    // empty string
    TTK_ASSERT(ToStr("") == std::string(""));

    // simple text
    TTK_ASSERT(ToStr("abc") == std::string("abc"));

    // unicode characters
    TTK_ASSERT(ToStr("%s", "\u0444\u0105") == std::string("\u0444\u0105"));
}

int ToStr_RunTests() {
    TTK_ADD_TEST(TestBasic, 0);

    return !TTK_Run();
}