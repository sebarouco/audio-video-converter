#include <gtest/gtest.h>
#include "domain/entities/User.h"
#include <stdexcept>
#include <thread>
#include <chrono>

using namespace domain::entities;

TEST(UserTest, ConstructorValidatesEmail) {
    EXPECT_THROW(User("id", "invalid-email", "hash", "name"), std::invalid_argument);
}

TEST(UserTest, ConstructorValidatesPasswordHash) {
    EXPECT_THROW(User("id", "test@example.com", "short", "name"), std::invalid_argument);
}

TEST(UserTest, ConstructorValidatesName) {
    EXPECT_THROW(User("id", "test@example.com", "validhash123456789012345678901234567890123456789012345678901234567890", ""), std::invalid_argument);
}

TEST(UserTest, ConstructorWithValidData) {
    EXPECT_NO_THROW(User("id", "test@example.com", "validhash123456789012345678901234567890123456789012345678901234567890", "Valid Name"));
}

TEST(UserTest, SetEmailValidatesFormat) {
    User user("id", "test@example.com", "validhash123456789012345678901234567890123456789012345678901234567890", "Valid Name");
    EXPECT_THROW(user.setEmail("invalid-email"), std::invalid_argument);
    EXPECT_NO_THROW(user.setEmail("new@example.com"));
}

TEST(UserTest, SetPasswordHashValidatesLength) {
    User user("id", "test@example.com", "validhash123456789012345678901234567890123456789012345678901234567890", "Valid Name");
    EXPECT_THROW(user.setPasswordHash("short"), std::invalid_argument);
    EXPECT_NO_THROW(user.setPasswordHash("validhash123456789012345678901234567890123456789012345678901234567890"));
}

TEST(UserTest, SetNameValidatesFormat) {
    User user("id", "test@example.com", "validhash123456789012345678901234567890123456789012345678901234567890", "Valid Name");
    EXPECT_THROW(user.setName(""), std::invalid_argument);
    EXPECT_THROW(user.setName("Invalid123"), std::invalid_argument);
    EXPECT_NO_THROW(user.setName("Valid Name"));
}

TEST(UserTest, GettersReturnCorrectValues) {
    User user("test-id", "test@example.com", "validhash123456789012345678901234567890123456789012345678901234567890", "Valid Name");
    
    EXPECT_EQ(user.getId(), "test-id");
    EXPECT_EQ(user.getEmail(), "test@example.com");
    EXPECT_EQ(user.getPasswordHash(), "validhash123456789012345678901234567890123456789012345678901234567890");
    EXPECT_EQ(user.getName(), "Valid Name");
}

TEST(UserTest, UpdateTimestampUpdatesTime) {
    User user("id", "test@example.com", "validhash123456789012345678901234567890123456789012345678901234567890", "Valid Name");
    auto oldTime = user.getUpdatedAt();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    user.updateTimestamp();
    
    auto newTime = user.getUpdatedAt();
    EXPECT_GT(newTime, oldTime);
}