/*
* SJSL::WorkerThread Unit tests
* This file is used to test the functionality and maintain the stability of the SJSL library
* This file tests the interface of the SJSL::WorkerThread class
*/

#include "catch2\catch_all.hpp"
#include "WorkerThread.h"
#include <iostream>
#include <chrono>

TEST_CASE("Assigning a local job to WorkerThread") {

	SJSL::WorkerThread* worker{ new SJSL::WorkerThread{} };

	int amountOfJobs{ 8 };

	//for (int i = 0; i < amountOfJobs; i++)
	//{
	//	worker->Assign([] { std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(100)); });
	//}

	REQUIRE(worker->GetAmountOfLocalJobs() == amountOfJobs);

	worker->Join();

	REQUIRE(worker->GetAmountOfLocalJobs() == 0);
	delete worker;
}