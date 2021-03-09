//
// Created by Yaozy on 2020/9/18.
//

#include <memory>
#include <iostream>
#include <sys/stat.h>
#include "Log.h"

namespace Base {

    std::unique_ptr<Logger> logger = nullptr;
    std::unique_ptr<FileLogger> fileLogger = nullptr;
    std::string dir;

    static const char black[] = {0x1b, '[', '1', ';', '3', '0', 'm', 0};
    static const char red[] = {0x1b, '[', '1', ';', '3', '1', 'm', 0};
    static const char green[] = {0x1b, '[', '1', ';', '3', '2', 'm', 0};
    static const char yellow[] = {0x1b, '[', '1', ';', '3', '3', 'm', 0};
    static const char blue[] = {0x1b, '[', '1', ';', '3', '4', 'm', 0};
    static const char white[] = {0x1b, '[', '1', ';', '3', '7', 'm', 0};
    static const char normal[] = {0x1b, '[', '0', ';', '3', '9', 'm', 0};


    Logger::Logger(LOGLEVEL level)
            : level(level) {}

    void Logger::Trace(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::trace) {
            std::string data = "[" + GetTime() + "][" + green + "TRACE" + normal + "][" + file + ":" + func + "() "
                               + std::to_string(line) + "] " + msg;
            std::cout << data << std::endl;
        }
    }

    void Logger::Debug(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::debug) {
            std::string data = "[" + GetTime() + "][" + black + "DEBUG" + normal + "][" + file + ":" + func + "() "
                               + std::to_string(line) + "] " + msg;
            std::cout << data << std::endl;
        }
    }

    void Logger::Info(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::info) {
            std::string data = "[" + GetTime() + "][" + blue + "INFO " + normal + "][" + file + ":" + func + "() "
                               + std::to_string(line) + "] " + msg;
            std::cout << data << std::endl;
        }
    }

    void Logger::Warn(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::warn) {
            std::string data = "[" + GetTime() + "][" + yellow + "WARN " + normal + "][" + file + ":" + func + "() "
                               + std::to_string(line) + "] " + msg;
            std::cout << data << std::endl;
        }
    }

    void Logger::Error(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::error) {
            std::string data = "[" + GetTime() + "][" + red + "ERROR" + normal + "][" + file + ":" + func + "() "
                               + std::to_string(line) + "] " + msg;
            std::cerr << data << std::endl;
        }
    }

    void Logger::ReSetLogLevel(ILog::LOGLEVEL level_) {
        level = level_;
    }


    FileLogger::FileLogger(LOGLEVEL level) : level(level), offsetDay(0), rollover(false) {
    }

    FileLogger::~FileLogger() {
        Shutdown();
        fileStream.close();
    }

    void FileLogger::Trace(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::debug) {
            std::string data =
                    "[" + GetTime() + "][" + green + "TRACE" + normal + "][" + file + ":" + func + "() "
                    + std::to_string(line) + "] " + msg;
            AddTask(data);
        }
    }

    void FileLogger::Debug(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::debug) {
            std::string data =
                    "[" + GetTime() + "][" + black + "DEBUG" + normal + "][" + file + ":" + func + "() "
                    + std::to_string(line) + "] " + msg;
            AddTask(data);
        }
    }

    void FileLogger::Info(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::info) {
            std::string data =
                    "[" + GetTime() + "][" + blue + "INFO " + normal + "][" + file + ":" + func + "() "
                    + std::to_string(line) + "] " + msg;
            AddTask(data);
        }
    }

    void FileLogger::Warn(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::warn) {
            std::string data =
                    "[" + GetTime() + "][" + yellow + "WARN " + normal + "][" + file + ":" + func + "() "
                    + std::to_string(line) + "] " + msg;
            AddTask(data);
        }
    }

    void FileLogger::Error(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::error) {
            std::string data =
                    "[" + GetTime() + "][" + red + "ERROR" + normal + "][" + file + ":" + func + "() "
                    + std::to_string(line) + "] " + msg;
            AddTask(data);
        }
    }

    void FileLogger::ReSetLogLevel(ILog::LOGLEVEL level_) {
        level = level_;
    }

    void FileLogger::Run(std::string &msg) {
        std::string fileName = dir + GetDate(0) + ".log";
        if (logName != fileName) {
            if (fileStream.is_open()) {
                fileStream.close();
            }
            fileStream.open(fileName, std::ios::app);
            logName = fileName;

            if (rollover) {
                sevenDaysAgoLogName = dir + GetDate(offsetDay) + ".log";
                if (FileExists(sevenDaysAgoLogName)) {
                    remove(sevenDaysAgoLogName.c_str());
                }
            }
        }
        fileStream << msg << std::endl;
    }

    void FileLogger::SetLogRollover(const int &day) {
        offsetDay = day;
        rollover = true;
    }

    std::string GetTime() {
        time_t now = time(nullptr);
        char buf[24] = {0};
        strftime(buf, 24, "%Y-%m-%d %H:%M:%S", localtime(&now));
        return buf;
    }

    std::string GetDate(const int &offset) {
        char dt[100];
        std::chrono::duration<int, std::ratio<60 * 60 * 24>> one_day(1);  //创建duration时间段对象
        std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point tomorrow = today + offset * one_day;  //两个时间点相加

        time_t tt = std::chrono::system_clock::to_time_t(tomorrow);
        tm *timePtr = localtime(&tt);
        sprintf(dt, "%d-%02d-%02d", timePtr->tm_year + 1900, timePtr->tm_mon + 1, timePtr->tm_mday);

        return dt;
    }

    bool FileExists(const std::string &name) {
        struct stat buffer{};
        return (stat(name.c_str(), &buffer) == 0);
    }

    void LogInit() {
        if (logger == nullptr)
            logger = std::make_unique<Logger>(ILog::LOGLEVEL::debug);
    }

    void LogInit(const std::string &path) {
        SetLogPath(path);
        if (fileLogger == nullptr)
            fileLogger = std::make_unique<FileLogger>(ILog::LOGLEVEL::debug);
    }

    void Debug(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (logger)
            logger->Debug(msg, file, line, func);
        if (fileLogger)
            fileLogger->Debug(msg, file, line, func);
    }

    void Trace(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (logger)
            logger->Trace(msg, file, line, func);
        if (fileLogger)
            fileLogger->Trace(msg, file, line, func);
    }

    void Info(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (logger)
            logger->Info(msg, file, line, func);
        if (fileLogger)
            fileLogger->Info(msg, file, line, func);
    }

    void Warn(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (logger)
            logger->Warn(msg, file, line, func);
        if (fileLogger)
            fileLogger->Warn(msg, file, line, func);
    }

    void Error(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (logger)
            logger->Error(msg, file, line, func);
        if (fileLogger)
            fileLogger->Error(msg, file, line, func);
    }

    void SetLogPath(const std::string &path) {
        dir = path;
        if (!dir.empty() && dir[dir.size() - 1] != '/') {
            dir += '/';
        }
    }

    void SetLevel(ILog::LOGLEVEL level) {
        if (logger)
            logger->ReSetLogLevel(level);
        if (fileLogger)
            fileLogger->ReSetLogLevel(level);
    }

    void SetLogRollover(const int &day) {
        if (fileLogger)
            fileLogger->SetLogRollover(day);
    }
}
