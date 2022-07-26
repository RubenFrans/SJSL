# CMake build file for the SJSL library

project(SJSL_LIB)

# Add library build target with sources
add_library(SJSLib 
 "JobSystem.h" "JobSystem.cpp" "WorkerThread.h" "WorkerThread.cpp" "Job.h" "Job.cpp")

# Set language standard
set_property(TARGET SJSLib PROPERTY CXX_STANDARD 17)