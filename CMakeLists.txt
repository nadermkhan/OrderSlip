# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(ProjectTracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Charts)

qt_standard_project_setup()

qt_add_executable(ProjectTracker
    main.cpp
)

qt_add_resources(ProjectTracker "resources"
    PREFIX "/"
    FILES
)

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
