#pragma once
#include <list>

namespace SJSL {

	enum class WorkerStatus {
		idle, // Worker has no jobs
		busy, // Worker is processing
		pending // Worker has pending jobs but is not currently working on them
	};

	class WorkerThread
	{

	public:
		WorkerThread() = default;
		virtual ~WorkerThread() = default;

		WorkerThread(const WorkerThread&) = delete; // Copy constructor
		WorkerThread(WorkerThread&&) = delete; // Move constructor
		WorkerThread& operator=(const WorkerThread&) = delete; // Copy assignment
		WorkerThread& operator=(const WorkerThread&&) = delete; // Move assignment


		void Assign(); // Assign a job to the worker thread

	private:
		WorkerStatus m_WorkerStatus;
		std::list<int> m_LocalJobs; // Other workers CANNOT steal from this list
		std::list<int> m_GlobalJobs; // Other workers can steal from this list to improve load ballancing

	};
}


