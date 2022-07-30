/*
* SJSL::JobSystem Unit tests
* This file is used to test the functionality and maintain the stability of the SJSL library
* This file tests the interface of the SJSL::JobSystem class
*/

#define CATCH_CONFIG_MAIN
#include "catch2\catch_all.hpp"
#include "JobSystem.h"
#include <iostream>
#include <thread>
#include <chrono>

void PrintNumber(int number) {

	std::cout << "Thread " << std::this_thread::get_id() << ": " << number << std::endl;
	std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000));
}

TEST_CASE("Initializing the job system") {

	SJSL::JobSystem js{};
	//js.Schedule([] { PrintNumber(1); });

	SJSL::Job job{ [] { PrintNumber(1); } };

	js.Schedule(job);
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


TEST_CASE("Waiting on a specific job") {

	SJSL::JobSystem js{};
	SJSL::Job job{ []() { PrintNumber(1); } };
	js.Schedule(job);
	std::cout << "Waiting for job to finish" << std::endl;
	//job.Join();

}