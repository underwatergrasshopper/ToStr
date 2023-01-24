#include "ToStr_Tests.h"

#include "ToStr.h"

#include <stdio.h>
#include <windows.h>

#include <set>

#include <TrivialTestKit.h>

//------------------------------------------------------------------------------
// Support
//------------------------------------------------------------------------------

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

bool CreateReadOnlyFile(const std::wstring& file_name) {
    HANDLE file_handle = CreateFileW(
        file_name.c_str(),
        GENERIC_READ,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_READONLY,
        NULL
    );

    if (file_handle == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_EXISTS) return true;
        return false;
    } 

    CloseHandle(file_handle);
    return true;
}

void PrintData(const unsigned char* data, size_t size) {
    for (size_t index = 0; index < size; ++index) {
        printf("%02X  ", data[index]);
    }
    puts("");
}

void PrintTextData(const char* text) {
    PrintData((const unsigned char*)text, strlen(text));
}

void PrintTextData(const wchar_t* text) {
    PrintData((const unsigned char*)text, wcslen(text) * 2);
}

template <unsigned N>
std::string CodeToTextUTF8(const uint8_t (&codes)[N]) {
    return std::string((const char*)codes);
}

template <unsigned N>
std::wstring CodeToTextUTF16(const uint16_t (&codes)[N]) {
    return std::wstring((const wchar_t*)codes);
}

//------------------------------------------------------------------------------
// Tests
//------------------------------------------------------------------------------

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

    // wrong encoding
    {
        TTK_ASSERT(ToUTF16(CodeToTextUTF8({'t', 'e', 'x', 't', '\0'})) == CodeToTextUTF16({'t', 'e', 'x', 't', '\0'})); // correct, control one
        // U+FFFD - Replacement Character
        TTK_ASSERT(ToUTF16(CodeToTextUTF8({0xC2, 'e', 'x', 't', '\0'})) == CodeToTextUTF16({0xFFFD, 'e', 'x', 't', '\0'}));
        TTK_ASSERT(ToUTF16(CodeToTextUTF8({0xFF, 'e', 'x', 't', '\0'})) == CodeToTextUTF16({0xFFFD, 'e', 'x', 't', '\0'}));
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

    // wrong encoding
    {
        TTK_ASSERT(ToUTF8(CodeToTextUTF16({'t', 'e', 'x', 't', '\0'})) == CodeToTextUTF8({'t', 'e', 'x', 't', '\0'})); // correct, control one
        // U+FFFD - Replacement Character (EF BF BD in utf8)
        TTK_ASSERT(ToUTF8(CodeToTextUTF16({0xDC00, 'e', 'x', 't', '\0'})) == CodeToTextUTF8({0xEF, 0xBF, 0xBD, 'e', 'x', 't', '\0'})); 
        TTK_ASSERT(ToUTF8(CodeToTextUTF16({0xD800, 'e', 'x', 't', '\0'})) == CodeToTextUTF8({0xEF, 0xBF, 0xBD, 'e', 'x', 't', '\0'}));
    }
}

void TestLoadSave() {
    TTK_ASSERT(CreateDirectoryA(".\\log", 0) || GetLastError() == ERROR_ALREADY_EXISTS);
    TTK_ASSERT(CreateDirectoryA(".\\log\\test", 0) || GetLastError() == ERROR_ALREADY_EXISTS);

    // ascii
    {
        const std::string file_name = "log\\test\\TestLoadSave.txt";
        const std::string expected_content = u8"Some text\n\uD558\u0444\U00020001\n.";

        TTK_ASSERT(SaveTextToFile(file_name, expected_content));

        bool is_loaded = false;
        const std::string content = LoadTextFromFile(file_name, &is_loaded);

        TTK_ASSERT(is_loaded);
        TTK_ASSERT(content == expected_content);
    }

    // empty file, ascii
    {
        const std::string file_name = "log\\test\\TestLoadSave_EmptyFile.txt";

        TTK_ASSERT(SaveTextToFile(file_name, ""));

        bool is_loaded = false;
        const std::string content = LoadTextFromFile(file_name, &is_loaded);

        TTK_ASSERT(is_loaded);
        TTK_ASSERT(content == "");
    }

    
    // no is_loaded check, ascii
    {
        const std::string file_name = "log\\test\\TestLoadSave_NoIsLoadedCheck.txt";
        const std::string expected_content = u8"Some text\n\uD558\u0444\U00020001\n.";

        TTK_ASSERT(SaveTextToFile(file_name, expected_content));

        const std::string content = LoadTextFromFile(file_name);

        TTK_ASSERT(content == expected_content);
    }
    
    // load from not existing, ascii
    {
        const std::string file_name = "log\\test\\TestLoadSave_NotExisting.txt";

        bool is_loaded = false;
        const std::string content = LoadTextFromFile(file_name, &is_loaded);

        TTK_ASSERT(!is_loaded);
        TTK_ASSERT(content == "");
    }

    // load from not existing, no is_loaded check, ascii
    {
        const std::string file_name = "log\\test\\TestLoadSave_NotExisting_NoIsLoadedCheck.txt";

        const std::string content = LoadTextFromFile(file_name);

        TTK_ASSERT(content == "");
    }

    // try save to read only, ascii
    {
        const std::string file_name = "log\\test\\TestLoadSave_TrySaveToReadOnly.txt";
        const std::wstring file_name_utf16 = ToUTF16(file_name);
        const std::string expected_content = u8"Some text\n\uD558\u0444\U00020001\n.";

        TTK_ASSERT(CreateReadOnlyFile(file_name_utf16));
        TTK_ASSERT(!SaveTextToFile(file_name, expected_content));

        bool is_loaded = false;
        const std::string content = LoadTextFromFile(file_name, &is_loaded);

        TTK_ASSERT(is_loaded);
        TTK_ASSERT(content == "");
    }
 
    // utf8
    {
        const std::string file_name = u8"log\\test\\TestLoadSave_\u0107\u0119\u0144.txt";
        const std::string expected_content = u8"Some text\n\uD558\u0444\U00020001\n.";

        TTK_ASSERT(SaveTextToFileUTF8(file_name, expected_content));

        bool is_loaded = false;
        const std::string content = LoadTextFromFileUTF8(file_name, &is_loaded);

        TTK_ASSERT(is_loaded);
        TTK_ASSERT(content == expected_content);
    }

    // empty file, utf8
    {
        const std::string file_name = u8"log\\test\\TestLoadSave_EmptyFile_\u0107\u0119\u0144.txt";

        TTK_ASSERT(SaveTextToFileUTF8(file_name, ""));

        bool is_loaded = false;
        const std::string content = LoadTextFromFileUTF8(file_name, &is_loaded);

        TTK_ASSERT(is_loaded);
        TTK_ASSERT(content == "");
    }

    // no is_loaded check, utf8
    {
        const std::string file_name = u8"log\\test\\TestLoadSave_NoIsLoadedCheck_\u0107\u0119\u0144.txt";
        const std::string expected_content = u8"Some text\n\uD558\u0444\U00020001\n.";

        TTK_ASSERT(SaveTextToFileUTF8(file_name, expected_content));

        const std::string content = LoadTextFromFileUTF8(file_name);

        TTK_ASSERT(content == expected_content);
    }

    // load from not existing, utf8
    {
        const std::string file_name = u8"log\\test\\TestLoadSave_NotExisting_\u0107\u0119\u0144.txt";

        bool is_loaded = false;
        const std::string content = LoadTextFromFileUTF8(file_name, &is_loaded);

        TTK_ASSERT(!is_loaded);
        TTK_ASSERT(content == "");
    }

    // load from not existing, no is_loaded check, utf8
    {
        const std::string file_name = u8"log\\test\\TestLoadSave_NotExisting_NoIsLoadedCheck_\u0107\u0119\u0144.txt";

        const std::string content = LoadTextFromFileUTF8(file_name);

        TTK_ASSERT(content == "");
    }

    // try save to read only, utf8
    {
        const std::string file_name = u8"log\\test\\TestLoadSave_TrySaveToReadOnly_\u0107\u0119\u0144.txt";
        const std::wstring file_name_utf16 = ToUTF16(file_name);
        const std::string expected_content = u8"Some text\n\uD558\u0444\U00020001\n.";

        TTK_ASSERT(CreateReadOnlyFile(file_name_utf16));
        TTK_ASSERT(!SaveTextToFileUTF8(file_name, expected_content));

        bool is_loaded = false;
        const std::string content = LoadTextFromFileUTF8(file_name, &is_loaded);

        TTK_ASSERT(is_loaded);
        TTK_ASSERT(content == "");
    }
}

void TestToStr() {
    // empty string
    TTK_ASSERT(ToStr("") == std::string(""));
    TTK_ASSERT(ToStr("", "") == std::string("")); // second arg should be ignored

    // simple text
    TTK_ASSERT(ToStr("abc") == std::string("abc"));
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
    TTK_ASSERT(InnerLoadContentFromFile("log\\test\\zero_format.txt") == "ToStr Error: Argument 'text' can not be 0 or nullptr.\n");

    system("ToStr_Test.exe ZERO_FORMAT_WITH_ARG > log\\test\\zero_format_with_arg.txt");
    TTK_ASSERT(InnerLoadContentFromFile("log\\test\\zero_format_with_arg.txt") == "ToStr Error: Argument 'format' can not be 0 or nullptr.\n");

    system("ToStr_Test.exe CUSTOM_ERR_MSG_HANDLING > log\\test\\custom_err_msg_handling.txt");
    TTK_ASSERT(InnerLoadContentFromFile("log\\test\\custom_err_msg_handling.txt") == "ToStr Error: Argument 'text' can not be 0 or nullptr.\naddition text\n");
}

//------------------------------------------------------------------------------

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
        ToStr(0, ""); // this function will exit with error code
        return false;

    } else if (IsFlag("ZERO_FORMAT_WITH_ARG")) {
        ToStr(0, ""); // this function will exit with error code
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