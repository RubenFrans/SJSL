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
