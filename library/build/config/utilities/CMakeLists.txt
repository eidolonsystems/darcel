file(GLOB header_files ./../../../include/darcel/utilities/*.hpp)
file(GLOB source_files ./../../../source/darcel/*.cpp)

add_library(utilities ${header_files} ${source_files})
set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
set_target_properties(utilities PROPERTIES
  STATIC_LIBRARY_FLAGS_RELEASE "${CMAKE_LIBRARY_FLAGS}"
  OUTPUT_NAME darcel_utilities)

install(TARGETS utilities CONFIGURATIONS Debug
  DESTINATION $ENV{LIB_INSTALL_DIRECTORY}/debug)
install(TARGETS utilities CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{LIB_INSTALL_DIRECTORY}/release)

file(GLOB test_header_files ./../../../include/darcel/utilities_tester/*.hpp)
file(GLOB test_source_files ./../../../source/utilities_tester/*.cpp)
add_executable(utilities_tester ${test_header_files} ${test_source_files})
set_source_files_properties(${test_header_files}
  PROPERTIES HEADER_FILE_ONLY TRUE)
add_custom_command(TARGET utilities_tester POST_BUILD COMMAND utilities_tester)
install(TARGETS utilities_tester CONFIGURATIONS Debug
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/debug)
install(TARGETS utilities_tester CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/release)
