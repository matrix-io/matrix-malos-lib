cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11)

include (../cmake/FatalWarnings.cmake)
ADM_EXTRA_WARNINGS()

find_package(Protobuf 3 REQUIRED)
find_package(Threads REQUIRED)

# This won't find ZQM in custom locations.
# FIXME(nelson.castillo): Improve.
FIND_LIBRARY(ZMQ_LIB NAMES zmq)

message(STATUS "ZMQ found => " "${ZMQ_LIB}")

PROTOBUF_GENERATE_CPP(MALOS_PROTO_SRC MALOS_PROTO_HEADER ../protocol-buffers/malos/driver.proto)

include_directories(
  ${PROJECT_BINARY_DIR} # Protos.
  ${PROTOBUF_INCLUDE_DIRS})

set(matrix_malos_zmq_src
  zmq_push.h
  zmq_push.cpp
  zmq_pull.h
  zmq_pull.cpp
)

add_library(matrix_malos_zmq ${matrix_malos_zmq_src})
set_property(TARGET matrix_malos_zmq PROPERTY CXX_STANDARD 11)
target_link_libraries(matrix_malos_zmq ${ZMQ_LIB} ${CMAKE_THREAD_LIBS_INIT})

set(matrix_malos_src
  malos_base.cpp malos_base.h
  driver_manager.cpp driver_manager.h
  ${MALOS_PROTO_SRC} ${MALOS_PROTO_HEADER}
)

add_library(matrix_malos ${matrix_malos_src})
set_property(TARGET matrix_malos PROPERTY CXX_STANDARD 11)
target_link_libraries(matrix_malos matrix_malos_zmq)
target_link_libraries(matrix_malos ${PROTOBUF_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

# Binary to be installed.
install(TARGETS matrix_malos_zmq DESTINATION lib)
install(TARGETS matrix_malos DESTINATION lib)

set (matrix_malos_headers
  malos_base.h
  driver_manager.h
  zmq_pull.h
  zmq_push.h
)

install (FILES ${matrix_malos_headers} DESTINATION include/matrix_malos)
install (FILES third_party/zmq.hpp  DESTINATION include/matrix_malos/third_party)

