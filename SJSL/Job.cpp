#include "Job.h"
#include <iostream>
#include <assert.h>


SJSL::Job::Job(const std::function<void(void)>& work)
	: m_Work{ work }
	, m_Status{ SJSL::JobStatus::unassigned }
	, m_JobMutex{}
	, m_JoinCondition{}
{

}

/*
* Executes job and notifies other threads that might be waiting for this job to finish.
*/
void SJSL::Job::Execute()
{
	assert(m_Status == SJSL::JobStatus::pending);

	m_JobMutex.lock();

	m_Status = SJSL::JobStatus::working;

	if (m_Work) {

		m_Work();
	}

	m_Status = SJSL::JobStatus::complete;

	m_JobMutex.unlock();

	m_JoinCondition.notify_all(); 

	assert(m_Status == SJSL::JobStatus::complete);

}

/*
* Used to synchronize a job with another thread.
* If you need to be sure that a job is finished before continueing code execution
* E.g. Before switching the front and backbuffer you should join all render jobs.
*/
void SJSL::Job::Join() {

	std::unique_lock joinLock{ m_JobMutex };

	m_JoinCondition.wait(joinLock, [&]() {

		return m_Status == SJSL::JobStatus::complete;

		});
	
}

SJSL::JobStatus SJSL::Job::GetJobStatus() const {
	return m_Status;
}

/*
* Resets the job status to unassigned
* This is used if you want to run the same job multiple times without needing to recreate jobs
*/
void SJSL::Job::Reset() {	

	std::unique_lock<std::mutex>(m_JobMutex);
	m_Status = SJSL::JobStatus::unassigned;
}

void SJSL::Job::MarkAssigned() {

	std::unique_lock<std::mutex>(m_JobMutex);
	m_Status = SJSL::JobStatus::pending;

}
