#include "Job.h"

SJSL::Job::Job(std::function<void(void)> work)
	: m_Work{ work }
{

}


void SJSL::Job::Execute()
{
	m_Work();
}
