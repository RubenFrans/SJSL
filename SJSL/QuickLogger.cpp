#include "QuickLogger.h"
#include <iostream>
#include <thread>

QuickLogger* QuickLogger::m_Instance = nullptr;

QuickLogger::QuickLogger() 
	: m_LoggingEnabled{ false }
	, m_LogFileName{"SJSL.log"}
	, m_OutputFileStream{}
{

}

void QuickLogger::SetLoggingEnabled(bool enabled) {

	m_LoggingEnabled = false;

}


void QuickLogger::Log(const std::string& logLine) {

	if (!m_OutputFileStream.is_open()) {
		m_OutputFileStream.open(m_LogFileName);
	}

	m_OutputFileStream << std::this_thread::get_id() << '\t' << logLine << std::endl;
}


QuickLogger::~QuickLogger() {

	m_OutputFileStream.close();

}

