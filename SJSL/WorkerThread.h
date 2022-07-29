#pragma once
#include <list>
#include <functional>
#include <thread>
#include <mutex>

namespace SJSL {

	enum class WorkerStatus {
		idle, // Worker has no jobs
		busy, // Worker is processing
		pending // Worker has pending jobs but is not currently working on them
	};

	class WorkerThread
	{

	public:
		
		WorkerThread();
		virtual ~WorkerThread();

		WorkerThread(const WorkerThread&) = delete; // Copy constructor
		WorkerThread(WorkerThread&&) = delete; // Move constructor
		WorkerThread& operator=(const WorkerThread&) = delete; // Copy assignment
		WorkerThread& operator=(const WorkerThread&&) = delete; // Move assignment

		void Assign(std::function<void()> job, bool isLocalJob = true); // Assign a job to the worker thread
		void Join();

		int GetAmountOfLocalJobs();
		int GetAmountOfGlobalJobs();

	private:

		void ProcessJobs();

		WorkerStatus m_WorkerStatus;
		std::list<std::function<void()>> m_LocalJobs; // Other workers CANNOT steal from this list
		std::list<std::function<void()>> m_GlobalJobs; // Other workers can steal from this list to improve load ballancing

		bool m_KillWorkerThread;

		std::thread m_JobThread;
		std::mutex m_LocalJobMutex;
		std::mutex m_GlobalJobMutex;
		std::condition_variable m_ProcessJobsCondition;
		std::condition_variable m_KillThreadCondition;

	};
}


