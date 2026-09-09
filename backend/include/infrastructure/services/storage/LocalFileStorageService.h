#pragma once

#include "domain/services/IFileStorageService.h"
#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace infrastructure {
namespace services {
namespace storage {

class LocalFileStorageService : public domain::services::IFileStorageService {
private:
    std::string uploadDir_;
    std::string convertedDir_;
    std::string tempDir_;
    size_t maxFileSize_;
    
    std::string generateUniqueFileName(const std::string& originalName);
    std::string getFullPath(const std::string& filename, const std::string& directory);
    bool ensureDirectoryExists(const std::string& directory);
    bool isValidFileSize(size_t size);
    
public:
    LocalFileStorageService(const std::string& uploadDir, 
                           const std::string& convertedDir,
                           const std::string& tempDir,
                           size_t maxFileSize = 536870912); // 512MB default
    
    ~LocalFileStorageService() override = default;
    
    std::string storeFile(const std::string& filename, 
                          const std::vector<uint8_t>& data) override;
    std::string storeFileFromPath(const std::string& sourcePath,
                                 const std::string& filename) override;
    std::vector<uint8_t> retrieveFile(const std::string& filePath) override;
    std::optional<std::string> retrieveFilePath(const std::string& filePath) override;
    void deleteFile(const std::string& filePath) override;
    bool fileExists(const std::string& filePath) override;
    size_t getFileSize(const std::string& filePath) override;
    std::string getFileName(const std::string& filePath) override;
    std::string getFileExtension(const std::string& filePath) override;
    
    // Utility methods
    void cleanupOldFiles(int hoursOld = 48);
    std::string getUploadDir() const { return uploadDir_; }
    std::string getConvertedDir() const { return convertedDir_; }
    std::string getTempDir() const { return tempDir_; }
};

} // namespace storage
} // namespace services
} // namespace infrastructure