#pragma once

#include <string>
#include <fstream>

class QuickLogger
{
public:

	static QuickLogger& GetInstance() {

		if (m_Instance == NULL) m_Instance = new QuickLogger();
		return *m_Instance;

	}

	~QuickLogger();
	void Log(const std::string&);
	void SetLoggingEnabled(bool);

private:
	QuickLogger();
	static QuickLogger* m_Instance;
	bool m_LoggingEnabled;
	std::string m_LogFileName;
	std::ofstream m_OutputFileStream;
};


