#include "ToStr_Tests.h"

#include "ToStr.h"

#include <stdio.h>
#include <windows.h>

#include <TrivialTestKit.h>

#include <set>

std::string LoadContentFromFile(const std::string& file_name) {
    std::string content;

    FILE* file = nullptr;
    if (fopen_s(&file, file_name.c_str(), "r") == 0 && file) {

        char c;
        while ((c = fgetc(file)) != EOF) content += c;
        fclose(file);
    }

    return content;
}

void TestToStr() {
    // empty string
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
#endif
    TTK_ASSERT(ToStr("") == std::string(""));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

    // simple text
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
#endif
    TTK_ASSERT(ToStr("abc") == std::string("abc"));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

    TTK_ASSERT(ToStr("%s", "abc") == std::string("abc"));

    // unicode characters
    TTK_ASSERT(ToStr("%s", u8"\u0444\u0105") == std::string(u8"\u0444\u0105"));

    // convert to text
    TTK_ASSERT(ToStr("%s %d %.2f", "text", 123, 3.14) == std::string("text 123 3.14"));
    TTK_ASSERT(ToStr(std::string("%s %d %.2f"), "text", 123, 3.14) == std::string("text 123 3.14"));

    // long text
    {
        const std::string sequence =  
            "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef" 
            "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef" 
            "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
            "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

        std::string long_text;

        size_t number = TOSTR_DEF_BUFFER_SIZE * 2 / sequence.length();

        for (size_t ix = 0; ix < number; ++ix) long_text += sequence;

        TTK_ASSERT(ToStr("%s", long_text.c_str()) == long_text);
    }

    // NOTE: Undefined behaviors:
    // ToStr("%s", 4);      // wrong argument type
    // ToStr("%d", 3, 4);   // to many arguments
    // ToStr("%d %d", 4);   // not enough arguments
}

void TestToStrFATAL_ERRROR() {
    TTK_ASSERT(CreateDirectoryA(".\\log", 0) || GetLastError() == ERROR_ALREADY_EXISTS);
    TTK_ASSERT(CreateDirectoryA(".\\log\\test", 0) || GetLastError() == ERROR_ALREADY_EXISTS);

    system("ToStr_Test.exe ZERO_FORMAT > log\\test\\zero_format.txt");
    TTK_ASSERT(LoadContentFromFile("log\\test\\zero_format.txt") == "ToStr Error: Argument 'format' can not be 0 or '\\0'.\n");

    system("ToStr_Test.exe CUSTOM_ERR_MSG_HANDLING > log\\test\\custom_err_msg_handling.txt");
    TTK_ASSERT(LoadContentFromFile("log\\test\\custom_err_msg_handling.txt") == "ToStr Error: Argument 'format' can not be 0 or '\\0'.\naddition text\n");
}

int ToStr_RunTests(int argc, char *argv[]) {
    std::set<std::string> flags;

    for (size_t index = 0; index < argc; ++index) {
        flags.insert(argv[index]);
    }

    auto IsFlag = [&flags](const std::string& flag) { 
        return flags.find(flag) != flags.end(); 
    };

    if (IsFlag("ZERO_FORMAT")) {
        ToStr(0); // this function will exit with error code

        return false;

    } else if (IsFlag("CUSTOM_ERR_MSG_HANDLING")) {
        ToStr_SetHandleFatalErrorMessageFunction([](const char* message){
            puts(message);
            puts("addition text");
            fflush(stdout);
        });
        ToStr(0); // this function will exit with error code

        return false;

    } else {
        TTK_ADD_TEST(TestToStr, 0);
        TTK_ADD_TEST(TestToStrFATAL_ERRROR, 0);

        return !TTK_Run();
    }
}