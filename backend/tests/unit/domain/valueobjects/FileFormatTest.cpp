#include <gtest/gtest.h>
#include "domain/valueobjects/FileFormat.h"
#include <algorithm>

using namespace domain::valueobjects;

TEST(FileFormatTest, ConstructorWithValidExtension) {
    FileFormat format("mp3");
    EXPECT_EQ(format.getExtension(), "mp3");
    EXPECT_TRUE(format.isAudio());
    EXPECT_FALSE(format.isVideo());
}

TEST(FileFormatTest, ConstructorWithVideoExtension) {
    FileFormat format("mp4");
    EXPECT_EQ(format.getExtension(), "mp4");
    EXPECT_TRUE(format.isVideo());
    EXPECT_FALSE(format.isAudio());
}

TEST(FileFormatTest, ConstructorNormalizesExtension) {
    FileFormat format1("MP3");
    FileFormat format2(".mp3");
    FileFormat format3("Mp3");
    
    EXPECT_EQ(format1.getExtension(), "mp3");
    EXPECT_EQ(format2.getExtension(), "mp3");
    EXPECT_EQ(format3.getExtension(), "mp3");
}

TEST(FileFormatTest, IsSupportedReturnsCorrectValue) {
    FileFormat mp3("mp3");
    FileFormat mp4("mp4");
    FileFormat txt("txt");
    
    EXPECT_TRUE(mp3.isSupported());
    EXPECT_TRUE(mp4.isSupported());
    EXPECT_FALSE(txt.isSupported());
}

TEST(FileFormatTest, CanConvertToSameMediaType) {
    FileFormat mp3("mp3");
    FileFormat wav("wav");
    FileFormat mp4("mp4");
    FileFormat avi("avi");
    
    EXPECT_TRUE(mp3.canConvertTo(wav));
    EXPECT_TRUE(mp4.canConvertTo(avi));
}

TEST(FileFormatTest, CanConvertVideoToAudio) {
    FileFormat mp4("mp4");
    FileFormat mp3("mp3");
    
    EXPECT_TRUE(mp4.canConvertTo(mp3));
}

TEST(FileFormatTest, CannotConvertAudioToVideo) {
    FileFormat mp3("mp3");
    FileFormat mp4("mp4");
    
    EXPECT_FALSE(mp3.canConvertTo(mp4));
}

TEST(FileFormatTest, EqualityOperator) {
    FileFormat format1("mp3");
    FileFormat format2("mp3");
    FileFormat format3("wav");
    
    EXPECT_TRUE(format1 == format2);
    EXPECT_FALSE(format1 == format3);
}

TEST(FileFormatTest, InequalityOperator) {
    FileFormat format1("mp3");
    FileFormat format2("wav");
    
    EXPECT_TRUE(format1 != format2);
}

TEST(FileFormatTest, GetSupportedFormatsReturnsList) {
    auto formats = FileFormat::getSupportedFormats();
    EXPECT_GT(formats.size(), 0);
    
    bool hasMp3 = std::find_if(formats.begin(), formats.end(), 
        [](const FileFormat& f) { return f.getExtension() == "mp3"; }) != formats.end();
    bool hasMp4 = std::find_if(formats.begin(), formats.end(), 
        [](const FileFormat& f) { return f.getExtension() == "mp4"; }) != formats.end();
    
    EXPECT_TRUE(hasMp3);
    EXPECT_TRUE(hasMp4);
}

TEST(FileFormatTest, GetSupportedAudioFormats) {
    auto formats = FileFormat::getSupportedAudioFormats();
    EXPECT_GT(formats.size(), 0);
    
    for (const auto& format : formats) {
        EXPECT_TRUE(format.isAudio());
        EXPECT_FALSE(format.isVideo());
    }
}

TEST(FileFormatTest, GetSupportedVideoFormats) {
    auto formats = FileFormat::getSupportedVideoFormats();
    EXPECT_GT(formats.size(), 0);
    
    for (const auto& format : formats) {
        EXPECT_TRUE(format.isVideo());
        EXPECT_FALSE(format.isAudio());
    }
}