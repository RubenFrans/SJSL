#include "JobSystem.h"
#include "WorkerThread.h"
#include <thread>

#include "QuickLogger.h"

// By default the jobsystem is initialized with 2 times the supported hardware threads for extra virtual threads
SJSL::JobSystem::JobSystem()
	: JobSystem(std::thread::hardware_concurrency() * 2, false)
{

}

SJSL::JobSystem::JobSystem(uint32_t nrOfWorkerThreads, bool isMainThreadWorker = false)
	: m_AmountOfWorkers{ nrOfWorkerThreads }
	, m_WorkerThreads{}
	, m_IsMainThreadWorker{ isMainThreadWorker }
	, m_AssignCounter{ 0 }
{
	InitializeWorkerThreads();
}

SJSL::JobSystem::~JobSystem()
{
	QuickLogger::GetInstance().Log("JobSystem: DestructorCalled");
	QuickLogger::GetInstance().Log("JobSystem: Deleting worker threads");
	for (WorkerThread* worker : m_WorkerThreads) {
		delete worker;
	}
	QuickLogger::GetInstance().Log("JobSystem: Deleted worker threads");
}

void SJSL::JobSystem::InitializeWorkerThreads() {
	
	QuickLogger::GetInstance().Log("JobSystem: started initializing worker threads");
	m_WorkerThreads.reserve(m_AmountOfWorkers);
	
	for (size_t i = 0; i < m_AmountOfWorkers; i++)
	{
		m_WorkerThreads.emplace_back(new WorkerThread());
	}
	QuickLogger::GetInstance().Log("JobSystem: initialized worker threads");
}

/*
* Schedules a job by creating a job and passing it to a workerthread
*/
void SJSL::JobSystem::Schedule(const std::function<void()>& work) {
	SJSL::Job* job = new SJSL::Job{ work };
	job->SetDetached(true);
	Schedule(job);
}

/*
* Schedules a job by passing the job to a workerthread
* Momentarily jobs are assigned to worker threads by corresponding index from when they are added.
* Future implementation should consider the current work load of each worker thread.
* Dividing jobs over workerthreads reduces the amount of job stealing individual worker threads need to do.
* See SJSL:: WorkerThread class 
* Scheduling directly using a job pointer provides the user to reuse jobs without needing to recreate them, but they'll have to clean up the job themself.
*/
void SJSL::JobSystem::Schedule(SJSL::Job* pJob) {
	QuickLogger::GetInstance().Log("JobSystem: Scheduling job");
	
	m_WorkerThreads[m_AssignCounter]->Assign(pJob);
	m_AssignCounter++;

	if (m_AssignCounter > m_AmountOfWorkers - 1)
		m_AssignCounter = 0;

	QuickLogger::GetInstance().Log("JobSystem: Done scheduling job");
}

uint32_t SJSL::JobSystem::GetAmountOfWorkerThreads() const {

	return m_AmountOfWorkers;

}
