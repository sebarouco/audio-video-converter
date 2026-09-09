#include "domain/valueobjects/FileFormat.h"
#include <algorithm>
#include <spdlog/spdlog.h>

namespace domain {
namespace valueobjects {

// Initialize static members
const std::unordered_set<std::string> FileFormat::SUPPORTED_AUDIO_FORMATS = {
    "mp3", "wav", "flac", "ogg", "aac", "m4a", "wma", "opus", "alac"
};

const std::unordered_set<std::string> FileFormat::SUPPORTED_VIDEO_FORMATS = {
    "mp4", "avi", "mov", "mkv", "webm", "flv", "wmv", "m4v", "mpeg"
};

FileFormat::FileFormat() : extension_(""), mimeType_("application/octet-stream"), mediaType_(MediaType::UNKNOWN) {
}

FileFormat::FileFormat(const std::string& extension) {
    extension_ = extension;
    std::transform(extension_.begin(), extension_.end(), extension_.begin(), ::tolower);
    
    // Remove leading dot if present
    if (!extension_.empty() && extension_[0] == '.') {
        extension_ = extension_.substr(1);
    }
    
    // Determine media type and MIME type
    if (SUPPORTED_AUDIO_FORMATS.find(extension_) != SUPPORTED_AUDIO_FORMATS.end()) {
        mediaType_ = MediaType::AUDIO;
        mimeType_ = "audio/" + extension_;
    } else if (SUPPORTED_VIDEO_FORMATS.find(extension_) != SUPPORTED_VIDEO_FORMATS.end()) {
        mediaType_ = MediaType::VIDEO;
        mimeType_ = "video/" + extension_;
    } else {
        mediaType_ = MediaType::UNKNOWN;
        mimeType_ = "application/octet-stream";
    }
}

FileFormat::FileFormat(const std::string& extension, const std::string& mimeType, MediaType mediaType)
    : extension_(extension), mimeType_(mimeType), mediaType_(mediaType) {
    std::transform(extension_.begin(), extension_.end(), extension_.begin(), ::tolower);
    
    if (!extension_.empty() && extension_[0] == '.') {
        extension_ = extension_.substr(1);
    }
}

bool FileFormat::isSupported() const {
    return mediaType_ == MediaType::AUDIO || mediaType_ == MediaType::VIDEO;
}

bool FileFormat::canConvertTo(const FileFormat& target) const {
    if (!isSupported() || !target.isSupported()) {
        return false;
    }
    
    // Audio to audio conversions are supported
    if (isAudio() && target.isAudio()) {
        return true;
    }
    
    // Video to video conversions are supported
    if (isVideo() && target.isVideo()) {
        return true;
    }
    
    // Video to audio (extract audio) is supported
    if (isVideo() && target.isAudio()) {
        return true;
    }
    
    return false;
}

std::vector<FileFormat> FileFormat::getSupportedFormats() {
    std::vector<FileFormat> formats;
    
    for (const auto& ext : SUPPORTED_AUDIO_FORMATS) {
        formats.emplace_back(ext);
    }
    
    for (const auto& ext : SUPPORTED_VIDEO_FORMATS) {
        formats.emplace_back(ext);
    }
    
    return formats;
}

std::vector<FileFormat> FileFormat::getSupportedAudioFormats() {
    std::vector<FileFormat> formats;
    
    for (const auto& ext : SUPPORTED_AUDIO_FORMATS) {
        formats.emplace_back(ext);
    }
    
    return formats;
}

std::vector<FileFormat> FileFormat::getSupportedVideoFormats() {
    std::vector<FileFormat> formats;
    
    for (const auto& ext : SUPPORTED_VIDEO_FORMATS) {
        formats.emplace_back(ext);
    }
    
    return formats;
}

bool FileFormat::operator==(const FileFormat& other) const {
    return extension_ == other.extension_ && mediaType_ == other.mediaType_;
}

bool FileFormat::operator!=(const FileFormat& other) const {
    return !(*this == other);
}

FileFormat FileFormat::fromExtension(const std::string& extension) {
    return FileFormat(extension);
}

} // namespace valueobjects
} // namespace domain