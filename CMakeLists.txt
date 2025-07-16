cmake_minimum_required(VERSION 3.8)
project(lidar_imu_init)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(-g)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-std=c++17)
set(CMAKE_CXX_FLAGS "-std=c++17 -O3")

add_definitions(-DROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/\")
if(ISAAC_SIM)
  add_compile_definitions(ISAAC_SIM)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message("Current CPU architecture: ${CMAKE_SYSTEM_PROCESSOR}")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
  include(ProcessorCount)
  ProcessorCount(N)
  message("Processor number: ${N}")

  if(N GREATER 4)
    add_definitions(-DMP_EN)
    add_definitions(-DMP_PROC_NUM=3)
  elseif(N GREATER 3)
    add_definitions(-DMP_EN)
    add_definitions(-DMP_PROC_NUM=2)
  else()
    add_definitions(-DMP_PROC_NUM=1)
  endif()
else()
  add_definitions(-DMP_PROC_NUM=1)
endif()

find_package(OpenMP QUIET)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

# ROS dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(spdlog REQUIRED)
find_package(OpenCV REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(Ceres REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED COMPONENTS common io)
if (USE_LIVOX)
  find_package(livox_ros_driver2 REQUIRED)
endif()

set(dependencies
  rclcpp
  rclcpp_components
  geometry_msgs
  nav_msgs
  sensor_msgs
  std_msgs
  std_srvs
  visualization_msgs
  pcl_ros
  pcl_conversions
  spdlog
  OpenCV
  tf2
  tf2_ros
  Eigen3
  ament_index_cpp
  PCL
  Ceres
  tf2_eigen
  tf2_geometry_msgs
  rosidl_default_runtime
)

if (USE_LIVOX)
  list(APPEND dependencies livox_ros_driver2)
endif()

set(msg_files
  "msg/Pose6D.msg"
  "msg/States.msg"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES
  std_msgs
  geometry_msgs
)

rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME}
  "rosidl_typesupport_cpp"
)

ament_export_dependencies(rosidl_default_runtime)

add_executable(li_init
  src/laserMapping.cpp 
  include/ikd-Tree/ikd_Tree.cpp 
  include/LI_init/LI_init.cpp 
  include/time_utils.cpp
  src/preprocess.cpp
)

target_include_directories(li_init PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>
  ${PCL_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${PYTHON_NumPy_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${ament_index_cpp_INCLUDE_DIRS}
)

target_link_libraries(li_init
  ${PCL_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${CERES_LIBRARIES}
  Eigen3::Eigen
  spdlog::spdlog
  Python3::Python
  Python3::NumPy
  ${OpenCV_LIBS}
  ${cpp_typesupport_target}
)

ament_target_dependencies(li_init ${dependencies})

# ---------------- Install --------------- #
install(TARGETS li_init
 DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY config launch rviz 
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
