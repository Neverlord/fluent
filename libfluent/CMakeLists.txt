# ----------------------------------------------------------------------------
#                                   libfluent
# ----------------------------------------------------------------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(libfluent_sources
  src/collection.cpp
  src/context.cpp
)

set(libfluent_libs ${CAF_LIBRARIES})

add_library(libfluent SHARED ${libfluent_sources})
set_target_properties(libfluent
  PROPERTIES
  SOVERSION 1
  VERSION 1.0
  OUTPUT_NAME fluent)

target_link_libraries(libfluent ${libfluent_libs})

# Install libfluent in PREFIX/lib and headers in PREFIX/include/fluent.
install(TARGETS libfluent DESTINATION lib)
install(DIRECTORY fluent/
        DESTINATION include/fluent FILES_MATCHING PATTERN "*.hpp")

