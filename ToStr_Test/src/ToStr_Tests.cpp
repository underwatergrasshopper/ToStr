#include "ToStr_Tests.h"

#include "ToStr.h"

#include <stdio.h>
#include <windows.h>

#include <TrivialTestKit.h>

#include <set>

std::string InnerLoadContentFromFile(const std::string& file_name) {
    std::string content;

    FILE* file = nullptr;
    if (fopen_s(&file, file_name.c_str(), "r") == 0 && file) {

        char c;
        while ((c = fgetc(file)) != EOF) content += c;
        fclose(file);
    }

    return content;
}

void TestToUTF16() {
    // empty text
    TTK_ASSERT(ToUTF16(u8"") == L"");

    // unicode characters
    TTK_ASSERT(ToUTF16(u8"Some text\u0444\U0002F820.") == L"Some text\u0444\U0002F820.");

    // long text
    {
        const std::string sequence_utf8 =  
            u8"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            u8"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            u8"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            u8"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. ";

        const std::wstring sequence_utf16 =  
            L"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            L"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            L"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            L"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. ";

        std::string long_text_utf8;
        std::wstring long_text_utf16;

        const size_t number = TOSTR_MIN_BUFFER_SIZE * 2 / sequence_utf16.length();
        for (size_t ix = 0; ix < number; ++ix) {
            long_text_utf8  += sequence_utf8;
            long_text_utf16 += sequence_utf16;
        }

        TTK_ASSERT(ToUTF16(long_text_utf8) == long_text_utf16);
    }
}

void TestToUTF8() {
    // empty text
    TTK_ASSERT(ToUTF8(L"") == u8"");

    // unicode characters
    TTK_ASSERT(ToUTF8(L"Some text\u0444\U0002F820.") == u8"Some text\u0444\U0002F820.");

    // long text
    {
        const std::string sequence_utf8 =  
            u8"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            u8"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            u8"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            u8"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. ";

        const std::wstring sequence_utf16 =  
            L"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            L"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            L"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. " 
            L"Some text\u0444. Some text\u0444. Some text\u0444. Some text\u0444. ";

        std::string long_text_utf8;
        std::wstring long_text_utf16;

        const size_t number = TOSTR_MIN_BUFFER_SIZE * 2 / sequence_utf8.length();
        for (size_t ix = 0; ix < number; ++ix) {
            long_text_utf8  += sequence_utf8;
            long_text_utf16 += sequence_utf16;
        }

        TTK_ASSERT(ToUTF8(long_text_utf16) == long_text_utf8);
    }
}

void TestLoadSave() {
    TTK_ASSERT(CreateDirectoryA(".\\log", 0) || GetLastError() == ERROR_ALREADY_EXISTS);
    TTK_ASSERT(CreateDirectoryA(".\\log\\test", 0) || GetLastError() == ERROR_ALREADY_EXISTS);

    {
        const std::string file_name = "log\\test\\file.txt";
        const std::string expected_content = u8"Some text\n\uD558\u0444\U00020001\n.";

        TTK_ASSERT(SaveTextToFile(file_name, expected_content));

        bool is_loaded = false;
        const std::string content = LoadTextFromFile(file_name, &is_loaded);

        TTK_ASSERT(is_loaded);
        TTK_ASSERT(expected_content == content);
    }

    {
        const std::string file_name = u8"log\\test\\\u0107\u0119\u0144.txt";
        const std::string expected_content = u8"Some text\n\uD558\u0444\U00020001\n.";

        TTK_ASSERT(SaveTextToFileUTF8(file_name, expected_content));

        bool is_loaded = false;
        const std::string content = LoadTextFromFileUTF8(file_name, &is_loaded);

        TTK_ASSERT(is_loaded);
        TTK_ASSERT(expected_content == content);
    }

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

        size_t number = TOSTR_MIN_BUFFER_SIZE * 2 / sequence.length();

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
    TTK_ASSERT(InnerLoadContentFromFile("log\\test\\zero_format.txt") == "ToStr Error: Argument 'format' can not be 0 or '\\0'.\n");

    system("ToStr_Test.exe CUSTOM_ERR_MSG_HANDLING > log\\test\\custom_err_msg_handling.txt");
    TTK_ASSERT(InnerLoadContentFromFile("log\\test\\custom_err_msg_handling.txt") == "ToStr Error: Argument 'format' can not be 0 or '\\0'.\naddition text\n");
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
        TTK_ADD_TEST(TestToUTF8, 0);
        TTK_ADD_TEST(TestToUTF16, 0);
        TTK_ADD_TEST(TestLoadSave, 0);
        TTK_ADD_TEST(TestToStr, 0);
        TTK_ADD_TEST(TestToStrFATAL_ERRROR, 0);

        return !TTK_Run();
    }
}