#ifndef FANOUTLOGGER_H
#define FANOUTLOGGER_H

#include <string>
#include <sstream>

class FanoutLogger
{
    public:
        enum MessageSeverity
        {
            FANOUT_LOG_ERROR = 1,
            FANOUT_LOG_WARNING,
            FANOUT_LOG_INFO,
            FANOUT_LOG_DEBUG
        };

        ~FanoutLogger();

        static void SetLoggingLevel(int level);

        static void LogMessage(MessageSeverity severity, const char* source, const char* message);
        static void LogMessage(MessageSeverity severity, const char* source, std::string message);
        static void LogMessage(MessageSeverity severity, const char* source, std::ostringstream& message);

    private:
        FanoutLogger();

        static const char* GetSeverityLabel(MessageSeverity severity);

        static int loggingLevel;

        // Ensure proper initialization and uninitialization
        static FanoutLogger systemLogger;
 };

#endif // FANOUTLOGGER_H
