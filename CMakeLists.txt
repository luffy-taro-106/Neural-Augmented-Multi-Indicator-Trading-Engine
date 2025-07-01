cmake_minimum_required(VERSION 3.14)
project(TradingStrategies)

set(CMAKE_CXX_STANDARD 17)

# Add pybind11 manually
include_directories("C:/Users/Darshan/anaconda3/Lib/site-packages/pybind11/include")

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)


add_library(bindings MODULE
    src/cpp/bindings.cpp
    src/cpp/macd_strategy.cpp
    src/cpp/rsi_strategy.cpp
    src/cpp/supertrend_strategy.cpp
)

target_include_directories(bindings PRIVATE src/cpp)
target_link_libraries(bindings PRIVATE Python3::Python)
