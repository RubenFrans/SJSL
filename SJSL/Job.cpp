#include "Job.h"
#include <iostream>
#include <assert.h>
#include "QuickLogger.h"

SJSL::Job::Job(const std::function<void(void)>& work)
	: m_Work{ work }
	, m_Status{ SJSL::JobStatus::unassigned }
	, m_RunDetached{ false }
	, m_JobMutex{}
	, m_JoinCondition{}
{

}

/*
* Executes job and notifies other threads that might be waiting for this job to finish.
*/
void SJSL::Job::Execute()
{
	QuickLogger::GetInstance().Log("Job: executing job");
	
	assert(m_Status == SJSL::JobStatus::pending);

	//std::unique_lock<std::mutex>(m_JobMutex);

	m_JobMutex.lock();

	m_Status = SJSL::JobStatus::working;

	if (m_Work) {

		m_Work();
	}
	else {
		std::cout << "Work was empty" << std::endl;
		QuickLogger::GetInstance().Log("Job: job was empty");
	}

	m_Status = SJSL::JobStatus::complete;

	m_JobMutex.unlock();

	m_JoinCondition.notify_all(); 

	assert(m_Status == SJSL::JobStatus::complete);
	QuickLogger::GetInstance().Log("Job: done executing job");
}

/*
* Used to synchronize a job with another thread.
* If you need to be sure that a job is finished before continueing code execution
* E.g. Before switching the front and backbuffer you should join all render jobs.
*/
void SJSL::Job::Join() {

	QuickLogger::GetInstance().Log("Job: Joining job");
	
	std::unique_lock joinLock{ m_JobMutex };

	m_JoinCondition.wait(joinLock, [&]() {

		QuickLogger::GetInstance().Log("Job: Checking JoinCondition");
		return m_Status == SJSL::JobStatus::complete;

		});
	
	QuickLogger::GetInstance().Log("Job: Joining job DONE");
}

SJSL::JobStatus SJSL::Job::GetJobStatus() const {
	return m_Status;
}

void SJSL::Job::SetDetached(bool runDetached) {
	std::unique_lock<std::mutex>(m_JobMutex);
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

	QuickLogger::GetInstance().Log("Job: Resetting job");

	std::unique_lock<std::mutex>(m_JobMutex);
	if (!m_RunDetached) {
		m_Status = SJSL::JobStatus::unassigned;
	}
	QuickLogger::GetInstance().Log("Job: Resetting job DONE");
}

void SJSL::Job::MarkAssigned() {

	QuickLogger::GetInstance().Log("Job: Marking job as assigned");
	std::unique_lock<std::mutex>(m_JobMutex);
	m_Status = SJSL::JobStatus::pending;
	QuickLogger::GetInstance().Log("Job: Marking job as assigned DONE");

}
