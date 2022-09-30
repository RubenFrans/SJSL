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

		Job(const std::function<void(void)>& work);
		virtual ~Job() = default;

		Job(const Job&) = default; 
		Job(Job&&) = delete; 
		Job& operator=(const Job&) = default;
		Job& operator=(const Job&&) = delete;

		void Join();
		JobStatus GetJobStatus() const;

		void SetDetached(bool runDetached);
		void Execute();
		bool RunsDetached() const;
		void Reset();
		void MarkAssigned();

	protected:


	private:
		std::function<void(void)> m_Work;
		JobStatus m_Status;
		bool m_RunDetached;
		std::mutex m_JobMutex{};
		std::condition_variable m_JoinCondition;
	};
}


