#pragma once

#ifndef BELDUM_LOGGING_H
#define BELDUM_LOGGING_H

#include <memory>

#ifndef SPDLOG_DEPENDENCY_EXIST
#error("SPDLOG_DEPENDENCY_EXIST not defined");
#endif
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

class BeldumLogging {
  public:
    BeldumLogging();
    void log(const std::string &message);
    void logWarning(const std::string &message);
    void logError(const std::string &message);

  private:
    std::shared_ptr<spdlog::logger> logger;
    const std::string loggingPath = "~/.beldum/logs/beldum.log";
    void initializeLogger();
};

#endif
