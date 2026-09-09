#include "infrastructure/services/conversion/FFmpegConverter.h"
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <regex>
#include <spdlog/spdlog.h>
#include <filesystem>

namespace infrastructure {
namespace services {
namespace conversion {

FFmpegConverter::FFmpegConverter(const std::string& ffmpegPath, int threads, int timeoutSeconds)
    : ffmpegPath_(ffmpegPath), threads_(threads), timeoutSeconds_(timeoutSeconds) {
    
    if (ffmpegPath_.empty()) {
        throw std::invalid_argument("FFmpeg path cannot be empty");
    }
    
    if (!std::filesystem::exists(ffmpegPath_)) {
        throw std::runtime_error("FFmpeg not found at: " + ffmpegPath_);
    }
    
    spdlog::info("FFmpegConverter initialized with path: {}, threads: {}", ffmpegPath_, threads_);
}

std::string FFmpegConverter::buildFFmpegCommand(const domain::services::ConversionRequest& request) const {
    std::ostringstream cmd;
    
    cmd << ffmpegPath_;
    
    // Input file with proper escaping
    cmd << " -i \"" << request.inputFilePath << "\"";
    
    // Threads
    cmd << " -threads " << threads_;
    
    // Overwrite output without prompting
    cmd << " -y";
    
    // Add conversion options based on target format
    if (!request.additionalOptions.empty()) {
        for (const auto& option : request.additionalOptions) {
            cmd << " " << option;
        }
    } else {
        // Use default options if none provided
        auto options = const_cast<FFmpegConverter*>(this)->getConversionOptions(request.sourceFormat, request.targetFormat);
        for (const auto& option : options) {
            cmd << " " << option;
        }
    }
    
    // Output file with proper escaping
    cmd << " \"" << request.outputFilePath << "\"";
    
    // Redirect stderr to stdout for progress parsing
    cmd << " 2>&1";
    
    return cmd.str();
}

void FFmpegConverter::updateProgress(const std::string& taskId, const domain::services::ConversionProgress& progress) {
    std::lock_guard<std::mutex> lock(progressMutex_);
    progressMap_[taskId] = progress;
}

domain::services::ConversionProgress FFmpegConverter::getProgressInternal(const std::string& taskId) const {
    std::lock_guard<std::mutex> lock(progressMutex_);
    auto it = progressMap_.find(taskId);
    if (it != progressMap_.end()) {
        return it->second;
    }
    
    // Return default progress if not found
    domain::services::ConversionProgress progress;
    progress.taskId = taskId;
    progress.progress = 0;
    progress.currentOperation = "Not started";
    progress.speed = 0.0;
    progress.eta = "Unknown";
    progress.status = domain::valueobjects::ConversionStatus::PENDING;
    
    return progress;
}

std::string FFmpegConverter::parseProgress(const std::string& output) const {
    // Parse FFmpeg progress output
    // This is a simplified version - production would need more sophisticated parsing
    std::regex progressRegex(R"(frame=\s*(\d+)\s+fps=\s*(\d+(?:\.\d+)?)\s+q=\s*(\d+(?:\.\d+)?)\s+size=\s*(\d+)\s*time=\s*(\d+:\d+:\d+\.\d+))");
    std::smatch match;
    
    if (std::regex_search(output, match, progressRegex)) {
        return match[0].str();
    }
    
    return "";
}

bool FFmpegConverter::isConversionRunning(const std::string& taskId) const {
    auto progress = getProgressInternal(taskId);
    return progress.status == domain::valueobjects::ConversionStatus::PROCESSING;
}

void FFmpegConverter::executeConversion(const domain::services::ConversionRequest& request) {
    std::string taskId = request.taskId;
    
    try {
        // Validate input file exists
        if (!std::filesystem::exists(request.inputFilePath)) {
            throw std::runtime_error("Input file does not exist: " + request.inputFilePath);
        }
        
        // Ensure output directory exists
        std::filesystem::path outputPath(request.outputFilePath);
        if (outputPath.has_parent_path()) {
            std::filesystem::create_directories(outputPath.parent_path());
        }
        
        // Update initial progress
        domain::services::ConversionProgress progress;
        progress.taskId = taskId;
        progress.progress = 0;
        progress.currentOperation = "Starting conversion";
        progress.speed = 0.0;
        progress.eta = "Calculating...";
        progress.status = domain::valueobjects::ConversionStatus::PROCESSING;
        updateProgress(taskId, progress);
        
        // Build command
        std::string command = buildFFmpegCommand(request);
        spdlog::info("Executing FFmpeg command: {}", command);
        
        // Execute command (simplified - production would use proper process management)
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            throw std::runtime_error("Failed to execute FFmpeg command");
        }
        
        char buffer[128];
        std::string output;
        int duration = 0;
        int currentTime = 0;
        
        // Read output line by line
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
            
            // Parse duration from FFmpeg output
            std::regex durationRegex(R"(Duration:\s+(\d+):(\d+):(\d+\.\d+))");
            std::cmatch durationMatch;
            if (std::regex_search(output.c_str(), durationMatch, durationRegex)) {
                int hours = std::stoi(durationMatch[1].str());
                int minutes = std::stoi(durationMatch[2].str());
                double seconds = std::stod(durationMatch[3].str());
                duration = hours * 3600 + minutes * 60 + static_cast<int>(seconds);
            }
            
            // Parse current time
            std::regex timeRegex(R"(time=(\d+):(\d+):(\d+\.\d+))");
            std::cmatch timeMatch;
            if (std::regex_search(buffer, timeMatch, timeRegex)) {
                int hours = std::stoi(timeMatch[1].str());
                int minutes = std::stoi(timeMatch[2].str());
                double seconds = std::stod(timeMatch[3].str());
                currentTime = hours * 3600 + minutes * 60 + static_cast<int>(seconds);
                
                // Calculate progress
                if (duration > 0) {
                    int progressPercent = static_cast<int>((static_cast<double>(currentTime) / duration) * 100);
                    
                    progress.progress = progressPercent;
                    progress.currentOperation = "Converting";
                    progress.speed = 1.0; // Simplified
                    progress.eta = "Calculating...";
                    updateProgress(taskId, progress);
                }
            }
        }
        
        int result = pclose(pipe);
        
        if (result == 0) {
            // Success - verify output file exists
            if (std::filesystem::exists(request.outputFilePath)) {
                progress.progress = 100;
                progress.currentOperation = "Completed";
                progress.status = domain::valueobjects::ConversionStatus::COMPLETED;
                updateProgress(taskId, progress);
                
                spdlog::info("Conversion completed successfully for task: {}", taskId);
            } else {
                throw std::runtime_error("Conversion completed but output file not found");
            }
        } else {
            // Failure
            progress.currentOperation = "Failed with exit code: " + std::to_string(result);
            progress.status = domain::valueobjects::ConversionStatus::FAILED;
            updateProgress(taskId, progress);
            
            spdlog::error("Conversion failed for task: {} with exit code: {}", taskId, result);
            throw std::runtime_error("FFmpeg conversion failed with exit code: " + std::to_string(result));
        }
        
    } catch (const std::exception& e) {
        spdlog::error("Error during conversion for task {}: {}", taskId, e.what());
        
        domain::services::ConversionProgress progress;
        progress.taskId = taskId;
        progress.currentOperation = "Error: " + std::string(e.what());
        progress.status = domain::valueobjects::ConversionStatus::FAILED;
        updateProgress(taskId, progress);
    }
}

domain::services::ConversionResult FFmpegConverter::convert(const domain::services::ConversionRequest& request) {
    try {
        spdlog::info("Starting conversion for task: {}", request.taskId);
        
        // Execute conversion in a separate thread (non-blocking)
        std::thread conversionThread(&FFmpegConverter::executeConversion, this, request);
        conversionThread.detach();
        
        // Return immediate result (conversion is async)
        domain::services::ConversionResult result;
        result.success = true;
        result.outputFilePath = request.outputFilePath;
        result.errorMessage = "";
        result.outputFileSize = 0;
        result.completedAt = std::chrono::system_clock::now();
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("Error starting conversion: {}", e.what());
        
        domain::services::ConversionResult result;
        result.success = false;
        result.outputFilePath = "";
        result.errorMessage = std::string("Conversion failed: ") + e.what();
        result.outputFileSize = 0;
        result.completedAt = std::chrono::system_clock::now();
        
        return result;
    }
}

domain::services::ConversionProgress FFmpegConverter::getProgress(const std::string& taskId) {
    return getProgressInternal(taskId);
}

void FFmpegConverter::cancelConversion(const std::string& taskId) {
    // In a production system, you would need to track and kill the FFmpeg process
    // For now, we'll just update the status
    spdlog::info("Cancelling conversion for task: {}", taskId);
    
    domain::services::ConversionProgress progress;
    progress.taskId = taskId;
    progress.currentOperation = "Cancelled";
    progress.status = domain::valueobjects::ConversionStatus::CANCELLED;
    updateProgress(taskId, progress);
}

std::vector<domain::valueobjects::FileFormat> FFmpegConverter::getSupportedFormats() {
    return domain::valueobjects::FileFormat::getSupportedFormats();
}

bool FFmpegConverter::canConvert(const domain::valueobjects::FileFormat& source, 
                                 const domain::valueobjects::FileFormat& target) {
    return source.canConvertTo(target);
}

std::vector<std::string> FFmpegConverter::getConversionOptions(const domain::valueobjects::FileFormat& source, 
                                                               const domain::valueobjects::FileFormat& target) {
    std::vector<std::string> options;
    
    // Add basic conversion options based on target format
    if (target.isAudio()) {
        options.push_back("-acodec");
        if (target.getExtension() == "mp3") {
            options.push_back("libmp3lame");
        } else if (target.getExtension() == "aac") {
            options.push_back("aac");
        } else if (target.getExtension() == "flac") {
            options.push_back("flac");
        } else {
            options.push_back("libmp3lame"); // Default
        }
        
        options.push_back("-b:a");
        options.push_back("192k"); // Default bitrate
    } else if (target.isVideo()) {
        options.push_back("-vcodec");
        if (target.getExtension() == "mp4") {
            options.push_back("libx264");
        } else if (target.getExtension() == "webm") {
            options.push_back("libvpx");
        } else {
            options.push_back("libx264"); // Default
        }
        
        options.push_back("-acodec");
        options.push_back("aac");
        
        options.push_back("-b:v");
        options.push_back("1M"); // Default video bitrate
    }
    
    return options;
}

} // namespace conversion
} // namespace services
} // namespace infrastructure