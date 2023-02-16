cmake_minimum_required(VERSION 2.8.3)
project(remote_manager)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  std_msgs
  project11_msgs
  roscpp
  udp_bridge
)

catkin_package(
  CATKIN_DEPENDS std_msgs project11_msgs roscpp udp_bridge
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}_node src/remote_manager_node.cpp)

add_dependencies(${PROJECT_NAME}_node ${catkin_EXPORTED_TARGETS})

target_link_libraries(${PROJECT_NAME}_node
    ${catkin_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME}_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

