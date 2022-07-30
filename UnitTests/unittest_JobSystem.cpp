/*
* SJSL::JobSystem Unit tests
* This file is used to test the functionality and maintain the stability of the SJSL library
* This file tests the interface of the SJSL::JobSystem class
*/

#define CATCH_CONFIG_MAIN
#include "catch2\catch_all.hpp"
//#include "vld.h"

#define _CRTDBG_MAP_ALLOC

#include "JobSystem.h"
#include <iostream>
#include <thread>
#include <chrono>


void PrintNumber(int number) {
	std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000));
	std::cout << "Thread " << std::this_thread::get_id() << ": " << number << std::endl;
	_CrtDumpMemoryLeaks();
}

TEST_CASE("Correct default amount of WorkerThread initialization") {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	SJSL::JobSystem js{};
	REQUIRE(js.GetAmountOfWorkerThreads() == std::thread::hardware_concurrency() * 2);
	_CrtDumpMemoryLeaks();
}

TEST_CASE("Correct custom amount of Workerthread initialization") {

	uint32_t amountOfWorkerThreads{ 10 };
	SJSL::JobSystem js{ amountOfWorkerThreads, false };
	REQUIRE(js.GetAmountOfWorkerThreads() == amountOfWorkerThreads);
	_CrtDumpMemoryLeaks();
}


TEST_CASE("Scheduling and waiting for a job") {

	SJSL::JobSystem js{};
	SJSL::Job* job{ new SJSL::Job{ [] { PrintNumber(1); }, false } };

	js.Schedule(job);

	std::cout << "Waiting for job to finish" << std::endl;
	job->Join();
	std::cout << "Job Finished" << std::endl;
	delete job;
	_CrtDumpMemoryLeaks();

}

TEST_CASE("Scheduling and waiting for multiple jobs") {

	SJSL::JobSystem js{};
	SJSL::Job* job{ new SJSL::Job{ [] { PrintNumber(1); }, false } };
	SJSL::Job* job1{ new SJSL::Job{ [] { PrintNumber(2); } , false } };
	SJSL::Job* job2{ new SJSL::Job{ [] { PrintNumber(3); } , false } };

	js.Schedule(job);
	js.Schedule(job1);
	js.Schedule(job2);

	std::cout << "Waiting for jobs to finish" << std::endl;
	job->Join();
	job1->Join();
	job2->Join();
	std::cout << "Jobs Finished" << std::endl;
	delete job;
	delete job1;
	delete job2; 
	_CrtDumpMemoryLeaks();
}


TEST_CASE("Scheduling detached jobs implicitly") {
	SJSL::JobSystem js{};
	js.Schedule( [] { PrintNumber(1); });
	js.Schedule( [] { PrintNumber(2); });
	js.Schedule( [] { PrintNumber(3); });
	_CrtDumpMemoryLeaks();
}

TEST_CASE("Scheduling detached jobs explicitly") {

	SJSL::JobSystem js{};
	SJSL::Job* job{ new SJSL::Job{ [] { PrintNumber(1); } , true } };
	SJSL::Job* job1{ new SJSL::Job{ [] { PrintNumber(2); } , true } };
	SJSL::Job* job2{ new SJSL::Job{ [] { PrintNumber(3); } , true } };

	js.Schedule(job);
	js.Schedule(job1);
	js.Schedule(job2);
	_CrtDumpMemoryLeaks();
}
