#!/bin/bash

echo "=== Online File Converter Integration Test ==="

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test configuration
BACKEND_URL="http://localhost:8080"
TEST_FILE="./test_audio.mp3"

echo -e "${YELLOW}Testing backend connectivity...${NC}"

# Test health endpoint
echo "Testing health endpoint..."
HEALTH_RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "$BACKEND_URL/api/health" 2>/dev/null || echo "000")

if [ "$HEALTH_RESPONSE" = "200" ] || [ "$HEALTH_RESPONSE" = "404" ]; then
    echo -e "${GREEN}✓ Backend is running${NC}"
else
    echo -e "${RED}✗ Backend is not accessible (HTTP $HEALTH_RESPONSE)${NC}"
    echo "Make sure the backend is running on $BACKEND_URL"
    exit 1
fi

# Test auth endpoints
echo -e "${YELLOW}Testing authentication endpoints...${NC}"

echo "Testing user registration..."
REGISTER_RESPONSE=$(curl -s -X POST "$BACKEND_URL/api/auth/register" \
    -H "Content-Type: application/json" \
    -d '{"email":"test@example.com","password":"TestPassword123","name":"Test User"}' 2>/dev/null)

if echo "$REGISTER_RESPONSE" | grep -q "success"; then
    echo -e "${GREEN}✓ User registration works${NC}"
else
    echo -e "${YELLOW}⚠ User registration response: $REGISTER_RESPONSE${NC}"
fi

echo "Testing user login..."
LOGIN_RESPONSE=$(curl -s -X POST "$BACKEND_URL/api/auth/login" \
    -H "Content-Type: application/json" \
    -d '{"email":"test@example.com","password":"TestPassword123"}' 2>/dev/null)

if echo "$LOGIN_RESPONSE" | grep -q "success"; then
    echo -e "${GREEN}✓ User login works${NC}"
    TOKEN=$(echo "$LOGIN_RESPONSE" | grep -o '"accessToken":"[^"]*' | cut -d'"' -f4)
    echo "  Token received: ${TOKEN:0:20}..."
else
    echo -e "${YELLOW}⚠ User login response: $LOGIN_RESPONSE${NC}"
fi

# Test file upload endpoint
echo -e "${YELLOW}Testing file upload endpoint...${NC}"

if [ -f "$TEST_FILE" ]; then
    echo "Testing file upload with $TEST_FILE..."
    UPLOAD_RESPONSE=$(curl -s -X POST "$BACKEND_URL/api/files/upload" \
        -F "file=@$TEST_FILE" 2>/dev/null)
    
    if echo "$UPLOAD_RESPONSE" | grep -q "success"; then
        echo -e "${GREEN}✓ File upload works${NC}"
        FILE_ID=$(echo "$UPLOAD_RESPONSE" | grep -o '"fileId":"[^"]*' | cut -d'"' -f4)
        echo "  File ID: $FILE_ID"
    else
        echo -e "${YELLOW}⚠ File upload response: $UPLOAD_RESPONSE${NC}"
    fi
else
    echo -e "${YELLOW}⚠ Test file not found: $TEST_FILE${NC}"
    echo "Skipping file upload test"
fi

# Test conversion endpoint
echo -e "${YELLOW}Testing conversion endpoint...${NC}"

if [ ! -z "$FILE_ID" ]; then
    echo "Starting conversion for file $FILE_ID..."
    CONVERSION_RESPONSE=$(curl -s -X POST "$BACKEND_URL/api/conversion/start" \
        -H "Content-Type: application/json" \
        -d "{\"fileId\":\"$FILE_ID\",\"targetFormat\":\"wav\"}" 2>/dev/null)
    
    if echo "$CONVERSION_RESPONSE" | grep -q "success"; then
        echo -e "${GREEN}✓ Conversion start works${NC}"
        TASK_ID=$(echo "$CONVERSION_RESPONSE" | grep -o '"taskId":"[^"]*' | cut -d'"' -f4)
        echo "  Task ID: $TASK_ID"
    else
        echo -e "${YELLOW}⚠ Conversion response: $CONVERSION_RESPONSE${NC}"
    fi
else
    echo -e "${YELLOW}⚠ No file ID available, skipping conversion test${NC}"
fi

echo -e "${GREEN}=== Integration test completed ===${NC}"