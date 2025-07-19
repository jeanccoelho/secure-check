cmake_minimum_required(VERSION 3.16)
project(SecurityChecker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Enable automatic MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/LandingPage.cpp
    src/SecurityChecker.cpp
    src/VulnerabilityManager.cpp
    src/SystemChecker.cpp
)

# Header files
set(HEADERS
    include/MainWindow.h
    include/LandingPage.h
    include/SecurityChecker.h
    include/VulnerabilityManager.h
    include/SystemChecker.h
    include/VulnerabilityDefinition.h
)

# Create executable
add_executable(SecurityChecker ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(SecurityChecker Qt6::Core Qt6::Widgets Qt6::Network)

# Copy vulnerabilities.json to build directory
configure_file(${CMAKE_SOURCE_DIR}/data/vulnerabilities.json ${CMAKE_BINARY_DIR}/vulnerabilities.json COPYONLY)

# Set executable properties for Windows
if(WIN32)
    set_target_properties(SecurityChecker PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()