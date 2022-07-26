# CMake build file for the SJSL Unit tests

project(SJSL_UnitTests)

# include the catch2 library from github
Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)

# add catch2 lib as dependecy
FetchContent_MakeAvailable(Catch2)
add_executable(UnitTests UnitTests.cpp)
target_link_libraries(UnitTests PRIVATE SJSLib Catch2::Catch2WithMain)

# Add Textconverter Lib as dependency
target_include_directories(UnitTests PUBLIC ${ToolLibraryIncDir})
target_link_libraries(UnitTests PUBLIC SJSLib)

add_custom_target(UnitTestsRun
  COMMAND UnitTests -d yes
  COMMENT "Running SJSL Tests"
)
