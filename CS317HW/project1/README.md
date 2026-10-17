>**Author**: Daschle Newberry
>**Email**: daschle.newberry@wsu.edu

## Project Structure
---
```
.
├── CMakeLists.txt -- CMake config for building project
├── DESIGN.md      -- Document used for planning
├── README.md      -- Basic README
├── src
│   ├── main.cpp   -- Input parsing logic
│   ├── NFA.cpp    -- NFA implementation
│   └── NFA.h      -- NFA header
```

## Build
---
### Prerequisites
- **CMake** 3.23+
- **GCC** 15+

### 1. Generate MakeFiles
```shell
cmake -B build
```
### 2. Build the project
```shell
cmake --build build
```
### 3. Run the project
```shell
./build/REtoNFA ...
```


### Notes
---
This project takes advantage of modern-ish C++ features (std::string::contains, <=> operator) so a compiler supporting parts of C++ 23 is required. GCC on the WSUV UX lab machines is 15+, so the project was tested and compiled there.