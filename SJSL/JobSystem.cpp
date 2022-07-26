
#include "JobSystem.h"
#include "WorkerThread.h"
#include <thread>

// By default the jobsystem is initialized with 2 times the supported hardware threads for extra virtual threads
SJSL::JobSystem::JobSystem()
	: JobSystem(std::thread::hardware_concurrency() * 2, false)
{

}

SJSL::JobSystem::JobSystem(uint32_t nrOfWorkerThreads, bool isMainThreadWorker = false)
	: m_AmountOfWorkers{ nrOfWorkerThreads }, m_WorkerThreads{}, m_IsMainThreadWorker{ isMainThreadWorker }
{
	InitializeWorkerThreads();
}

SJSL::JobSystem::~JobSystem()
{
}

void SJSL::JobSystem::InitializeWorkerThreads() {

	std::fill(m_WorkerThreads.begin(), m_WorkerThreads.end(), new WorkerThread());
}

void SJSL::JobSystem::Schedule() {

}
