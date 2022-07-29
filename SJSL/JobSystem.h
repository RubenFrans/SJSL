/*
* SJSL::JobSystem
* This class is the main interface for the SJSL library. 
* The JobSystem class' responsibilities are:
*	Initializing and managing lifetime worker threads.
*	Scheduling jobs by assigning them to worker threads.
*	Basic loadbalancing by assigning jobs to worker threads that have less work.
* 
*/
#pragma once

#include <vector>
#include <functional>

namespace SJSL {

	class WorkerThread;

	class JobSystem final {
	public:

		JobSystem();
		JobSystem(uint32_t nrOfWorkerThreads, bool isMainThreadWorker);
		~JobSystem();

		JobSystem(const JobSystem&) = delete; // Copy constructor
		JobSystem(JobSystem&&) = delete; // Move constructor
		JobSystem& operator=(const JobSystem&) = delete; // Copy assignment
		JobSystem& operator=(const JobSystem&&) = delete; // Move assignment

		void Schedule(std::function<void()> job);

		uint32_t GetAmountOfWorkerThreads();

	private:
		void InitializeWorkerThreads();

		uint32_t m_AmountOfWorkers;
		int m_AssignCounter;
		bool m_IsMainThreadWorker;
		std::vector<WorkerThread*> m_WorkerThreads;

	};
}

