////////////////////////////////////////////////////////////////////////////////
// MIT License
// 
// Copyright (c) 2023 underwatergrasshopper
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

/**
* @file ToStr.h
* @author underwatergrasshopper
* @version 0.2.0
*/

#ifndef TOSTR_H_
#define TOSTR_H_

#include <windows.h>
#include <locale.h>
#include <wchar.h>

#include <string>
#include <utility>

//------------------------------------------------------------------------------
// Declarations
//------------------------------------------------------------------------------

// Converts utf16 string to utf8 string.
// Invalid unicode will be replaced with 'EF BF BD' code sequence.
std::string ToUTF8(const std::wstring& text_utf16);

// Converts utf8 string to utf16 string.
// Invalid unicode will be replaced with 'FFFD' code.
std::wstring ToUTF16(const std::string& text_utf8);

//------------------------------------------------------------------------------

// This is the size for internal buffer placed in stack region of memory. 
// Functions placed below uses this buffer for internal string operations, until there is need for bigger one. 
// If there is need for bigger one, then dynamically allocated buffer is created temporary.
enum { TOSTR_MIN_BUFFER_SIZE = 4096 };

// Converts arguments to text according to the format. Encoding of strings: ASCII, UTF8.
// format           Same rules as for 'printf' function.
// arguments        Same rules as for 'printf' function.

std::string ToStr(const char* text); // pass through

std::string ToStr(const std::string& text); // pass through

template <typename... Types>
std::string ToStr(const char* format, Types&&... arguments);

template <typename... Types>
std::string ToStr(const std::string& format, Types&&... arguments);

// Replaces default function for handling error messages to custom, used by ToStr function. 
// After handling error message, ToStr function aborts execution of calling program.
void ToStr_SetHandleFatalErrorMessageFunction(void (*handle_fatal_error_message)(const char* message)); // not multi-thread safe

void ToStr_DefaultHandleFatalErrorMessage(const char* message);

//------------------------------------------------------------------------------

// Loads text from file.
// file_name            File name with full path to file. Encoding: ASCII.
// is_loaded            (Optional) If entire file text has been loaded - sets to true, otherwise - sets to false.
// Returns              Loaded text. Encoding: ASCII or UTF8.
std::string LoadTextFromFile(const std::string& file_name, bool* is_loaded = nullptr);


// Loads text from file. Reads content of file as is in UTF8 encoding.
// file_name            File name with full path to file. Encoding: ASCII or UTF8.
// is_loaded            (Optional) If entire file text has been loaded - sets to true, otherwise - sets to false.
// Returns              Loaded text. Encoding: ASCII or UTF8.
std::string LoadTextFromFileUTF8(const std::string& file_name, bool* is_loaded = nullptr);

// Loads text from file. Expects UTF8 BOM in file. Excludes any BOM from string.
// file_name            File name with full path to file. Encoding: ASCII or UTF8.
// is_loaded            (Optional) If entire file text has been loaded - sets to true, otherwise - sets to false.
// Returns              Loaded text. Encoding: ASCII or UTF8.
std::string LoadTextFromFileUTF8_BOM(const std::string& file_name, bool* is_loaded = nullptr);

// Saves text to file.
// file_name            File name with full path to file. Encoding: ASCII.
// text                 Text to be saved in file. Encoding: ASCII or UTF8.
// Returns              true    - if entire text has been loaded from file,
//                      false   - otherwise.
bool SaveTextToFile(const std::string& file_name, const std::string& text);


// Saves text to file in UTF8 format.
// file_name            File name with full path to file. Encoding: ASCII or UTF8.
// text                 Text to be saved in file. Encoding: ASCII or UTF8.
// Returns              true    - if entire text has been loaded from file,
//                      false   - otherwise.
bool SaveTextToFileUTF8(const std::string& file_name, const std::string& text);


// Saves text to file. Adds UTF8 BOM to file.
// file_name            File name with full path to file. Encoding: ASCII or UTF8.
// text                 Text to be saved in file. Encoding: ASCI or UTF8.
// Returns              true    - if entire text has been loaded from file,
//                      false   - otherwise.
bool SaveTextToFileUTF8_BOM(const std::string& file_name, const std::string& text);

//------------------------------------------------------------------------------
// Inner (only to use internally by this lib)
//------------------------------------------------------------------------------

struct ToStr_Data {
    void (*handle_fatal_error_message)(const char* message);
};

inline ToStr_Data& ToStr_ToData() {
    static ToStr_Data s_data = {
        ToStr_DefaultHandleFatalErrorMessage
    };

    return s_data;
}

inline void ToStr_FatalError(const char* message) {
    ToStr_Data& data = ToStr_ToData();
    if (data.handle_fatal_error_message) data.handle_fatal_error_message(message);
    exit(EXIT_FAILURE);
}

//------------------------------------------------------------------------------

inline std::string ToUTF8(const std::wstring& text_utf16) {
    auto GetErrMsg = [](DWORD error_code) -> const char* {
        return "ToUTF8 Error: Can not convert a text from utf-16 to utf-8.";
    };

    std::string text_utf8;

    char stack_buffer[TOSTR_MIN_BUFFER_SIZE] = {};

    if (!text_utf16.empty()) {
        int size = WideCharToMultiByte(CP_UTF8, 0, text_utf16.c_str(), -1, NULL, 0, NULL, NULL);
        if (size == 0) {
            ToStr_FatalError(GetErrMsg(GetLastError()));
        }

        char* buffer = (size > TOSTR_MIN_BUFFER_SIZE) ? (new char[size]) : stack_buffer;

        size = WideCharToMultiByte(CP_UTF8, 0, text_utf16.c_str(), -1, buffer, size, NULL, NULL);
        if (size == 0) {
            ToStr_FatalError(GetErrMsg(GetLastError()));
        }

        if (size > 1) text_utf8 = std::string(buffer, size - 1);

        if (buffer != stack_buffer) delete[] buffer;
    }

    return text_utf8;
}

inline std::wstring ToUTF16(const std::string& text_utf8) {
    auto GetErrMsg = [](DWORD error_code) -> const char* {
        return "ToUTF16 Error: Can not convert a text from utf-8 to utf-16.";
    };

    std::wstring text_utf16;

    wchar_t stack_buffer[TOSTR_MIN_BUFFER_SIZE] = {};

    if (!text_utf8.empty()) {
        int size = MultiByteToWideChar(CP_UTF8, 0, text_utf8.c_str(), -1, NULL, 0);
        if (size == 0) {
            ToStr_FatalError(GetErrMsg(GetLastError()));
        }

        wchar_t* buffer = (size > TOSTR_MIN_BUFFER_SIZE) ? (new wchar_t[size]) : stack_buffer;

        size = MultiByteToWideChar(CP_UTF8, 0, text_utf8.c_str(), -1, buffer, size);
        if (size == 0) {
            ToStr_FatalError(GetErrMsg(GetLastError()));
        }

        if (size > 1) text_utf16 = std::wstring(buffer, size - 1);

        if (buffer != stack_buffer) delete[] buffer;
    }

    return text_utf16;
}

//------------------------------------------------------------------------------

inline std::string ToStr(const char* text) {
    if (text == nullptr) {
        ToStr_FatalError("ToStr Error: Argument 'text' can not be 0 or nullptr.");
    } 
    return std::string(text);
}


inline std::string ToStr(const std::string& text) {
    return text;
}

template <typename... Types>
std::string ToStr(const char* format, Types&&... arguments) {
    std::string text;

    char stack_buffer[TOSTR_MIN_BUFFER_SIZE];

    if (format == nullptr) {
        ToStr_FatalError("ToStr Error: Argument 'format' can not be 0 or nullptr.");
    } 

    const int length = snprintf(stack_buffer, TOSTR_MIN_BUFFER_SIZE, format, arguments...);

    if (length < 0) {
        ToStr_FatalError("ToStr Error: Encoding error.");
    } 

    if (length >= TOSTR_MIN_BUFFER_SIZE) {
        const size_t    size    = length + 1;
        char*           buffer  = new char[size];

        const int expected_same_length = snprintf(buffer, size, format, arguments...);

        if (expected_same_length < 0) {
            ToStr_FatalError("ToStr Error: Encoding error at second writing to buffer.");
        } 
        if (expected_same_length != length) {
            ToStr_FatalError("ToStr Error: Message actual length miss-match between first and second write to buffer.");
        }

        text = std::string(buffer, length);

        delete[] buffer;
    } else {
        text = std::string(stack_buffer, length);
    }

    return text;
}

template <typename... Types>
inline std::string ToStr(const std::string& format, Types&&... arguments) {
    return ToStr(format.c_str(), std::forward<Types>(arguments)...);
}

inline void ToStr_SetHandleFatalErrorMessageFunction(void (*handle_fatal_error_message)(const char* message)) {
    ToStr_ToData().handle_fatal_error_message = handle_fatal_error_message;
}

inline void ToStr_DefaultHandleFatalErrorMessage(const char* message) {
    if (fwide(stdout, 0) > 0) {
        wprintf(L"%hs\n", message);
    } else {
        puts(message);
    }
    fflush(stdout);
}

//------------------------------------------------------------------------------

class ToStr_LocaleGuardian {
public:
    ToStr_LocaleGuardian(int category, const char* locale) {
        char* beckup = setlocale(LC_ALL, ".UTF8");
        m_backup = beckup ? std::string(beckup) : "";
    }
    virtual ~ToStr_LocaleGuardian() {
        if (!m_backup.empty()) setlocale(LC_ALL, m_backup.c_str());
    }
private:
    std::string m_backup;
};

#define TOSTR_LOCALE_GUARDIAN_UTF8() ToStr_LocaleGuardian locale_guardian_utf8(LC_ALL, ".UTF8")

inline std::string LoadTextFromFile(const std::string& file_name, bool* is_loaded) {
    std::string text;

    FILE* file = nullptr;
    if (fopen_s(&file, file_name.c_str(), "rt") == 0 && file) {

        char c;
        while ((c = fgetc(file)) != EOF) text += c;
        fclose(file);

        if (is_loaded) *is_loaded = true;
    } else {
        if (is_loaded) *is_loaded = false;
    }

    return text;
}

inline std::string LoadTextFromFileUTF8(const std::string& file_name, bool* is_loaded) {
    std::string text;

    FILE* file = nullptr;
    if (_wfopen_s(&file, ToUTF16(file_name).c_str(), L"rb") == 0 && file) {
        char c;
        while (fread(&c, sizeof(char), 1, file) == 1) {
            if (c != '\r') text += c; // suppress 'carriage return' (CR)
        }

        const bool is_all_read = feof(file);

        fclose(file);

        if (is_loaded) *is_loaded = is_all_read;
    } else {
        if (is_loaded) *is_loaded = false;
    }

    return text;
}

inline std::string LoadTextFromFileUTF8_BOM(const std::string& file_name, bool* is_loaded) {
    std::wstring text;

    FILE* file = nullptr;
    if (_wfopen_s(&file, ToUTF16(file_name).c_str(), L"rt, ccs=UTF-8") == 0 && file) {

        wchar_t c;
        while ((c = fgetwc(file)) != WEOF) text += c;
        fclose(file);

        if (is_loaded) *is_loaded = true;
    } else {
        if (is_loaded) *is_loaded = false;
    }

    return ToUTF8(text);
}

inline bool SaveTextToFile(const std::string& file_name, const std::string& text) {
    FILE* file = nullptr;
    if (fopen_s(&file, file_name.c_str(), "wt") == 0 && file) {
        const int count = fprintf(file, "%s", text.c_str());
        fclose(file);

        return count == text.length();
    }

    return false;
}

inline bool SaveTextToFileUTF8(const std::string& file_name, const std::string& text) {
    FILE* file = nullptr;
    if (_wfopen_s(&file, ToUTF16(file_name).c_str(), L"wb") == 0 && file) {
        const size_t count =  fwrite(text.c_str(), sizeof(char), text.length(), file);
        fclose(file);

        return count == text.length();
    }

    return false;
}

inline bool SaveTextToFileUTF8_BOM(const std::string& file_name, const std::string& text) {
    FILE* file = nullptr;
    if (_wfopen_s(&file, ToUTF16(file_name).c_str(), L"wb") == 0 && file) {

        constexpr static char BOM[] = { '\xEF', '\xBB', '\xBF' };

        const bool is_wirten = 
            fwrite(BOM, sizeof(char), sizeof(BOM), file) == 3 &&
            fwrite(text.c_str(), sizeof(char), text.length(), file) == text.length();

        fclose(file);

        return is_wirten;
    }

    return false;
}

#endif // TOSTR_H_
