#pragma once
#include <functional>

namespace SJSL {

	class Job
	{
	public:
		Job(std::function<void(void)>);
		virtual ~Job() = default;

		Job(const Job&) = delete; // Copy constructor
		Job(Job&&) = delete; // Move constructor
		Job& operator=(const Job&) = delete; // Copy assignment
		Job& operator=(const Job&&) = delete; // Move assignment

		void Execute();

	private:
		std::function<void(void)> m_Work;
	};
}


