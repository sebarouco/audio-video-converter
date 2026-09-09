#include <drogon/drogon.h>
#include <iostream>
#include <memory>
#include "infrastructure/config/AppConfig.h"
#include "common/logging/Logger.h"
#include "application/services/ServiceInitializer.h"

int main() {
    using namespace infrastructure::config;
    using namespace common::logging;
    using namespace application::services;
    
    try {
        // Initialize logger
        Logger::init();
        Logger::info("Starting Online File Converter");
        
        // Initialize all services
        ServiceInitializer::initializeServices("config/config.json");
        Logger::info("Application services initialized successfully");
        
        // Load configuration
        auto& config = AppConfig::getInstance();
        Logger::info("Configuration loaded successfully");
        
        // Initialize Drogon
        drogon::app().loadConfigFile("config/config.json");
        
        // Register controllers
        drogon::app().registerController(new presentation::controllers::FileController());
        drogon::app().registerController(new presentation::controllers::ConversionController());
        
        // Start the server
        Logger::info("Starting HTTP server on " + config.getServerHost() + ":" + std::to_string(config.getServerPort()));
        // Listen on both IPv4 and IPv6
        drogon::app().addListener("0.0.0.0", config.getServerPort());
        drogon::app().addListener("::", config.getServerPort());
        drogon::app().setThreadNum(config.getServerThreads());
        
        // Enable CORS
        drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr& req,
                                                   const drogon::HttpResponsePtr& resp) {
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
        });
        
        // Run the application
        drogon::app().run();
        
    } catch (const std::exception& e) {
        Logger::error("Fatal error: " + std::string(e.what()));
        std::cerr << "Fatal error: " << e.what() << std::endl;
        ServiceInitializer::shutdownServices();
        return 1;
    }
    
    Logger::info("Online File Converter stopped");
    ServiceInitializer::shutdownServices();
    return 0;
}