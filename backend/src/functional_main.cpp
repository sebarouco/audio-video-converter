#include <drogon/drogon.h>
#include <iostream>
#include <memory>
#include "infrastructure/config/AppConfig.h"
#include "common/logging/Logger.h"
#include "application/services/ServiceInitializer.h"
#include "presentation/controllers/ConversionController.h"
#include "presentation/controllers/FileController.h"
#include "presentation/controllers/AuthController.h"

int main() {
    using namespace infrastructure::config;
    using namespace common::logging;
    using namespace application::services;
    using namespace presentation::controllers;
    
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
        
        // Controllers are auto-registered by Drogon via METHOD_LIST macros
        // No need to manually register them
        
        // Start the server
        Logger::info("Starting HTTP server on " + config.getServerHost() + ":" + std::to_string(config.getServerPort()));
        drogon::app().addListener(config.getServerHost(), config.getServerPort());
        drogon::app().setThreadNum(config.getServerThreads());
        
        // Enable CORS
        drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr& req,
                                                   const drogon::HttpResponsePtr& resp) {
            (void)req; // Suppress unused parameter warning
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
        });
        
        // Handle OPTIONS requests for CORS preflight
        drogon::app().registerHandler("/api/{path}",
            [](const drogon::HttpRequestPtr& req,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                if (req->method() == drogon::HttpMethod::Options) {
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->addHeader("Access-Control-Allow-Origin", "*");
                    resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
                    resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
                    callback(resp);
                } else {
                    // Let other handlers process the request
                    callback(drogon::HttpResponse::newNotFoundResponse());
                }
            },
            {drogon::Options, drogon::Get, drogon::Post, drogon::Put, drogon::Delete}
        );
        
        // Run the application
        Logger::info("Starting Drogon application");
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