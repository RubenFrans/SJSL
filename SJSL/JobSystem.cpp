
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

	//for (WorkerThread* thread : m_WorkerThreads) {
	//	thread->Join();
	//}

	for (WorkerThread* thread : m_WorkerThreads) {
		delete thread;
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
* Schedules a job by passing the work to a free workerthread
*/
void SJSL::JobSystem::Schedule(std::function<void()> job) {

	m_WorkerThreads[m_AssignCounter]->Assign(job);
	m_AssignCounter++;

	if (m_AssignCounter > m_AmountOfWorkers - 1)
		m_AssignCounter = 0;

}

uint32_t SJSL::JobSystem::GetAmountOfWorkerThreads() {

	return m_AmountOfWorkers;

}
