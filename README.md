# Description
C++ library which provides simple ability to:
- convert multiple variables into string,
- convert string between utf-8 and utf-16 encodings,
- load text from a file and save text to a file.

Targeted platform: Windows.

## HOWTO: Proper clone repository with Git
Run:
```
git clone --recurse-submodules https://github.com/underwatergrasshopper/ToStr.git
```
.

## HOWTO: Run tests manually
To run all tests for Visual Studio\*, call `Test.bat`.             
To run all tests for MinGW\*\*, call `MinGW_Test.bat`.     
To run selected tests for MinGW\*\*, call `MinGW_Test.bat run <build_target> <architecture>` from `ToStr_Test` folder, where
```
<build_target>
    Release
    Debug
<architecture>
    64
    32
```
.
<sup>\*) To be able compile, add path to MSBuild bin folder in `ToStr/TestCache.bat`:
```
set MSBUILD_PATH=<path>
```
.
</sup>        
<sup>\*\*) To be able compile with 64 bit and 32 bit gcc, add paths to mingw bin folder in `ToStr/ToStr_Test/MinGW_MakeCache.bat`:
```
set MINGW32_BIN_PATH=<path>
set MINGW64_BIN_PATH=<path>
```
.
</sup>

## Builds and tests results

Compiler: **MSVC** (automated)

| Test Platform | Target Platform | Environment | Architecture | Stream Orientation \* | Build and Test |
|-|-|-|-|-|-|
| Windows Server 2022, Windows Server 2019 | Windows 10 |  Visual Studio 2022, Visual Studio  2019 | x86, x86_64 | Narrow and Wide | [![Build and Test](https://github.com/underwatergrasshopper/TrivialTestKit/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/underwatergrasshopper/ToStr/actions/workflows/build_and_test.yml) |

 Compiler: **LLVM CLang MinGW** (automated)

| Test Platform | Target Platform | Environment | Architecture | Stream Orientation \* | Build and Test |
|-|-|-|-|-|-|
| Windows Server 2022 | Windows 10 | MinGW-W64 (LLVM Clang 15.0.0) | x86, x86_64 | Narrow and Wide |  [![Build and Test (MinGW)](https://github.com/underwatergrasshopper/TrivialTestKit/actions/workflows/build_and_test_mingw.yml/badge.svg)](https://github.com/underwatergrasshopper/ToStr/actions/workflows/build_and_test_mingw.yml) |

<sup>\* Narrow for a stream when first function which operate on the stream is from `printf` family.
Wide for a stream when first function which operate on the stream is from `wprintf` family. See also documentation of `fwide`.</sup>

## Examples 
### Converting variables to a string

Converts variables to a string according to the format. Structure of the format is the same as in standard 'printf' function.

```c++
std::string text = ToStr("Some variables: %d, %.2f, %s.", 34, 3.14, "text");
```

### Converting strings between utf-8 and utf-16 encoding

Converts a string from utf-16 to utf-8 encoding.

```c++
std::string text = ToUTF8(L"Some text \u0444.");
```

Converts a string from utf-8 to utf-16 encoding.

```c++
std::wstring text = ToUTF16(u8"Some text \u0444.");
```

### Loading text from file and saving text to file

Loads text from file

```c++
std::string text = LoadTextFromFile("path\\to\\file.txt");
```

```c++
bool is_loaded;
std::string text = LoadTextFromFile("path\\to\\file.txt", &is_loaded);
```
... for utf-8 strings ...

```c++
std::string text = LoadTextFromFileUTF8(u8"path\\to\\file\u0444.txt");
```

```c++
bool is_loaded;
std::string text = LoadTextFromFile(u8"path\\to\\file\u0444.txt", &is_loaded);
```

Saves text to file

```c++
bool is_saved = SaveTextToFile("path\\to\\file.txt", "Some text.\nSome other text.");
```
... for utf-8 strings ...
```c++
bool is_saved = SaveTextToFile(u8"path\\to\\file\u0444.txt", u8"Some text \u0444.\nSome other text.");
```







