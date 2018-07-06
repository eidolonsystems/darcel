file(GLOB source_files ./../../../source/data_types_tests/*.cpp)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

add_executable(data_types_tests ${source_files})
add_custom_command(TARGET data_types_tests
  POST_BUILD COMMAND data_types_tests)
install(TARGETS data_types_tests CONFIGURATIONS Debug
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/debug)
install(TARGETS data_types_tests CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/release)
