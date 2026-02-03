# Audio & Video Converter 🎵🎬

A modern web-based file converter that transforms audio and video files between different formats. Built with FastAPI backend and HTML/CSS/JavaScript frontend, featuring a sleek dark theme and intuitive drag-and-drop interface.

https://www.youtube.com/watch?v=SzKijNaoJW4

## ✨ Features

- 🎵 **Audio Conversions**: MP3, WAV, AAC, OGG, FLAC
- 🎬 **Video Conversions**: MP4, WebM, MOV, AVI, MKV
- 🎧 **Video-to-Audio Extraction**: Extract audio from any video
- 🎨 **Modern Dark Theme**: Sleek, professional interface
- 📁 **Drag & Drop**: Intuitive file upload experience
- 📊 **Progress Tracking**: Real-time conversion progress
- 🚀 **Fast Processing**: Powered by FFmpeg for efficient conversions
- 📱 **Responsive Design**: Works on desktop and mobile devices

## 🏗️ Architecture

### Backend (FastAPI)
- **FastAPI**: Modern, fast web framework for APIs
- **FFmpeg**: Powerful multimedia processing engine
- **Python**: Core backend logic and file handling
- **CORS**: Cross-origin resource sharing for frontend communication

### Frontend (HTML/CSS/JavaScript)
- **Vanilla JavaScript**: No framework dependencies
- **Tailwind CSS**: Modern utility-first styling
- **RemixIcon**: Beautiful icon library
- **Responsive Design**: Mobile-friendly interface

## 🚀 Quick Start

### Prerequisites
- Python 3.7+
- FFmpeg (installed and accessible in system PATH)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/audio-video-converter.git
   cd audio-video-converter
   ```

2. **Navigate to backend directory**
   ```bash
   cd file-converter-app/backend
   ```

3. **Install Python dependencies**
   ```bash
   pip install -r requirements.txt
   ```

4. **Install FFmpeg**
   
   **Windows:**
   ```bash
   # Download from https://ffmpeg.org/download.html
   # Add to system PATH
   ```
   
   **macOS:**
   ```bash
   brew install ffmpeg
   ```
   
   **Linux:**
   ```bash
   sudo apt update
   sudo apt install ffmpeg
   ```

5. **Start the server**
   ```bash
   python run.py
   ```

6. **Open your browser**
   Navigate to `http://127.0.0.1:8000`

## 📁 Project Structure

```
file-converter-app/
├── backend/
│   ├── main.py              # FastAPI application and API endpoints
│   ├── run.py               # Server startup script
│   ├── requirements.txt     # Python dependencies
│   ├── static/
│   │   └── index.html      # Frontend interface
│   └── uploads/            # Temporary file storage
└── frontend/                # Next.js frontend (optional)
```

## 🔧 How It Works

### 1. File Upload Process
- User drags & drops or selects a file
- Frontend sends file to `/upload` endpoint
- Backend generates unique filename and saves file
- Returns file path for conversion

### 2. Conversion Process
- User selects target format from dropdown
- Frontend sends conversion request to `/convert` endpoint
- Backend routes to appropriate conversion function:
  - **Video-to-Video**: FFmpeg re-encoding with H.264/AAC
  - **Audio-to-Audio**: FFmpeg audio codec conversion
  - **Video-to-Audio**: FFmpeg audio extraction
- Returns converted file path

### 3. Download Process
- Frontend creates download link with converted file path
- User clicks download to get converted file
- Backend serves file via `/download` endpoint

## 🎯 Supported Formats

### Video Formats
| Input | Output | Description |
|-------|--------|-------------|
| MP4 | WebM, MOV, AVI, MKV | Modern web format to various containers |
| WebM | MP4, MOV, AVI, MKV | Web-optimized to standard formats |
| MOV | MP4, WebM, AVI, MKV | Apple QuickTime to universal formats |
| AVI | MP4, WebM, MOV, MKV | Legacy format to modern containers |
| MKV | MP4, WebM, MOV, AVI | Flexible container to standard formats |

### Audio Formats
| Input | Output | Description |
|-------|--------|-------------|
| MP3 | WAV, AAC, OGG, FLAC | Compressed to uncompressed/other formats |
| WAV | MP3, AAC, OGG, FLAC | Uncompressed to compressed formats |
| AAC | MP3, WAV, OGG, FLAC | Apple standard to universal formats |
| OGG | MP3, WAV, AAC, FLAC | Open source to other formats |
| FLAC | MP3, WAV, AAC, OGG | Lossless to compressed formats |

### Video-to-Audio Extraction
- Extract high-quality audio from any video format
- Supports all audio output formats
- Preserves original audio quality when possible

## 🛠️ API Endpoints

### Upload File
```http
POST /upload
Content-Type: multipart/form-data

Response:
{
  "message": "File uploaded successfully",
  "file_path": "uploads/uuid-filename.ext",
  "file_name": "original-filename.ext"
}
```

### Convert File
```http
POST /convert
Content-Type: application/json

{
  "file_path": "uploads/uuid-filename.ext",
  "target_format": "mp3"
}

Response:
{
  "message": "File converted successfully",
  "output_path": "uploads/uuid-filename.mp3"
}
```

### Download File
```http
GET /download/{file_path}

Response: File download with proper MIME type
```

## 🎨 Frontend Features

### User Interface
- **Drag & Drop Zone**: Visual feedback for file uploads
- **File Information**: Display filename and size
- **Format Selection**: Dynamic dropdown based on file type
- **Progress Bar**: Real-time conversion progress
- **Success Notifications**: Clear feedback for completed conversions

### Responsive Design
- Mobile-friendly layout
- Touch-enabled interactions
- Adaptive font sizes and spacing
- Optimized for various screen sizes

## 🔒 Security Considerations

- **File Type Validation**: Only accepts supported audio/video formats
- **Unique Filenames**: UUID-based naming prevents conflicts
- **Temporary Storage**: Files stored only during conversion
- **CORS Configuration**: Controlled cross-origin access
- **Error Handling**: Comprehensive error logging and user feedback

## 🐛 Troubleshooting

### Common Issues

**FFmpeg not found:**
```bash
# Check if FFmpeg is installed
ffmpeg -version

# If not found, install FFmpeg and add to PATH
```

**File conversion fails:**
- Check file format is supported
- Ensure sufficient disk space
- Verify FFmpeg codecs are available

**Server won't start:**
- Check Python version (3.7+)
- Install all requirements: `pip install -r requirements.txt`
- Check port 8000 is not in use

### Logging
- Backend logs displayed in console
- Detailed error messages for debugging
- File operation tracking

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Commit changes: `git commit -am 'Add feature'`
4. Push to branch: `git push origin feature-name`
5. Submit a pull request

## 📝 License

This project is open source and available under the [MIT License](LICENSE).

## 🙏 Acknowledgments

- **FastAPI** - Modern, fast web framework for building APIs
- **FFmpeg** - Complete, cross-platform solution for audio/video conversion
- **Tailwind CSS** - Utility-first CSS framework for rapid UI development
- **RemixIcon** - Beautiful open-source icon library

## 📞 Support

If you encounter any issues or have questions:
- Create an issue on GitHub
- Check the troubleshooting section
- Review the API documentation

---

**Made with ❤️ for the developer community**
