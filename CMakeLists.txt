cmake_minimum_required(VERSION 3.16)
project(radar-sim-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(Eigen3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW3 REQUIRED fftw3)

# Main executable
add_executable(radar_sim
    src/main.cpp
    src/radar_params.cpp
    src/chirp.cpp
    src/simulate.cpp
    src/matched_filter.cpp
    src/doppler.cpp
    src/cfar.cpp
    src/export_csv.cpp
)

target_include_directories(radar_sim PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${FFTW3_INCLUDE_DIRS}
)

target_link_libraries(radar_sim PRIVATE
    Eigen3::Eigen
    ${FFTW3_LIBRARIES}
    fftw3
)
