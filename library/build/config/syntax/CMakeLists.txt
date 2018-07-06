file(GLOB header_files ./../../../include/darcel/syntax_tests/*.hpp)
file(GLOB source_files ./../../../source/syntax_tests/*.cpp)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

add_executable(syntax_tests ${source_files})
set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
add_custom_command(TARGET syntax_tests POST_BUILD COMMAND syntax_tests)
install(TARGETS syntax_tests CONFIGURATIONS Debug
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/debug)
install(TARGETS syntax_tests CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/release)
