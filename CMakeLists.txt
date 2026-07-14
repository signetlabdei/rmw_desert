cmake_minimum_required(VERSION 3.5)

project(rmw_desert)

include(config.cmake)

# Default to C++20
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -pthread)
endif()

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros_core REQUIRED)

find_package(rmw REQUIRED)
find_package(rmw_dds_common REQUIRED)
find_package(rosidl_runtime_c REQUIRED)
find_package(rosidl_typesupport_introspection_c REQUIRED)
find_package(rosidl_typesupport_introspection_cpp REQUIRED)

find_package(rcutils REQUIRED)
find_package(rcpputils REQUIRED)

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
  src/get_service_endpoint_info.cpp
  src/event.cpp
  src/get_service_names_and_types.cpp
  src/desert_classes/DesertNode.cpp
  src/desert_classes/DesertPublisher.cpp
  src/desert_classes/DesertSubscriber.cpp
  src/desert_classes/DesertClient.cpp
  src/desert_classes/DesertService.cpp
  src/desert_classes/DesertGuardCondition.cpp
  src/desert_classes/CStringHelper.cpp
  src/desert_classes/TcpDaemon.cpp
  src/desert_classes/Discovery.cpp
  src/desert_classes/TopicsConfig.cpp
  src/desert_classes/demangle.cpp
)

if (${CBOR_LIB} STREQUAL "libmcu_cbor")
  set(CBOR_ROOT cbor)
  include_directories(${CBOR_ROOT}/include)
  target_sources(rmw_desert PRIVATE
    ${CBOR_ROOT}/src/common.c
    ${CBOR_ROOT}/src/parser.c
    ${CBOR_ROOT}/src/decoder.c
    ${CBOR_ROOT}/src/encoder.c
    ${CBOR_ROOT}/src/helper.c
    ${CBOR_ROOT}/src/ieee754.c
    src/desert_classes/LibMcuCBorStream.cpp
  )
  target_compile_definitions(rmw_desert PRIVATE LIBMCU_CBOR_ENABLED)
elseif (${CBOR_LIB} STREQUAL "NanoCBOR")
  set(CBOR_ROOT thirdparty/NanoCBOR)
  include_directories(${CBOR_ROOT}/include)
  target_sources(rmw_desert PRIVATE
    ${CBOR_ROOT}/src/decoder.c
    ${CBOR_ROOT}/src/encoder.c
    src/desert_classes/NanoCBorStream.cpp
  )
  target_compile_definitions(rmw_desert PRIVATE NANOCBOR_ENABLED)
else ()
  message(FATAL_ERROR "At least one CBOR libraty is required")
endif ()

target_include_directories(rmw_desert PRIVATE ${CBOR_INC_DIR})

if (SECURE_MODE_ENABLED)
  if (NOT ${CBOR_LIB} STREQUAL "NanoCBOR")
    message(FATAL_ERROR "Secure mode requires NanoCBOR lib.")
  endif ()

  add_compile_options(-fPIC)

  set(COSE_ROOT "thirdparty/libcose")
  set(COSE_LIB "libcose")
  set(COSE_INC_DIR "${COSE_ROOT}/include")
  add_library(${COSE_LIB} OBJECT
    ${COSE_ROOT}/src/cose_common.c
    ${COSE_ROOT}/src/cose_crypto.c
    ${COSE_ROOT}/src/cose_encrypt.c
    ${COSE_ROOT}/src/cose_hdr.c
    ${COSE_ROOT}/src/cose_hkdf.c
    ${COSE_ROOT}/src/cose_key.c
    ${COSE_ROOT}/src/cose_recipient.c
    ${COSE_ROOT}/src/cose_sign.c
    ${COSE_ROOT}/src/cose_signature.c
    ${COSE_ROOT}/src/crypt/keygen_symm.c
  )
  target_include_directories(${COSE_LIB} PRIVATE ${COSE_INC_DIR} ${CBOR_INC_DIR})

  if (${CRYPTO_LIB} STREQUAL "mbedtls")
    add_subdirectory(thirdparty/mbedtls)

    target_sources(${COSE_LIB} PRIVATE ${COSE_ROOT}/src/crypt/mbedtls.c)
    target_link_libraries(${COSE_LIB} PRIVATE mbedtls)
    target_link_libraries(rmw_desert PRIVATE mbedtls)
    add_compile_definitions(CRYPTO_MBEDTLS)
  elseif (${CRYPTO_LIB} STREQUAL "ascon-c")
    set(ASCON_LIB_NAME "ascon-c")
    set(ASCON_AEAD_ROOT thirdparty/ascon-c/crypto_aead/asconaead128/opt64)
    set(ASCON_HASH_ROOT thirdparty/ascon-c/crypto_hash/asconhash256/opt64)
    add_library(${ASCON_LIB_NAME} OBJECT
            ${ASCON_AEAD_ROOT}/aead.c
            ${ASCON_AEAD_ROOT}/permutations.c
            ${ASCON_AEAD_ROOT}/printstate.c
            ${ASCON_HASH_ROOT}/hash.c
    )
    target_compile_definitions(${ASCON_LIB_NAME} PRIVATE ASCON_INLINE_MODE=0)

    target_sources(${COSE_LIB} PRIVATE ${COSE_ROOT}/src/crypt/ascon_c.c)
    add_compile_definitions(CRYPTO_ASCON)
  else ()
    message(FATAL_ERROR "Cryptolib ${CRYPTO_LIB} is not supported")
  endif ()

  target_sources(rmw_desert PRIVATE
    $<TARGET_OBJECTS:${COSE_LIB}>
    src/desert_classes/security/sec_utils.cpp
    src/desert_classes/security/AeadParams.cpp
    src/desert_classes/security/SecurityLayer.cpp
  )
  target_include_directories(rmw_desert PRIVATE ${COSE_INC_DIR})
  target_compile_definitions(rmw_desert PRIVATE SECURE_MODE_ENABLED)
  target_compile_definitions(rmw_desert PRIVATE
          AEAD_ALGO="${AEAD_ALGO}"
          KDF_ALGO="${KDF_ALGO}"
          PIV_LEN=${PIV_LEN}
          SENDER_ID="${SENDER_ID}"
          RECEIVER_ID="${RECEIVER_ID}")
endif ()

INCLUDE (FindPkgConfig)

# specific order: dependents before dependencies
target_link_libraries(rmw_desert PUBLIC
  rmw::rmw
  rcutils::rcutils
  rcpputils::rcpputils
  rmw_dds_common::rmw_dds_common_library
  rosidl_typesupport_introspection_c::rosidl_typesupport_introspection_c
  rosidl_typesupport_introspection_cpp::rosidl_typesupport_introspection_cpp
  rosidl_runtime_c::rosidl_runtime_c
)

configure_rmw_library(rmw_desert)

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME}
PRIVATE "rmw_desert_BUILDING_LIBRARY")

# specific order: dependents before dependencies
ament_export_libraries(rmw_desert)

ament_export_dependencies(
  rcutils
  rcpputils
  rmw
  rosidl_runtime_c
  rmw_dds_common
  rosidl_typesupport_introspection_c
  rosidl_typesupport_introspection_cpp
)

register_rmw_implementation(
  "c:rosidl_typesupport_c:rosidl_typesupport_introspection_c"
  "cpp:rosidl_typesupport_cpp:rosidl_typesupport_introspection_cpp")

ament_package()

install(
  TARGETS rmw_desert
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
