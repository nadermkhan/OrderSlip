# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(ProjectTracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Charts)

# Enable automatic processing of Qt files
qt_standard_project_setup()

# Add executable
qt_add_executable(ProjectTracker
    main.cpp
)

# Link required Qt libraries
target_link_libraries(ProjectTracker PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
)

# Create projects.db during build
add_custom_command(TARGET ProjectTracker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/projects.db
)

# Windows specific settings
if(WIN32)
    # Set properties for Windows executable
    set_target_properties(ProjectTracker PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()