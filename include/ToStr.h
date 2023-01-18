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
* @version 0.1.0
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
std::string ToUTF8(const std::wstring& text_utf16);

// Converts utf8 string to utf16 string.
std::wstring ToUTF16(const std::string& text_utf8);

//------------------------------------------------------------------------------

// If ToStr function exceeds size of TOSTR_MIN_BUFFER_SIZE, then dynamically allocates new one with needed size.
// In other words, generated messages can be longer than this size.

// This is the internal buffer size under which no dynamic allocation happens internally in this lib.
enum { TOSTR_MIN_BUFFER_SIZE = 4096 };

// Converts arguments to text according to the format. Encoding of strings: ASCII, UTF8.
// format           Same rules as for 'printf' function.
// arguments        Same rules as for 'printf' function.
template <typename... Types>
std::string ToStr(const char* format, Types&&... arguments);

template <typename... Types>
std::string ToStr(const std::string& format, Types&&... arguments);

// Replaces default function for handling error messages to custom, used by ToStr function. 
// After handling error message, ToStr function aborts execution of calling program.
void ToStr_SetHandleFatalErrorMessageFunction(void (*handle_fatal_error_message)(const char* message)); // not multi-thread safe

void ToStr_DefaultHandleFatalErrorMessage(const char* message);

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

void ToStr_FatalError(const char* message) {
    ToStr_Data& data = ToStr_ToData();
    if (data.handle_fatal_error_message) data.handle_fatal_error_message(message);
    exit(EXIT_FAILURE);
}

//------------------------------------------------------------------------------

inline std::string ToUTF8(const std::wstring& text_utf16) {
    auto GetErrMsg = [](DWORD error_code) -> const char* {
        switch (error_code) {
        case ERROR_NO_UNICODE_TRANSLATION:  return "ToUTF8 Error: Invalid unicode character was found in a string.";
        }
        return "ToUTF8 Error: Can not convert a text from utf-16 to utf-8.";
    };

    std::string text_utf8;

    char stack_buffer[TOSTR_MIN_BUFFER_SIZE] = {};

    if (!text_utf16.empty()) {
        int count = WideCharToMultiByte(CP_UTF8, 0, text_utf16.c_str(), -1, NULL, 0, NULL, NULL);
        if (count == 0) {
            ToStr_FatalError(GetErrMsg(GetLastError()));
        }

        char* buffer = (count > TOSTR_MIN_BUFFER_SIZE) ? (new char[count]) : stack_buffer;

        count = WideCharToMultiByte(CP_UTF8, 0, text_utf16.c_str(), -1, buffer, count, NULL, NULL);
        if (count == 0) {
            ToStr_FatalError(GetErrMsg(GetLastError()));
        }

        text_utf8 = std::string(buffer);

        if (buffer != stack_buffer) delete[] buffer;
    }

    return text_utf8;
}

inline std::wstring ToUTF16(const std::string& text_utf8) {
    auto GetErrMsg = [](DWORD error_code) -> const char* {
        switch (error_code) {
        case ERROR_NO_UNICODE_TRANSLATION:  return "ToUTF16 Error: Invalid unicode character was found in a string.";
        }
        return "ToUTF16 Error: Can not convert a text from utf-8 to utf-16.";
    };

    std::wstring text_utf16;

    wchar_t stack_buffer[TOSTR_MIN_BUFFER_SIZE] = {};

    if (!text_utf8.empty()) {
        int count = MultiByteToWideChar(CP_UTF8, 0, text_utf8.c_str(), -1, NULL, 0);
        if (count == 0) {
            ToStr_FatalError(GetErrMsg(GetLastError()));
        }

        wchar_t* buffer = (count > TOSTR_MIN_BUFFER_SIZE) ? (new wchar_t[count]) : stack_buffer;

        count = MultiByteToWideChar(CP_UTF8, 0, text_utf8.c_str(), -1, buffer, count);
        if (count == 0) {
            ToStr_FatalError(GetErrMsg(GetLastError()));
        }

        text_utf16 = std::wstring(buffer);

        if (buffer != stack_buffer) delete[] buffer;
    }

    return text_utf16;
}

//------------------------------------------------------------------------------

template <typename... Types>
std::string ToStr(const char* format, Types&&... arguments) {
    std::string text;

    char stack_buffer[TOSTR_MIN_BUFFER_SIZE];

    if (format == nullptr) {
        ToStr_FatalError("ToStr Error: Argument 'format' can not be 0 or '\\0'.");
    } 

    const int length = snprintf(stack_buffer, TOSTR_MIN_BUFFER_SIZE, format, arguments...);

    if (errno != 0) {
        if (errno == EINVAL) ToStr_FatalError("ToStr Error: Wrong argument.");
        ToStr_FatalError("ToStr Error: Writing to buffer failed.");
    } 

    if (length < 0) {
        ToStr_FatalError("ToStr Error: Encoding error.");
    } 

    if (length >= TOSTR_MIN_BUFFER_SIZE) {
        const size_t    size    = length + 1;
        char*           buffer  = new char[size];

        const int expected_same_length = snprintf(buffer, size, format, arguments...);

        if (errno != 0) {
            if (errno == EINVAL) ToStr_FatalError("ToStr Error: Wrong argument at second writing to buffer.");
            ToStr_FatalError("ToStr Error: Writing to buffer failed at second writing to buffer.");
        } 
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

#endif // TOSTR_H_
