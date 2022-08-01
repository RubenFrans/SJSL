/*
* SJSL::JobSystem Unit tests
* This file is used to test the functionality and maintain the stability of the SJSL library
* This file tests the interface of the SJSL::JobSystem class
*/

#define CATCH_CONFIG_MAIN
#include "catch2\catch_all.hpp"
#include "vld.h"

#include "JobSystem.h"
#include "WorkerThread.h"
#include <iostream>
#include <thread>
#include <chrono>


void PrintNumber(int number) {
	std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000));
	std::cout << "Thread " << std::this_thread::get_id() << ": " << number << std::endl;
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


TEST_CASE("Scheduling and waiting for a job") {

	SJSL::JobSystem js{};
	SJSL::Job* job{ new SJSL::Job{ [] { PrintNumber(1); } } };

	js.Schedule(job);

	std::cout << "Waiting for job to finish" << std::endl;
	job->Join();
	std::cout << "Job Finished" << std::endl;
	delete job;

}

TEST_CASE("Scheduling and waiting for multiple self managed jobs") {

	SJSL::JobSystem js{};
	SJSL::Job* job{ new SJSL::Job{ [] { PrintNumber(1); } } };
	SJSL::Job* job1{ new SJSL::Job{ [] { PrintNumber(2); } } };
	SJSL::Job* job2{ new SJSL::Job{ [] { PrintNumber(3); } } };

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
}


TEST_CASE("Scheduling detached jobs") {
	SJSL::JobSystem js{};
	js.Schedule( [] { PrintNumber(1); });
	js.Schedule( [] { PrintNumber(2); });
	js.Schedule( [] { PrintNumber(3); });
}

TEST_CASE("Reusing a non detached job") {

	SJSL::JobSystem js{};
	SJSL::Job* job{ new SJSL::Job{ [] { PrintNumber(1); } } };
	SJSL::Job* job1{ new SJSL::Job{ [] { PrintNumber(2); } } };
	SJSL::Job* job2{ new SJSL::Job{ [] { PrintNumber(3); } } };

	js.Schedule(job);
	js.Schedule(job1);
	js.Schedule(job2);

	job->Join();
	job1->Join();
	job2->Join();

	REQUIRE(job->GetJobStatus() == SJSL::JobStatus::complete);
	REQUIRE(job1->GetJobStatus() == SJSL::JobStatus::complete);
	REQUIRE(job2->GetJobStatus() == SJSL::JobStatus::complete);

	job->Reset();
	job1->Reset();
	job2->Reset();

	REQUIRE(job->GetJobStatus() == SJSL::JobStatus::unassigned);
	REQUIRE(job1->GetJobStatus() == SJSL::JobStatus::unassigned);
	REQUIRE(job2->GetJobStatus() == SJSL::JobStatus::unassigned);

	js.Schedule(job);
	js.Schedule(job1);
	js.Schedule(job2);

	job->Join();
	job1->Join();
	job2->Join();

	REQUIRE(job->GetJobStatus() == SJSL::JobStatus::complete);
	REQUIRE(job1->GetJobStatus() == SJSL::JobStatus::complete);
	REQUIRE(job2->GetJobStatus() == SJSL::JobStatus::complete);

}

