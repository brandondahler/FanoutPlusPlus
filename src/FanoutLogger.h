#ifndef FANOUTLOGGER_H
#define FANOUTLOGGER_H

#include <string>
#include <sstream>

namespace FanoutLogger
{
        enum MessageSeverity
        {
            LOG_ERROR = 0,
            LOG_WARNING,
            LOG_INFO,
            LOG_DEBUG
        };

        void LogMessage(MessageSeverity severity, const char* source, const char* message);
        void LogMessage(MessageSeverity severity, const char* source, std::string message);
        void LogMessage(MessageSeverity severity, const char* source, std::ostringstream& message);

 };

#endif // FANOUTLOGGER_H
