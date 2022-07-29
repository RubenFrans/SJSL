/*
* SJSL Unit tests
* This file is used to test the functionality and maintain the stability of the SJSL library
*/

#define CATCH_CONFIG_MAIN
#include "catch2\catch_all.hpp"
#include "JobSystem.h"
#include "Job.h"
#include <iostream>
#include <thread>
#include <chrono>

TEST_CASE("Catch2 functional test") {
	REQUIRE(1 == 1);
}


void PrintNumber(int number) {

	std::cout << "Thread " << std::this_thread::get_id() << ": " << number << std::endl;
	std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000));
}

// Not sure yet on how to take on testing multithreaded code
TEST_CASE("Scheduling jobs") {

	SJSL::JobSystem js{};
	js.Schedule([] { PrintNumber(1); });

}

TEST_CASE("Correct default amount of WorkerThread initialization") {
	
	SJSL::JobSystem js{};
	REQUIRE(js.GetAmountOfWorkerThreads() == std::thread::hardware_concurrency() * 2);
}

TEST_CASE("Correct custom amount of Workerthread initialization") {

	uint32_t amountOfWorkerThreads{ 10 };
	SJSL::JobSystem js{ amountOfWorkerThreads, false };
	REQUIRE(js.GetAmountOfWorkerThreads() == amountOfWorkerThreads);
}