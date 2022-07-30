#include "Job.h"
#include <iostream>

SJSL::Job::Job(const std::function<void(void)>& work)
	: m_Work{ work }
	, m_Status{ SJSL::JobStatus::unassigned }
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

}

void SJSL::Job::Join() {

	std::mutex m{};
	std::condition_variable m_JoinCondition;
	std::unique_lock joinLock{ m };

	m_JoinCondition.wait(joinLock, [&]() {

		return m_Status == SJSL::JobStatus::complete;

		});
}

SJSL::JobStatus SJSL::Job::GetJobStatus() {

	return m_Status;
}
