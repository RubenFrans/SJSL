#include "WorkerThread.h"


SJSL::WorkerThread::WorkerThread()
	: m_WorkerStatus{ WorkerStatus::idle }
	, m_LocalJobs{}
	, m_GlobalJobs{}
	, m_KillWorkerThread{ false }
	, m_LocalJobMutex{}
{
	m_JobThread = std::thread(&WorkerThread::ProcessJobs, this);
}

/*
* If a workerthread is destroyed before being joined automatically join it
*/
SJSL::WorkerThread::~WorkerThread() {
	
	if(m_JobThread.joinable())
		Join();
}

/*
* Before joining the worker thread we make sure all it's jobs are finished
* This makes sure no work is abandoned when the jobsystem is terminated prematurely (happens in small unit tests or applications)
*/
void SJSL::WorkerThread::Join() {

	std::unique_lock joinLock{ m_JoinThreadMutex };
	m_JoinThreadCondition.wait(joinLock, [&]() {

		return m_LocalJobs.empty();

		});

	m_KillWorkerThread = true;
	m_ProcessJobsCondition.notify_all();
	m_JobThread.join();

}

void SJSL::WorkerThread::Assign(std::shared_ptr<SJSL::Job>& pJob, bool isLocalJob) {

	if (!isLocalJob) {
		AssignJobToGlobalQueue(pJob);
	}
	else {
		AssignJobToLocalQueue(pJob);
	}
}

/*
* Handles Assigning a job to the local jobqueue of the worker thread
*/
void SJSL::WorkerThread::AssignJobToLocalQueue(std::shared_ptr<SJSL::Job>& pJob) {

	std::unique_lock<std::mutex> lock(m_LocalJobMutex);

	m_LocalJobs.emplace_back(pJob);
	pJob->MarkAssigned();
	m_ProcessJobsCondition.notify_all();
	
	// Notify that the thread can't be killed because there are jobs in the local queue
	m_JoinThreadCondition.notify_all();
	
}


/*
* Handles Assigning a job to the global jobqueue of the worker thread
*/
void SJSL::WorkerThread::AssignJobToGlobalQueue(std::shared_ptr<SJSL::Job>& pJob) {
	
	std::unique_lock<std::mutex> lock(m_GlobalJobMutex);

	m_GlobalJobs.emplace_back(pJob);
	pJob->MarkAssigned();
	m_ProcessJobsCondition.notify_all();

	// Notify that the thread can't be killed because there are jobs in the local queue
	m_JoinThreadCondition.notify_all();

}


/*
* Processes jobs that the worker has in queue.
* Local jobs are prioritized over global jobs because no other thread is able to steal them.
* Local jobs should be used when dealing with jobs that require code to be executed from a certain thread.
* E.g. UI
* When a thread has no jobs left in his queue's the worker will try to steal work from other worker's global queue's
*/
void SJSL::WorkerThread::ProcessJobs() {

	while (!m_KillWorkerThread)
	{

		std::unique_lock<std::mutex> localLock{ m_LocalJobMutex };

		m_ProcessJobsCondition.wait(localLock, [&]() {

			return m_LocalJobs.size() > 0 || m_KillWorkerThread;

			});


		if (m_KillWorkerThread)
			break;

		std::shared_ptr<SJSL::Job> job = m_LocalJobs.front();
		m_LocalJobs.pop_front();


		// Unlock before start executing the job
		localLock.unlock();
		
		job->Execute();

		// If local jobs are exausted notify that the thread can be killed
		if (m_LocalJobs.empty())
			m_JoinThreadCondition.notify_all();

	}
}

void SJSL::WorkerThread::ProcessLocalJob() {

}

void SJSL::WorkerThread::ProcessGlobalJob() {

}

size_t SJSL::WorkerThread::GetAmountOfLocalJobs() const {
	return m_LocalJobs.size();
}

size_t SJSL::WorkerThread::GetAmountOfGlobalJobs() const {

	return m_GlobalJobs.size();

}
