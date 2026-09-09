FROM ubuntu:22.04 AS base

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libssl-dev \
    libpq-dev \
    libhiredis-dev \
    ffmpeg \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libswresample-dev \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy CMakeLists.txt and source files
COPY CMakeLists.txt ./
COPY include/ ./include/
COPY src/ ./src/
COPY cmake/ ./cmake/
COPY config/ ./config/

# Build the application
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Runtime stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libssl3 \
    libpq5 \
    libhiredis0.14 \
    ffmpeg \
    libavcodec58 \
    libavformat58 \
    libavutil56 \
    libswresample3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the built application
COPY --from=base /app/build/online_file_converter ./bin/
COPY config/ ./config/

# Create necessary directories
RUN mkdir -p data/uploads data/converted data/temp logs

# Expose port
EXPOSE 8080

# Run the application
CMD ["./bin/online_file_converter"]