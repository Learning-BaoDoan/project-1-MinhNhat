# Project-2-MinhNhat

Program that converts binary weather data files to JSON format.

## Overview

This project reads binary weather sensor data and converts it to structured JSON output. The binary format contains sensor readings including temperature, humidity, pressure, GPS coordinates, wind data, and more.

## Prerequisites

To build and run this project, you need:

1. **C Compiler**: [MinGW](https://sourceforge.net/projects/mingw/) (Windows) or GCC (Linux/macOS)
2. **CMake**: [Download here](https://cmake.org/download/) (version 3.12.4 or higher)
3. **Operating System**: Windows, Linux, or macOS

## Project Structure

```
project-2-MinhNhat/
├── CMakeLists.txt          # Build configuration
├── README.md              # This file
├── include/               # Header files
│   ├── binary_io.h        # Binary I/O functions
│   ├── weather_types.h    # Data structures & constants
│   ├── weather_parser.h   # Main parser functions
│   └── json_writer.h      # JSON output functions
├── src/                   # Source files
│   ├── binary_io.c        # Binary I/O implementation
│   ├── weather_parser.c   # Parser implementation
│   ├── json_writer.c      # JSON writer implementation
│   └── main.c             # Main entry point
├── bin/                   # Executable output (created by cmake)
├── lib/                   # Library output (created by cmake)
├── data/                  # Default output directory
└── sample_data/           # Sample input files (if any)
```

## Binary File Format

The input binary file follows this structure:

**Header (10 bytes):**
- 4 bytes: File ID
- 2 bytes: File version
- 4 bytes: Number of records

**Each Record (57 bytes):**
- 4 bytes: Sensor ID
- 1 byte: Battery status (0=normal, 1=low, 2=emergency)
- 4 bytes: Timestamp (UNIX)
- 8 bytes: Latitude (GPS)
- 8 bytes: Longitude (GPS)
- 4 bytes: Temperature
- 4 bytes: Humidity
- 4 bytes: Pressure
- 2 bytes: CO2 concentration
- 4 bytes: Wind speed
- 2 bytes: Wind direction
- 4 bytes: Rainfall
- 4 bytes: UV index
- 4 bytes: Light intensity

## Building the Project

### Windows (MinGW)

1. **Clean previous builds** (optional):
   ```powershell
   # Delete build contents
   Get-ChildItem -Path build -Recurse -Force -ErrorAction Ignore | Remove-Item -Recurse -Force -ErrorAction Ignore
   
   # Delete bin contents
   Get-ChildItem bin -Force | Remove-Item -Recurse -Force -ErrorAction Ignore
   ```

2. **Configure and build**:
   ```powershell
   # Configure CMake
   cmake -G "MinGW Makefiles" -B build .
   
   # Build the project
   mingw32-make -C build -j20
   ```

### Linux/macOS

1. **Clean and build**:
   ```bash
   # Create build directory
   mkdir -p build
   cd build
   
   # Configure and build
   cmake ..
   make -j$(nproc)
   cd ..
   ```

### Alternative Build Commands

```bash
# Using CMake directly (cross-platform)
cmake -B build
cmake --build build --parallel

# Create directories first if needed
mkdir -p bin data
```

## Running the Program

### Basic Usage

```bash
# Windows
./bin/weather_parser.exe

# Linux/macOS
./bin/weather_parser
```

### Command Line Arguments

```bash
# Use default files (weather_data.bin -> data/weather_data.json)
./bin/weather_parser

# Custom input file, default output
./bin/weather_parser my_data.bin

# Custom input and output files
./bin/weather_parser input/sensor_data.bin output/results.json

# Show help
./bin/weather_parser --help
```

### Using Make Targets

```bash
# Run with default files
make -C build run

# Run with sample data (if available)
make -C build run_sample
```

## Sample Input/Output

### Input Binary File
- `weather_data.bin` - Binary weather data file

### Output JSON Structure
```json
{
  "metadata": {
    "file_id": "WTHR",
    "version": 1,
    "record_count": 100
  },
  "records": [
    {
      "sensor_id": 12345,
      "battery": "normal",
      "timestamp": 1704067200,
      "location": {
        "lat": 10.7769100,
        "lon": 106.7009100
      },
      "measurements": {
        "temperature": 28.50,
        "humidity": 75.20,
        "pressure": 1013.25,
        "co2": 400,
        "wind": {
          "speed": 5.30,
          "direction": 180
        },
        "rain": 0.00,
        "uv": 7.50,
        "light": 45000.00
      }
    }
  ]
}
```

## Troubleshooting

### Common Issues

1. **MinGW not found**:
   - Make sure MinGW is installed and added to PATH
   - Try: `gcc --version` to verify installation

2. **CMake not found**:
   - Install CMake and add to PATH
   - Try: `cmake --version`

3. **Permission denied**:
   - Run terminal as administrator (Windows)
   - Check file permissions (Linux/macOS)

4. **File not found**:
   - Make sure input binary file exists
   - Check file path and spelling

### Build Errors

```bash
# If build fails, try cleaning and rebuilding
rm -rf build bin lib
mkdir build
cmake -B build
cmake --build build
```

## Development

### Adding New Features

1. **New data types**: Modify `include/weather_types.h`
2. **New binary formats**: Update `src/binary_io.c`
3. **New output formats**: Create new writer in `src/`
4. **New parsers**: Extend `src/weather_parser.c`

### Testing

```bash
# Build in debug mode
cmake -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build

# Run with verbose output
./bin/weather_parser -v input.bin output.json
```

## License

See project root for details.
