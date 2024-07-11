cmake_minimum_required(VERSION 3.5)

project(rmw_desert)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -pthread)
endif()

find_package(ament_cmake_ros REQUIRED)

find_package(rclcpp REQUIRED)
find_package(rcutils REQUIRED)
find_package(rcpputils REQUIRED)

find_package(rmw REQUIRED)
find_package(rmw_dds_common REQUIRED)
find_package(rosidl_runtime_c REQUIRED)
find_package(rosidl_typesupport_introspection_c REQUIRED)
find_package(rosidl_typesupport_introspection_cpp REQUIRED)

find_package(std_msgs REQUIRED)

include_directories(include)

set(CBOR_ROOT cbor)
include_directories(${CBOR_ROOT}/include)

add_library(rmw_desert
  src/get_node_info_and_types.cpp
  src/init.cpp
  src/qos_profiles.cpp
  src/rmw.cpp
  src/get_topic_names_and_types.cpp
  src/dynamic_message_type_support.cpp
  src/init_options.cpp
  src/features.cpp
  src/get_topic_endpoint_info.cpp
  src/get_network_flow_endpoints.cpp
  src/event.cpp
  src/get_service_names_and_types.cpp
  src/desert_classes/DesertPublisher.cpp
  src/desert_classes/CBorStream.cpp
  src/desert_classes/CStringHelper.cpp
  src/desert_classes/TcpDaemon.cpp
  
  ${CBOR_ROOT}/src/common.c
  ${CBOR_ROOT}/src/parser.c
  ${CBOR_ROOT}/src/decoder.c
  ${CBOR_ROOT}/src/encoder.c
  ${CBOR_ROOT}/src/helper.c
  ${CBOR_ROOT}/src/ieee754.c
)

INCLUDE (FindPkgConfig)
pkg_check_modules (NDN_CXX libndn-cxx)

include_directories(rmw_desert ${NDN_CXX_INCLUDE_DIRS})
link_directories(rmw_desert ${NDN_CXX_LIBRARY_DIRS})
target_link_libraries(rmw_desert ${NDN_CXX_LIBRARIES})

#target_link_libraries(rmw_desert PUBLIC
#  rmw::rmw)

#target_link_libraries(rmw_desert PRIVATE
#  rclcpp::rclcpp
#  rcutils::rcutils
#  rcpputils::rcpputils
#  rmw_dds_common::rmw_dds_common_library
#  rosidl_typesupport_introspection_c::rosidl_typesupport_introspection_c
#  rosidl_typesupport_introspection_cpp::rosidl_typesupport_introspection_cpp
#  rosidl_runtime_c::rosidl_runtime_c
#)

# specific order: dependents before dependencies
ament_target_dependencies(rmw_desert
  rmw
  rclcpp
  rcutils
  rcpputils
  rmw_dds_common
  rosidl_typesupport_introspection_c
  rosidl_typesupport_introspection_cpp
  rosidl_runtime_c
  std_msgs
)
#ament_target_dependencies(rmw_desert std_msgs)

configure_rmw_library(rmw_desert)

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME}
PRIVATE "rmw_desert_BUILDING_LIBRARY")

# specific order: dependents before dependencies
ament_export_include_directories(include)
ament_export_libraries(rmw_desert)

ament_export_dependencies(rclcpp)
ament_export_dependencies(rcutils)
ament_export_dependencies(rcpputils)
ament_export_dependencies(rmw)
ament_export_dependencies(rosidl_runtime_c)
ament_export_dependencies(rmw_dds_common)
ament_export_dependencies(rosidl_typesupport_introspection_c)
ament_export_dependencies(rosidl_typesupport_introspection_cpp)
ament_export_dependencies(std_msgs)

register_rmw_implementation(
  "c:rosidl_typesupport_c:rosidl_typesupport_introspection_c"
  "cpp:rosidl_typesupport_cpp:rosidl_typesupport_introspection_cpp")

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()

install(
  TARGETS rmw_desert
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
