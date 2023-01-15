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

#include <string>
#include <utility>

//------------------------------------------------------------------------------
// Declarations
//------------------------------------------------------------------------------

template <typename... Types>
std::string ToStr(const char* format, Types&&... arguments);

template <typename... Types>
std::string ToStr(const std::string& format, Types&&... arguments);

void ToStr_SetHandleFatalErrorMessageFunction(void (*handle_fatal_error_message)(const char* message));
void ToStr_DefaultHandleFatalErrorMessage(const char* message);

//------------------------------------------------------------------------------
// Inner
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

template <typename... Types>
inline std::string ToStr(const char* format, Types&&... arguments) {

    auto FatalError = [](const char* message) {
        ToStr_Data& data = ToStr_ToData();
        if (data.handle_fatal_error_message) data.handle_fatal_error_message(message);
        exit(EXIT_FAILURE);
    };

    std::string text;

    enum { SIZE = 4096 };
    char buffer[SIZE];

    const int length = snprintf(buffer, SIZE, format, arguments...);

    if (errno != 0) {
        if (errno == EINVAL) FatalError("ToStr Error: Wrong argument.");
        FatalError("ToStr Error: Writing to buffer failed.");
    } 
    if (length < 0) {
        FatalError("ToStr Error: Encoding error.");
    }
    if (length >= SIZE) {
        const size_t    ext_size    = length + 1;
        char*           ext_buffer  = new char[ext_size];

        const int same_length = snprintf(ext_buffer, ext_size, format, arguments...);

        if (errno != 0) {
            if (errno == EINVAL) FatalError("ToStr Error: Wrong argument at second writing to buffer.");
            FatalError("ToStr Error: Second writing to buffer failed.");
        }
        if (same_length < 0) {
            FatalError("ToStr Error: Encoding error at second writing to buffer.");
        }
        if (same_length != length) {
            FatalError("ToStr Error: Message actual length miss-match between first and second write to buffer.");
        }

        text = std::string(ext_buffer, same_length);

        delete[] ext_buffer;
    } else {
        text = std::string(buffer, length);
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
    puts(message);
    fflush(stdout);
}

#endif // TOSTR_H_
