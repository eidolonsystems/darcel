file(GLOB header_files ./../../../include/darcel/data_types/*.hpp)
file(GLOB source_files ./../../../source/darcel/*.cpp)

add_library(data_types ${header_files} ${source_files})
set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
set_target_properties(data_types PROPERTIES
  STATIC_LIBRARY_FLAGS_RELEASE "${CMAKE_LIBRARY_FLAGS}"
  OUTPUT_NAME darcel_data_types)

install(TARGETS data_types CONFIGURATIONS Debug
  DESTINATION $ENV{LIB_INSTALL_DIRECTORY}/debug)
install(TARGETS data_types CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{LIB_INSTALL_DIRECTORY}/release)

file(GLOB test_header_files ./../../../include/darcel/data_types_tester/*.hpp)
file(GLOB test_source_files ./../../../source/data_types_tester/*.cpp)
add_executable(data_types_tester ${test_header_files} ${test_source_files})
set_source_files_properties(${test_header_files}
  PROPERTIES HEADER_FILE_ONLY TRUE)
add_custom_command(TARGET data_types_tester POST_BUILD
  COMMAND data_types_tester)
install(TARGETS data_types_tester CONFIGURATIONS Debug
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/debug)
install(TARGETS data_types_tester CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/release)
