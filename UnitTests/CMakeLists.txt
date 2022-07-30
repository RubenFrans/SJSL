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
add_executable(UnitTests "unittest_JobSystem.cpp" "unittest_WorkerThread.cpp")
target_link_libraries(UnitTests PRIVATE SJSLib Catch2::Catch2WithMain)

# Add Textconverter Lib as dependency
target_include_directories(UnitTests PUBLIC ${SJSLIncDir})
target_link_libraries(UnitTests PUBLIC SJSLib)

target_include_directories(UnitTests PUBLIC "${CMAKE_SOURCE_DIR}/include/vld")

set(VLD_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/vld/x64/vld.lib")

target_link_libraries(UnitTests PUBLIC ${VLD_LIBRARIES})

add_custom_target(UnitTestsRun
  COMMAND UnitTests -d yes
  COMMENT "Running SJSL Tests"
)
