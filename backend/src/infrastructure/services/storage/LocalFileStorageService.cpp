#include "infrastructure/services/storage/LocalFileStorageService.h"
#include <fstream>
#include <filesystem>
#include <random>
#include <spdlog/spdlog.h>
#include <chrono>

namespace infrastructure {
namespace services {
namespace storage {

LocalFileStorageService::LocalFileStorageService(const std::string& uploadDir,
                                                   const std::string& convertedDir,
                                                   const std::string& tempDir,
                                                   size_t maxFileSize)
    : uploadDir_(uploadDir), convertedDir_(convertedDir), tempDir_(tempDir), maxFileSize_(maxFileSize) {
    
    // Ensure directories exist
    ensureDirectoryExists(uploadDir_);
    ensureDirectoryExists(convertedDir_);
    ensureDirectoryExists(tempDir_);
    
    spdlog::info("LocalFileStorageService initialized with upload: {}, converted: {}, temp: {}", 
                 uploadDir_, convertedDir_, tempDir_);
}

std::string LocalFileStorageService::generateUniqueFileName(const std::string& originalName) {
    // Generate timestamp
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    // Generate random string
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::string randomStr;
    for (int i = 0; i < 8; i++) {
        randomStr += std::to_string(dis(gen));
    }
    
    // Extract extension
    size_t dotPos = originalName.find_last_of('.');
    std::string extension = (dotPos != std::string::npos) ? originalName.substr(dotPos) : "";
    
    return std::to_string(timestamp) + "_" + randomStr + extension;
}

std::string LocalFileStorageService::getFullPath(const std::string& filename, const std::string& directory) {
    std::filesystem::path dirPath(directory);
    std::filesystem::path filePath = dirPath / filename;
    return filePath.string();
}

bool LocalFileStorageService::ensureDirectoryExists(const std::string& directory) {
    try {
        std::filesystem::path dirPath(directory);
        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
            spdlog::info("Created directory: {}", directory);
        }
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to create directory {}: {}", directory, e.what());
        return false;
    }
}

bool LocalFileStorageService::isValidFileSize(size_t size) {
    return size > 0 && size <= maxFileSize_;
}

std::string LocalFileStorageService::storeFile(const std::string& filename, 
                                                const std::vector<uint8_t>& data) {
    try {
        if (!isValidFileSize(data.size())) {
            spdlog::warn("File size {} exceeds maximum allowed size", data.size());
            return "";
        }
        
        std::string uniqueFileName = generateUniqueFileName(filename);
        std::string fullPath = getFullPath(uniqueFileName, uploadDir_);
        
        std::ofstream file(fullPath, std::ios::binary);
        if (!file) {
            spdlog::error("Failed to create file: {}", fullPath);
            return "";
        }
        
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
        
        spdlog::info("File stored successfully: {} (size: {} bytes)", fullPath, data.size());
        return fullPath;
        
    } catch (const std::exception& e) {
        spdlog::error("Error storing file: {}", e.what());
        return "";
    }
}

std::string LocalFileStorageService::storeFileFromPath(const std::string& sourcePath,
                                                       const std::string& filename) {
    try {
        if (!std::filesystem::exists(sourcePath)) {
            spdlog::error("Source file does not exist: {}", sourcePath);
            return "";
        }
        
        size_t fileSize = std::filesystem::file_size(sourcePath);
        if (!isValidFileSize(fileSize)) {
            spdlog::warn("File size {} exceeds maximum allowed size", fileSize);
            return "";
        }
        
        std::string uniqueFileName = generateUniqueFileName(filename);
        std::string fullPath = getFullPath(uniqueFileName, uploadDir_);
        
        std::filesystem::copy_file(sourcePath, fullPath);
        
        spdlog::info("File copied from {} to {}", sourcePath, fullPath);
        return fullPath;
        
    } catch (const std::exception& e) {
        spdlog::error("Error storing file from path: {}", e.what());
        return "";
    }
}

std::vector<uint8_t> LocalFileStorageService::retrieveFile(const std::string& filePath) {
    try {
        if (!std::filesystem::exists(filePath)) {
            spdlog::error("File does not exist: {}", filePath);
            return {};
        }
        
        size_t fileSize = std::filesystem::file_size(filePath);
        std::vector<uint8_t> data(fileSize);
        
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            spdlog::error("Failed to open file: {}", filePath);
            return {};
        }
        
        file.read(reinterpret_cast<char*>(data.data()), fileSize);
        file.close();
        
        spdlog::info("File retrieved successfully: {} (size: {} bytes)", filePath, fileSize);
        return data;
        
    } catch (const std::exception& e) {
        spdlog::error("Error retrieving file: {}", e.what());
        return {};
    }
}

std::optional<std::string> LocalFileStorageService::retrieveFilePath(const std::string& filePath) {
    if (std::filesystem::exists(filePath)) {
        return filePath;
    }
    return std::nullopt;
}

void LocalFileStorageService::deleteFile(const std::string& filePath) {
    try {
        if (std::filesystem::exists(filePath)) {
            std::filesystem::remove(filePath);
            spdlog::info("File deleted: {}", filePath);
        } else {
            spdlog::warn("File does not exist for deletion: {}", filePath);
        }
    } catch (const std::exception& e) {
        spdlog::error("Error deleting file: {}", e.what());
    }
}

bool LocalFileStorageService::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

size_t LocalFileStorageService::getFileSize(const std::string& filePath) {
    try {
        if (std::filesystem::exists(filePath)) {
            return std::filesystem::file_size(filePath);
        }
        return 0;
    } catch (const std::exception& e) {
        spdlog::error("Error getting file size: {}", e.what());
        return 0;
    }
}

std::string LocalFileStorageService::getFileName(const std::string& filePath) {
    try {
        std::filesystem::path path(filePath);
        return path.filename().string();
    } catch (const std::exception& e) {
        spdlog::error("Error getting file name: {}", e.what());
        return "";
    }
}

std::string LocalFileStorageService::getFileExtension(const std::string& filePath) {
    try {
        std::filesystem::path path(filePath);
        return path.extension().string();
    } catch (const std::exception& e) {
        spdlog::error("Error getting file extension: {}", e.what());
        return "";
    }
}

void LocalFileStorageService::cleanupOldFiles(int hoursOld) {
    try {
        auto now = std::chrono::system_clock::now();
        auto cutoff = now - std::chrono::hours(hoursOld);
        
        std::vector<std::string> directories = {uploadDir_, convertedDir_, tempDir_};
        
        for (const auto& directory : directories) {
            if (!std::filesystem::exists(directory)) {
                continue;
            }
            
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    auto ftime = std::filesystem::last_write_time(entry);
                    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                        ftime - std::filesystem::file_time_type::clock::now()
                        + std::chrono::system_clock::now());
                    
                    if (sctp < cutoff) {
                        std::filesystem::remove(entry);
                        spdlog::info("Deleted old file: {}", entry.path().string());
                    }
                }
            }
        }
        
        spdlog::info("Cleanup completed for files older than {} hours", hoursOld);
        
    } catch (const std::exception& e) {
        spdlog::error("Error during cleanup: {}", e.what());
    }
}

} // namespace storage
} // namespace services
} // namespace infrastructure