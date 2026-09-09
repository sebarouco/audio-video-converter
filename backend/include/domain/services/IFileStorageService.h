#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

namespace domain {
namespace services {

class IFileStorageService {
public:
    virtual ~IFileStorageService() = default;
    
    virtual std::string storeFile(const std::string& filename, 
                                  const std::vector<uint8_t>& data) = 0;
    virtual std::string storeFileFromPath(const std::string& sourcePath,
                                          const std::string& filename) = 0;
    virtual std::vector<uint8_t> retrieveFile(const std::string& filePath) = 0;
    virtual std::optional<std::string> retrieveFilePath(const std::string& filePath) = 0;
    virtual void deleteFile(const std::string& filePath) = 0;
    virtual bool fileExists(const std::string& filePath) = 0;
    virtual size_t getFileSize(const std::string& filePath) = 0;
    virtual std::string getFileName(const std::string& filePath) = 0;
    virtual std::string getFileExtension(const std::string& filePath) = 0;
};

} // namespace services
} // namespace domain