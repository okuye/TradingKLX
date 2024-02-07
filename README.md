# KLX Trading C++ Project

## Description
This project does amazing things with C++.

## Buiding
1. Remove the buid directory if it exits and create one and cd into it  `A script should exist for this`
2. Compile using `cmake .. -G "Ninja" -DCMAKE_C_COMPILER=C:/cygwin64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/cygwin64/bin/g++.exe`.

## On Windows:
Using Cygwin’s GCC and G++:

Your current approach of using Cygwin’s GCC and G++ compilers with CMake is valid, especially if your project or its dependencies are Unix/Linux-centric.
The command you provided explicitly specifies the compilers to CMake, which is necessary when not using the default compiler for the platform (like MSVC on Windows).
cmake .. -G "Ninja" -DCMAKE_C_COMPILER=C:/cygwin64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/cygwin64/bin/g++.exe

## On macOS:
Clang Compilers:

macOS comes with Clang compilers installed by default, which are highly compatible with GCC.
For most projects, simply running the following command is sufficient:
cmake .. -G "Ninja"

CMake will automatically pick up the default system compilers (Clang).



## Usage
1. Clone the repository.
2. Compile using `g++ main.cpp -o myprogram`.
3. Run `./myprogram`.

