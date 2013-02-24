#include "FanoutLogger.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif // HAVE_SYSLOG_H

using namespace std;

// Set default log level
int FanoutLogger::loggingLevel = FANOUT_LOG_INFO;

// Ensure proper initialization and uninitialization
FanoutLogger FanoutLogger::systemLogger;


void FanoutLogger::SetLoggingLevel(int level)
{
    loggingLevel = level;
}

void FanoutLogger::LogMessage(MessageSeverity severity, const char* source, const char* message)
{

    if (severity <= loggingLevel)
    {
        ostringstream errorMessage;
        errorMessage << setw(7) << GetSeverityLabel(severity) << " : " << source << " :: " <<  message;

        #ifdef HAVE_SYSLOG_H
            int syslogSeverity;
            switch (severity)
            {
                case FANOUT_LOG_DEBUG:
                    syslogSeverity = LOG_DEBUG;
                    break;
                case FANOUT_LOG_INFO:
                    syslogSeverity = LOG_INFO;
                    break;

                case FANOUT_LOG_WARNING:
                    syslogSeverity = LOG_WARNING;
                    break;

                default:
                case FANOUT_LOG_ERROR:
                    syslogSeverity = LOG_ERR;
                    break;
            }

            syslog(syslogSeverity, errorMessage.str().c_str());
        #endif

        cout << errorMessage.str().c_str() << endl;
    }
}

void FanoutLogger::LogMessage(MessageSeverity severity, const char* source, string message)
{
    LogMessage(severity, source, message.c_str());
}

void FanoutLogger::LogMessage(MessageSeverity severity, const char* source, ostringstream& message)
{
    LogMessage(severity, source, message.str());
}




/* Private functions */

FanoutLogger::FanoutLogger()
{
    #ifdef HAVE_SYSLOG_H
        openlog(PACKAGE, LOG_CONS | LOG_PID, LOG_DAEMON);
    #endif // HAVE_SYSLOG_H
}

FanoutLogger::~FanoutLogger()
{
    #ifdef HAVE_SYSLOG_H
        closelog();
    #endif // HAVE_SYSLOG_H

}


const char* FanoutLogger::GetSeverityLabel(MessageSeverity severity)
{
    switch (severity)
    {
        case FANOUT_LOG_ERROR:
            return "Error";
        case FANOUT_LOG_WARNING:
            return "Warning";
        case FANOUT_LOG_INFO:
            return "Info";
        case FANOUT_LOG_DEBUG:
            return "Debug";
    }

    return "Unknown";
}
