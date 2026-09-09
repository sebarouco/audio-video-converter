#pragma once

#include <string>
#include <vector>
#include <unordered_set>

namespace domain {
namespace valueobjects {

enum class MediaType {
    AUDIO,
    VIDEO,
    IMAGE,
    DOCUMENT,
    UNKNOWN
};

class FileFormat {
private:
    std::string extension_;
    std::string mimeType_;
    MediaType mediaType_;
    
    static const std::unordered_set<std::string> SUPPORTED_AUDIO_FORMATS;
    static const std::unordered_set<std::string> SUPPORTED_VIDEO_FORMATS;
    
public:
    FileFormat();
    explicit FileFormat(const std::string& extension);
    FileFormat(const std::string& extension, const std::string& mimeType, MediaType mediaType);
    
    std::string getExtension() const { return extension_; }
    std::string getMimeType() const { return mimeType_; }
    MediaType getMediaType() const { return mediaType_; }
    
    bool isAudio() const { return mediaType_ == MediaType::AUDIO; }
    bool isVideo() const { return mediaType_ == MediaType::VIDEO; }
    bool isImage() const { return mediaType_ == MediaType::IMAGE; }
    bool isDocument() const { return mediaType_ == MediaType::DOCUMENT; }
    
    bool isSupported() const;
    bool canConvertTo(const FileFormat& target) const;
    
    static std::vector<FileFormat> getSupportedFormats();
    static std::vector<FileFormat> getSupportedAudioFormats();
    static std::vector<FileFormat> getSupportedVideoFormats();
    static FileFormat fromExtension(const std::string& extension);
    
    bool operator==(const FileFormat& other) const;
    bool operator!=(const FileFormat& other) const;
};

} // namespace valueobjects
} // namespace domain