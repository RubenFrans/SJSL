#include "JobSystem.h"
#include "WorkerThread.h"
#include <thread>



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
	for (WorkerThread* worker : m_WorkerThreads) {
		delete worker;
	}
}

void SJSL::JobSystem::InitializeWorkerThreads() {
	
	m_WorkerThreads.reserve(m_AmountOfWorkers);
	
	for (size_t i = 0; i < m_AmountOfWorkers; i++)
	{
		m_WorkerThreads.emplace_back(new WorkerThread());
	}

}

/*
* Schedules a job by creating a job and passing it to a workerthread
* Returns the created to the user so they can manually check the state of the job.
*/
std::shared_ptr<SJSL::Job> SJSL::JobSystem::Schedule(const std::function<void()>& work) {
	std::shared_ptr<SJSL::Job> job = std::make_shared<SJSL::Job>(work);
	Schedule(job);

	return job;
}

/*
* Schedules a job by passing the job to a workerthread
* Momentarily jobs are assigned to worker threads by corresponding index from when they are added.
* Future implementation should consider the current work load of each worker thread.
* Dividing jobs over workerthreads reduces the amount of job stealing individual worker threads need to do.
* See SJSL:: WorkerThread class 
*/
void SJSL::JobSystem::Schedule(std::shared_ptr<SJSL::Job>& pJob) {

	m_WorkerThreads[m_AssignCounter]->Assign(pJob);		
	m_AssignCounter = ++m_AssignCounter % m_AmountOfWorkers;
}

uint32_t SJSL::JobSystem::GetAmountOfWorkerThreads() const {

	return m_AmountOfWorkers;

}
