file(GLOB header_files ./../../../include/darcel/lexicon/*.hpp)
file(GLOB source_files ./../../../source/darcel/*.cpp)

add_library(lexicon ${header_files} ${source_files})
set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
set_target_properties(lexicon PROPERTIES
  STATIC_LIBRARY_FLAGS_RELEASE "${CMAKE_LIBRARY_FLAGS}"
  OUTPUT_NAME darcel_lexicon)

install(TARGETS lexicon CONFIGURATIONS Debug
  DESTINATION $ENV{LIB_INSTALL_DIRECTORY}/debug)
install(TARGETS lexicon CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{LIB_INSTALL_DIRECTORY}/release)

file(GLOB test_header_files ./../../../include/darcel/lexicon_tester/*.hpp)
file(GLOB test_source_files ./../../../source/lexicon_tester/*.cpp)
add_executable(lexicon_tester ${test_header_files} ${test_source_files})
set_source_files_properties(${test_header_files}
  PROPERTIES HEADER_FILE_ONLY TRUE)
add_custom_command(TARGET lexicon_tester POST_BUILD
  COMMAND lexicon_tester)
install(TARGETS lexicon_tester CONFIGURATIONS Debug
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/debug)
install(TARGETS lexicon_tester CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/release)
