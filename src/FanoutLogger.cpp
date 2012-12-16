#include "FanoutLogger.h"


#include <iostream>
#include <iomanip>

using namespace std;

namespace FanoutLogger
{
    const char* GetSeverityLabel(MessageSeverity severity);
    int loggingLevel = LOG_INFO;
};

void FanoutLogger::SetLoggingLevel(int level)
{
    loggingLevel = level;
}

void FanoutLogger::LogMessage(MessageSeverity severity, const char* source, const char* message)
{
    if (severity <= loggingLevel)
        cout << setw(7) << GetSeverityLabel(severity) << " : " << source << " :: " <<  message << endl;
}

void FanoutLogger::LogMessage(MessageSeverity severity, const char* source, string message)
{
    LogMessage(severity, source, message.c_str());
}

void FanoutLogger::LogMessage(MessageSeverity severity, const char* source, ostringstream& message)
{
    LogMessage(severity, source, message.str());
}

const char* FanoutLogger::GetSeverityLabel(MessageSeverity severity)
{
    switch (severity)
    {
        case LOG_ERROR:
            return "Error";
        case LOG_WARNING:
            return "Warning";
        case LOG_INFO:
            return "Info";
        case LOG_DEBUG:
            return "Debug";
    }

    return "Unknown";
}

