#include "header_files/beldum_logging.hpp"

#include <cstdlib>
#include <filesystem>

#ifndef SPDLOG_DEPENDENCY_EXIST
#error("SPDLOG_DEPENDENCY_EXIST not defined");
#endif
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

BeldumLogging::BeldumLogging() {
    initializeLogger();
}

void BeldumLogging::initializeLogger() {
    std::string fullPath =
        std::filesystem::path(getenv("HOME")).string() + "/.beldum/logs/beldum.log";

    // Ensure the log directory exists
    std::filesystem::create_directories(std::filesystem::path(fullPath).parent_path());

    // Check if the logger already exists
    if (spdlog::get("beldum_logger") != nullptr) {
        logger = spdlog::get("beldum_logger");
    } else {
        // Create a new logger if not found
        logger = spdlog::basic_logger_mt("beldum_logger", fullPath);
        logger->set_level(spdlog::level::info);
        logger->flush_on(spdlog::level::info);
    }
    logger->set_level(spdlog::level::info);             // Set default log level
    logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v"); // Set log message format
}

void BeldumLogging::log(const std::string &message) {
    logger->info(message);
}

void BeldumLogging::logWarning(const std::string &message) {
    logger->warn(message);
}

void BeldumLogging::logError(const std::string &message) {
    logger->error(message);
}
