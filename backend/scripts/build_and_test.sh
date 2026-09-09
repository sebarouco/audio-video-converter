#!/bin/bash

echo "=== Building and Testing Online File Converter ==="

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if we're in the backend directory
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}Error: CMakeLists.txt not found. Please run this script from the backend directory.${NC}"
    exit 1
fi

# Create build directory
echo -e "${YELLOW}Creating build directory...${NC}"
mkdir -p build
cd build

# Configure with CMake
echo -e "${YELLOW}Configuring project with CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Debug
if [ $? -ne 0 ]; then
    echo -e "${RED}CMake configuration failed${NC}"
    exit 1
fi

# Build the project
echo -e "${YELLOW}Building project...${NC}"
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Build successful${NC}"

# Run unit tests
echo -e "${YELLOW}Running unit tests...${NC}"
ctest --output-on-failure
if [ $? -ne 0 ]; then
    echo -e "${RED}Unit tests failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ All tests passed${NC}"

echo -e "${GREEN}=== Build and test completed successfully ===${NC}"