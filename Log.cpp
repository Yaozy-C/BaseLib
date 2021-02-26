//
// Created by Yaozy on 2020/9/18.
//

#include <memory>
#include <iostream>
#include "Log.h"

namespace Base {

    std::unique_ptr<ILog> logger = nullptr;
    std::unique_ptr<ILog> fileLogger = nullptr;
    std::string dir;

    static const char black[] = {0x1b, '[', '1', ';', '3', '0', 'm', 0};
    static const char red[] = {0x1b, '[', '1', ';', '3', '1', 'm', 0};
    static const char yellow[] = {0x1b, '[', '1', ';', '3', '3', 'm', 0};
    static const char blue[] = {0x1b, '[', '1', ';', '3', '4', 'm', 0};
    static const char normal[] = {0x1b, '[', '0', ';', '3', '9', 'm', 0};

    Logger::Logger(LOGLEVEL level)
            : level(level) {}

    void Logger::Debug(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::debug) {
            std::string data = "[" + GetTime() + "][" + black + "DEBUG" + normal + "][" + file + ":" + func + "()  "
                               + std::to_string(line) + "] " + msg;
            std::cout << data << std::endl;
        }
    }

    void Logger::Info(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::info) {
            std::string data = "[" + GetTime() + "][" + blue + "INFO" + normal + "][" + file + ":" + func + "()  "
                               + std::to_string(line) + "] " + msg;
            std::cout << data << std::endl;
        }
    }

    void Logger::Warn(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::warn) {
            std::string data = "[" + GetTime() + "][" + yellow + "WARN" + normal + "][" + file + ":" + func + "()  "
                               + std::to_string(line) + "] " + msg;
            std::cout << data << std::endl;
        }
    }

    void Logger::Error(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::error) {
            std::string data = "[" + GetTime() + "][" + red + "ERROR" + normal + "][" + file + ":" + func + "()  "
                               + std::to_string(line) + "] " + msg;
            std::cerr << data << std::endl;
        }
    }

    void Logger::ReSetLogLevel(ILog::LOGLEVEL level_) {
        level = level_;
    }


    FileLogger::FileLogger(LOGLEVEL level) : level(level) {
    }

    FileLogger::~FileLogger() {
        Shutdown();
    }

    void FileLogger::Debug(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::debug) {
            std::string data =
                    "[" + GetTime() + "][" + black + "DEBUG" + normal + "][" + file + ":" + func + "()  "
                    + std::to_string(line) + "] " + msg;
            AddTask(data);
        }
    }

    void FileLogger::Info(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::info) {
            std::string data =
                    "[" + GetTime() + "][" + blue + "INFO" + normal + "][" + file + ":" + func + "()  "
                    + std::to_string(line) + "] " + msg;
            AddTask(data);
        }
    }

    void FileLogger::Warn(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::warn) {
            std::string data =
                    "[" + GetTime() + "][" + yellow + "WARN" + normal + "][" + file + ":" + func + "()  "
                    + std::to_string(line) + "] " + msg;
            AddTask(data);
        }
    }

    void FileLogger::Error(const std::string &msg, const std::string &file, std::size_t line, const std::string &func) {
        if (level >= LOGLEVEL::error) {
            std::string data =
                    "[" + GetTime() + "][" + red + "ERROR" + normal + "][" + file + ":" + func + "()  "
                    + std::to_string(line) + "] " + msg;
            AddTask(data);
        }
    }

    void FileLogger::ReSetLogLevel(ILog::LOGLEVEL level_) {
        level = level_;
    }

    void FileLogger::Run(std::string &msg) {
        std::string fileName = dir + GetDate() + ".log";
        if (!fileStream.is_open()) {
            fileStream.open(fileName, std::ios::app);
        }
        fileStream << msg << std::endl;
        fileStream.close();
    }

    std::string GetTime() {
        time_t now = time(nullptr);
        char buf[24] = {0};
        strftime(buf, 24, "%Y-%m-%d %H:%M:%S", localtime(&now));
        return buf;
    }

    std::string GetDate() {
        time_t now = time(nullptr);
        char buf[24] = {0};
        strftime(buf, 24, "%Y-%m-%d", localtime(&now));
        return buf;
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
}
