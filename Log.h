//
// Created by Yaozy on 2020/9/18.
//

#ifndef LOG_H
#define LOG_H

#include <string>
#include <mutex>
#include <thread>
#include <fstream>
#include "IndependentThread.h"

#define LOG_DEBUG(msg) Base::Debug(msg,__FILE__,__LINE__,__FUNCTION__)
#define LOG_INFO(msg) Base::Info(msg,__FILE__,__LINE__,__FUNCTION__)
#define LOG_WARN(msg) Base::Warn(msg,__FILE__,__LINE__,__FUNCTION__)
#define LOG_ERROR(msg) Base::Error(msg,__FILE__,__LINE__,__FUNCTION__)
#define LOG_INIT_LOGGER() Base::LogInit()
#define LOG_INIT_FILELOGGER(msg) Base::LogInit(msg)
#define LOG_SETLEVEL(level) Base::SetLevel(level)

#define ERROR Base::ILog::LOGLEVEL::error
#define WARN Base::ILog::LOGLEVEL::warn
#define INFO Base::ILog::LOGLEVEL::info
#define DEBUG Base::ILog::LOGLEVEL::debug


namespace Base {

    class ILog {
    public:
        enum class LOGLEVEL {
            error = 0,
            warn = 1,
            info = 2,
            debug = 3
        };

        ILog() = default;

        virtual ~ILog() = default;

        ILog(const ILog &) = default;

        ILog &operator=(const ILog &) = default;

    public:

        virtual void
        Debug(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) = 0;

        virtual void
        Info(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) = 0;

        virtual void
        Warn(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) = 0;

        virtual void
        Error(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) = 0;

        virtual void ReSetLogLevel(ILog::LOGLEVEL level) = 0;
    };

    class Logger : public ILog {
    public:
        explicit Logger(LOGLEVEL level = LOGLEVEL::info);

        ~Logger() override = default;

        Logger(const Logger &) = delete;

        Logger &operator=(const Logger &) = delete;

    public:

        void Debug(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) override;

        void Info(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) override;

        void Warn(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) override;

        void Error(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) override;

        void ReSetLogLevel(ILog::LOGLEVEL level) override;

    private:
        LOGLEVEL level;
    };


    class FileLogger : public ILog, public IndependentThread<std::string> {

    public:

        explicit FileLogger(Logger::LOGLEVEL level = Logger::LOGLEVEL::info);

        ~FileLogger() override;

        FileLogger(const FileLogger &) = delete;

        FileLogger &operator=(const FileLogger &) = delete;

        void Debug(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) override;

        void Info(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) override;

        void Warn(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) override;

        void Error(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) override;

        void ReSetLogLevel(ILog::LOGLEVEL level) override;

    private:
        Logger::LOGLEVEL level;
        std::ofstream fileStream;
        void Run(std::string &msg) override;
    };

    static std::string GetTime();

    static std::string GetDate();

    void LogInit();

    void LogInit(const std::string &path);

    void Debug(const std::string &msg, const std::string &file, std::size_t line, const std::string &func);

    void Info(const std::string &msg, const std::string &file, std::size_t line, const std::string &func);

    void Warn(const std::string &msg, const std::string &file, std::size_t line, const std::string &func);

    void Error(const std::string &msg, const std::string &file, std::size_t line, const std::string &func);

    void SetLogPath(const std::string &path);

    void SetLevel(ILog::LOGLEVEL level);
}


#endif //LOG_H
