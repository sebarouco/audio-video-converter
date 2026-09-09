# Online File Converter - Backend

Professional C++ backend for online file conversion using Clean Architecture principles.

## Technologies

- **C++20** - Modern C++ with latest features
- **Drogon Framework** - High-performance HTTP web framework
- **CMake** - Build system
- **FFmpeg** - Audio/video conversion
- **JWT** - Authentication
- **PostgreSQL** - Database
- **Redis** - Task queue and caching
- **spdlog** - Logging
- **nlohmann/json** - JSON handling

## Architecture

The project follows Clean Architecture principles with clear separation of concerns:

```
├── domain/           # Business logic and entities
├── application/      # Use cases and application services  
├── infrastructure/   # External services and implementations
├── presentation/     # HTTP controllers and API layer
└── common/          # Shared utilities
```

## Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+)
- CMake 3.20+
- FFmpeg development libraries
- PostgreSQL development libraries
- Redis development libraries
- OpenSSL

## Installation

### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libssl-dev \
    libpq-dev \
    libhiredis-dev \
    ffmpeg \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libswresample-dev
```

### Build

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## Configuration

Edit `config/config.json` to configure:

- Server settings (host, port, threads)
- Database connection
- Redis connection
- File storage paths
- FFmpeg settings
- JWT configuration
- Logging configuration

## Running

```bash
./build/online_file_converter
```

## Docker

Build and run with Docker Compose:

```bash
cd docker
docker-compose up
```

## API Endpoints

### Authentication
- `POST /api/auth/register` - Register new user
- `POST /api/auth/login` - Login user
- `POST /api/auth/refresh` - Refresh access token
- `POST /api/auth/logout` - Logout user

### Files
- `POST /api/files/upload` - Upload file
- `GET /api/files/{id}` - Get file info
- `DELETE /api/files/{id}` - Delete file

### Conversion
- `POST /api/conversion/start` - Start conversion
- `GET /api/conversion/{id}` - Get conversion status
- `GET /api/conversion/{id}/progress` - Get conversion progress
- `GET /api/conversion/{id}/download` - Download converted file
- `DELETE /api/conversion/{id}` - Cancel conversion

### History
- `GET /api/history` - Get conversion history
- `GET /api/history/{id}` - Get conversion details
- `DELETE /api/history/{id}` - Delete history entry

## Development

### Code Style

- Follow C++ Core Guidelines
- Use modern C++20 features
- Follow SOLID principles
- Maintain Clean Architecture

### Testing

```bash
cd build
ctest
```

## License

MIT License