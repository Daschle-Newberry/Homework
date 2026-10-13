cmake_minimum_required(VERSION 3.23)
project(REtoNFA LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STAND_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS 
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

list(REMOVE_ITEM LIB_SOURCES, "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(REtoNFALib STATIC ${LIB_SOURCES})
target_include_directories(REtoNFALib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR/src}")
target_compile_options(REtoNFALib PUBLIC 
  -Wall 
  -Wextra 
  -g 
  -fno-omit-frame-pointer 
  -fsanitize=address,undefined 
  -fanalyzer
)

target_link_options(REtoNFALib PUBLIC 
  -fsanitize=address,undefined 
  -fanalyzer
)
add_executable(REtoNFA src/main.cpp)
target_link_libraries(REtoNFA PRIVATE REtoNFALib)

#enable_testing()
#add_subdirectory(tests)



