cmake_minimum_required (VERSION 2.8)
project (rpi-dnp3-gpio)
set(RPI_DNP3_GPIO_VERSION 1.0.0)

# other options off-by-default that you can enable
option(WERROR "Set all warnings to errors" OFF)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
# different release and debug flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
  
# ---- executable ----
add_executable(rpi-dnp3-gpio src/main.cpp deps/inih/ini.c deps/inih/ini.h)
target_link_libraries (rpi-dnp3-gpio)

