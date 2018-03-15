file(GLOB header_files
  ./../../../include/darcel/*.hpp)
file(GLOB source_files
  ./../../../source/*.cpp)

add_executable(darcel ${header_files} ${source_files})

set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)

install(TARGETS darcel DESTINATION $ENV{INSTALL_DIRECTORY})
