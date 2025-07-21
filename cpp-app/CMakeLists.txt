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
    src/OllamaClient.cpp
)

# Header files
set(HEADERS
    include/MainWindow.h
    include/LandingPage.h
    include/SecurityChecker.h
    include/VulnerabilityManager.h
    include/SystemChecker.h
    include/VulnerabilityDefinition.h
    include/OllamaClient.h
)

# Create executable
add_executable(SecurityChecker ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(SecurityChecker Qt6::Core Qt6::Widgets Qt6::Network)

# Copy vulnerabilities.json to build directory
configure_file(${CMAKE_SOURCE_DIR}/data/vulnerabilities.json ${CMAKE_BINARY_DIR}/vulnerabilities.json COPYONLY)

# Install rules for packaging
install(TARGETS SecurityChecker
    RUNTIME DESTINATION bin
)

install(FILES ${CMAKE_SOURCE_DIR}/data/vulnerabilities.json
    DESTINATION bin
)

install(FILES ${CMAKE_SOURCE_DIR}/README.md
    DESTINATION .
)

# Set executable properties for Windows
if(WIN32)
    set_target_properties(SecurityChecker PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Windows-specific install
    install(FILES ${CMAKE_BINARY_DIR}/vulnerabilities.json
        DESTINATION .
    )
endif()

# Set version information
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_NAME "SecurityChecker")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Ferramenta open-source para verificação de vulnerabilidades")
set(CPACK_PACKAGE_VENDOR "OpenSource Security Tools")
set(CPACK_PACKAGE_CONTACT "security@example.com")

# Platform-specific packaging
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "SecurityChecker")
    set(CPACK_NSIS_PACKAGE_NAME "SecurityChecker")
    set(CPACK_NSIS_CONTACT "security@example.com")
    set(CPACK_NSIS_HELP_LINK "https://github.com/jeanccoelho/secure-check")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "SecurityChecker")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "security@example.com")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Ferramenta open-source para verificação e correção de vulnerabilidades de segurança")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)