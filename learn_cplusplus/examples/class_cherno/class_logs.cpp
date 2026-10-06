#include <iostream>

#define LOG(x)                                                                 \
    std::cout                                                                  \
        << x << std::endl; // define a macro for logging values to the console

class LogClass {

  public:
    enum LogLevel { LogLevelError = 0, LogLevelWarning, LogLevelInfo };

    void setLevel(LogLevel level) {
        // Set the log level
        m_LogLevel = level;
    }
    void Error(const char *message) {
        if (m_LogLevel >= LogLevelError) {
            LOG("[Error]: " << message);
        }
    }
    void Warn(const char *message) {
        if (m_LogLevel >= LogLevelWarning) {
            LOG("[Warning]: " << message);
        }
    }
    void Info(const char *message) {
        if (m_LogLevel >= LogLevelInfo) {
            LOG("[Info]: " << message);
        }
    }

  private:
    LogLevel m_LogLevel;
};
int main() {

    LogClass myLog;
    myLog.setLevel(LogClass::LogLevelWarning);
    myLog.Error("This is an error message.");
    myLog.Warn("This is a warning message.");
    myLog.Info("This is an info message.");
    std::cin.get();
    return 0;
}