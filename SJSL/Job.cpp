#include "Job.h"
#include <iostream>

SJSL::Job::Job(const std::function<void(void)>& work)
	: m_Work{ work }
	, m_Status{ SJSL::JobStatus::unassigned }
	, m_RunDetached{ false }
	, m_JoinMutex{}
	, m_JoinCondition{}
{

}

/*
* Executes job and notifies other threads that might be waiting for this job to finish.
*/
void SJSL::Job::Execute()
{
	m_Status = SJSL::JobStatus::working;

	if (m_Work) {

		m_Work();
	}
	else {
		std::cout << "Work was empty" << std::endl;

	}

	m_Status = SJSL::JobStatus::complete;
	m_JoinCondition.notify_all(); 
}

/*
* Used to synchronize a job with another thread.
* If you need to be sure that a job is finished before continueing code execution
* E.g. Before switching the front and backbuffer you should join all render jobs.
*/
void SJSL::Job::Join() {

	std::unique_lock joinLock{ m_JoinMutex };

	m_JoinCondition.wait(joinLock, [&]() {

		return m_Status == SJSL::JobStatus::complete;

		});
}

SJSL::JobStatus SJSL::Job::GetJobStatus() const {

	return m_Status;
}

void SJSL::Job::SetDetached(bool runDetached) {
	m_RunDetached = runDetached;
}

bool SJSL::Job::RunsDetached() const {
	return m_RunDetached;
}

/*
* Resets the job status to unassigned
* This is used if you want to run the same job multiple times without needing to recreate jobs
* This is only possible when the job wasn't scheduled as a detached job because it would be deleted and cause a dangling pointer
*/
void SJSL::Job::Reset() {	
	if (!m_RunDetached) {
		m_Status = SJSL::JobStatus::unassigned;
	}
}
