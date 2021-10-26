# set the project name
project(grpc_iface_demo)

# app
find_package(Qt5Core)
add_executable(app main.cpp)
target_link_libraries(app Qt5::Core)
