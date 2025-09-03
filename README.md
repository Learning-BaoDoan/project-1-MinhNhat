# Project_1_MinhNhat

This is my project

## Overview

Parse JSON string data using the cJSON library.

## Prerequisites

To build and run this project, you need:

1. C Compiler [MinGW](https://sourceforge.net/projects/mingw/)
2. [CMake](https://cmake.org/download/)
3. Windows operating system (based on the includes used in the code)

## Project Structure

'''
project-1-MinhNhat/
|   .gitignore
|   CMakeLists.txt
|   main.c
|   README.md
|   toolchain_win.cmake
|
+---bin
+---build
+---data
|       data.json
|
+---inc
|       cJSON.h
|       cJSON_Utils.h
|
+---lib
|   +---shared
|   \---static
\---src
        cJSON.c
        cJSON_Utils.c
'''

## Building the Project

To build the project on Windows:

1. Configure CMake:

```powershell
project-1-MinhNhat
cmake -G "MinGW Makefiles" -D CMAKE_TOOLCHAIN_FILE=../toolchain_win.cmake -B build .
```

2. Build the project:

```powershell
mingw32-make -C build -j20
```

## Running the Program

After building, you can find the executable in the `bin` directory. Run it using:

```powershell
./bin/project_1.exe 
```

The program will output: "main example"

## Others

1. Delete everything inside ./build but keep the build folder itself, ignore errors if it doesn’t exist
```powershell
Get-ChildItem -Path build -Recurse -Force -ErrorAction Ignore | Remove-Item -Recurse -Force -ErrorAction Ignore
```

2. Delete everything inside ./bin but keep the bin folder itself
```powershell
Get-ChildItem bin -Force | Remove-Item -Recurse -Force -ErrorAction Ignore
```