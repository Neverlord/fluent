cmake_minimum_required(VERSION 2.8)
project(fluent_examples CXX)

add_custom_target(all_examples)

include_directories(${LIBCAF_INCLUDE_DIRS})

macro(add folder name)
  add_executable(${name} ${folder}/${name}.cpp ${ARGN})
  target_link_libraries(${name}
                        ${LD_FLAGS}
                        ${CAF_LIBRARIES}
                        ${FLUENT_LIBRARIES}
                        ${PTHREAD_LIBRARIES}
                        ${WSLIB})
  install(FILES ${folder}/${name}.cpp DESTINATION share/fluent/examples/${folder})
  add_dependencies(${name} all_examples)
endmacro()

# introductionary applications
add(. reliable_unicast)

