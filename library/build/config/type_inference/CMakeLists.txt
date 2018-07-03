file(GLOB header_files ./../../../include/darcel/type_inference/*.hpp)
file(GLOB source_files ./../../../source/darcel/*.cpp)

add_library(type_inference ${header_files} ${source_files})
set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
set_target_properties(type_inference PROPERTIES
  STATIC_LIBRARY_FLAGS_RELEASE "${CMAKE_LIBRARY_FLAGS}"
  OUTPUT_NAME darcel_type_inference)

install(TARGETS type_inference CONFIGURATIONS Debug
  DESTINATION $ENV{LIB_INSTALL_DIRECTORY}/debug)
install(TARGETS type_inference CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{LIB_INSTALL_DIRECTORY}/release)

file(GLOB test_source_files ./../../../source/type_inference_tester/*.cpp)
add_executable(type_inference_tester ${test_source_files})
add_custom_command(TARGET type_inference_tester POST_BUILD
  COMMAND type_inference_tester)
install(TARGETS type_inference_tester CONFIGURATIONS Debug
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/debug)
install(TARGETS type_inference_tester CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/release)
