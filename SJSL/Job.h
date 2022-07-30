#pragma once
#include <functional>
#include <mutex>
#include <condition_variable>

namespace SJSL {

	enum class JobStatus {
		working,
		complete,
		pending,
		unassigned
	};

	class Job
	{
	public:
		Job(const std::function<void(void)>& work, bool runDetached = true);
		virtual ~Job() = default;

		Job(const Job&) = default; // Copy constructor
		Job(Job&&) = delete; // Move constructor
		Job& operator=(const Job&) = default; // Copy assignment
		Job& operator=(const Job&&) = delete; // Move assignment

		void Execute();
		void Join();
		JobStatus GetJobStatus();
		bool RunsDetached();

	private:
		std::function<void(void)> m_Work;
		JobStatus m_Status;
		bool m_RunDetached;
		std::mutex m_JoinMutex{};
		std::condition_variable m_JoinCondition;
	};
}


