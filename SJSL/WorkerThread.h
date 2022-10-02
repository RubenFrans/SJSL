/*
* SJSL::Workerthread
* The Workerthread class manages 2 queue's of jobs that are to be executed by the thread managed by this class
* 
* The local queue is a queue of jobs that are guranteed to be executed by this thread.
* This is usefull for executing thread specific code like UI.
* Jobs in the local queue have priority over the jobs in the global queue because they can only be executed by the owning thread.
* 
* The global queue (not implemented yet) contains jobs that do not have thread specific code.
* Jobs in this queue can be stolen by other worker threads if they don't have any work left.
* This way all jobs are automatically load balanced between all worker threads.
*/

#pragma once
#include <list>
#include <functional>
#include <thread>
#include <mutex>
#include "Job.h"

namespace SJSL {

	enum class WorkerStatus {
		idle, // Worker has no jobs
		busy, // Worker is processing
		pending // Worker has pending jobs but is not currently working on them
	};

	class WorkerThread final
	{

	public:
		
		WorkerThread();
		virtual ~WorkerThread();

		WorkerThread(const WorkerThread&) = delete; // Copy constructor
		WorkerThread(WorkerThread&&) = delete; // Move constructor
		WorkerThread& operator=(const WorkerThread&) = delete; // Copy assignment
		WorkerThread& operator=(const WorkerThread&&) = delete; // Move assignment

		void Assign(std::shared_ptr<SJSL::Job>& pJob, bool isLocalJob = true);
		void Join();

		size_t GetAmountOfLocalJobs() const;
		size_t GetAmountOfGlobalJobs() const;

	private:

		void ProcessJobs();
		void ProcessLocalJob();
		void ProcessGlobalJob();
		void AssignJobToLocalQueue(std::shared_ptr<SJSL::Job>& pJob);
		void AssignJobToGlobalQueue(std::shared_ptr<SJSL::Job>& pJob);

		WorkerStatus m_WorkerStatus;
		std::list<std::shared_ptr<SJSL::Job>> m_LocalJobs; // Other workers CANNOT steal from this list
		std::list<std::shared_ptr<SJSL::Job>> m_GlobalJobs; // Other workers can steal from this list to improve load ballancing

		bool m_KillWorkerThread;

		std::thread m_JobThread;
		std::mutex m_LocalJobMutex;
		std::mutex m_GlobalJobMutex;
		std::mutex m_JoinThreadMutex;
		std::condition_variable m_ProcessJobsCondition;
		std::condition_variable m_JoinThreadCondition;

	};
}


