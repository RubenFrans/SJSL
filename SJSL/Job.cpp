#include "Job.h"
#include <iostream>

SJSL::Job::Job(const std::function<void(void)>& work, bool runDetached)
	: m_Work{ work }
	, m_Status{ SJSL::JobStatus::unassigned }
	, m_RunDetached{ runDetached }
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

SJSL::JobStatus SJSL::Job::GetJobStatus() {

	return m_Status;
}

bool SJSL::Job::RunsDetached() {
	return m_RunDetached;
}
