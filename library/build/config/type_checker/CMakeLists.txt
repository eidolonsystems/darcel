file(GLOB source_files ./../../../source/type_checker_tests/*.cpp)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

add_executable(type_checker_tests ${source_files})
add_custom_command(TARGET type_checker_tests
  POST_BUILD COMMAND type_checker_tests)
install(TARGETS type_checker_tests CONFIGURATIONS Debug
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/debug)
install(TARGETS type_checker_tests CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/release)
