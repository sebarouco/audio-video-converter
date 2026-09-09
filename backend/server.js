const express = require('express');
const cors = require('cors');
const multer = require('multer');
const path = require('path');
const fs = require('fs');
const { exec } = require('child_process');
const { promisify } = require('util');

const execAsync = promisify(exec);
const app = express();
const PORT = 8080;

// Middleware
app.use(cors());
app.use(express.json());

// Create necessary directories
const uploadsDir = path.join(__dirname, 'uploads');
const convertedDir = path.join(__dirname, 'converted');

[uploadsDir, convertedDir].forEach(dir => {
    if (!fs.existsSync(dir)) {
        fs.mkdirSync(dir, { recursive: true });
    }
});

// Configure multer for file uploads
const storage = multer.diskStorage({
    destination: (req, file, cb) => {
        cb(null, uploadsDir);
    },
    filename: (req, file, cb) => {
        const fileId = 'file_' + Date.now() + '_' + Math.random().toString(36).substr(2, 9);
        cb(null, fileId + path.extname(file.originalname));
    }
});

const upload = multer({ storage: storage });

// Store conversion tasks in memory
const conversionTasks = new Map();

// Health check endpoint
app.get('/api/health', (req, res) => {
    res.json({
        status: 'healthy',
        message: 'Backend is running'
    });
});

// File upload endpoint
app.post('/api/files/upload', upload.single('file'), (req, res) => {
    console.log('File upload request received');
    console.log('Request file:', req.file);
    console.log('Request body:', req.body);
    
    if (!req.file) {
        console.log('No file in request');
        return res.status(400).json({
            success: false,
            message: 'No file uploaded'
        });
    }

    console.log('File uploaded successfully:', req.file.filename);
    
    res.json({
        success: true,
        message: 'File uploaded successfully',
        data: {
            fileId: req.file.filename,
            fileName: req.file.originalname,
            filePath: req.file.path
        }
    });
});

// Start conversion endpoint
app.post('/api/conversion/start', async (req, res) => {
    console.log('Conversion start request received:', req.body);
    
    const { fileId, targetFormat } = req.body;
    
    if (!fileId || !targetFormat) {
        console.log('Missing required parameters');
        return res.status(400).json({
            success: false,
            message: 'Missing required parameters'
        });
    }

    const taskId = 'task_' + Date.now() + '_' + Math.random().toString(36).substr(2, 9);
    const inputPath = path.join(uploadsDir, fileId);
    const outputPath = path.join(convertedDir, `${taskId}.${targetFormat}`);

    console.log(`Starting conversion: ${inputPath} -> ${outputPath}`);

    // Verify input file exists
    if (!fs.existsSync(inputPath)) {
        console.log('Input file not found:', inputPath);
        return res.status(404).json({
            success: false,
            message: 'Input file not found'
        });
    }

    // Initialize task
    conversionTasks.set(taskId, {
        taskId,
        status: 'processing',
        progress: 0,
        outputPath,
        targetFormat
    });

    console.log(`Task ${taskId} initialized, starting conversion in background`);

    // Start conversion in background
    performConversion(taskId, inputPath, outputPath, targetFormat).catch(err => {
        console.error(`Conversion error for task ${taskId}:`, err);
        const task = conversionTasks.get(taskId);
        if (task) {
            task.status = 'failed';
            task.error = err.message;
        }
    });

    res.json({
        success: true,
        message: 'Conversion started',
        data: {
            taskId: taskId,
            status: 'processing'
        }
    });
});

// Perform FFmpeg conversion
async function performConversion(taskId, inputPath, outputPath, targetFormat) {
    const task = conversionTasks.get(taskId);
    
    try {
        // Build FFmpeg command based on target format
        let ffmpegCommand = `ffmpeg -i "${inputPath}" -y`;
        
        // Add format-specific options
        const audioFormats = ['mp3', 'wav', 'aac', 'ogg', 'flac', 'm4a', 'wma', 'opus', 'alac'];
        const videoFormats = ['mp4', 'avi', 'mov', 'mkv', 'webm', 'flv', 'wmv', 'm4v', 'mpeg'];
        
        if (audioFormats.includes(targetFormat.toLowerCase())) {
            // Audio conversion or extraction
            ffmpegCommand += ' -vn'; // No video
            ffmpegCommand += ' -acodec';
            
            switch (targetFormat.toLowerCase()) {
                case 'mp3':
                    ffmpegCommand += ' libmp3lame -b:a 192k';
                    break;
                case 'wav':
                    ffmpegCommand += ' pcm_s16le';
                    break;
                case 'aac':
                    ffmpegCommand += ' aac -b:a 192k';
                    break;
                case 'ogg':
                    ffmpegCommand += ' libvorbis -b:a 192k';
                    break;
                case 'flac':
                    ffmpegCommand += ' flac';
                    break;
                default:
                    ffmpegCommand += ' libmp3lame -b:a 192k';
            }
        } else if (videoFormats.includes(targetFormat.toLowerCase())) {
            // Video conversion
            ffmpegCommand += ' -vcodec';
            
            switch (targetFormat.toLowerCase()) {
                case 'mp4':
                case 'mov':
                case 'm4v':
                    ffmpegCommand += ' libx264 -acodec aac -b:v 1M';
                    break;
                case 'webm':
                    ffmpegCommand += ' libvpx -acodec libvorbis -b:v 1M';
                    break;
                case 'avi':
                    ffmpegCommand += ' libx264 -acodec aac -b:v 1M';
                    break;
                case 'mkv':
                    ffmpegCommand += ' libx264 -acodec aac -b:v 1M';
                    break;
                default:
                    ffmpegCommand += ' libx264 -acodec aac -b:v 1M';
            }
        }
        
        ffmpegCommand += ` "${outputPath}"`;
        
        console.log(`Executing FFmpeg for task ${taskId}: ${ffmpegCommand}`);
        
        // Execute FFmpeg
        await execAsync(ffmpegCommand);
        
        // Update task status
        if (fs.existsSync(outputPath)) {
            task.status = 'completed';
            task.progress = 100;
            console.log(`Conversion completed for task ${taskId}`);
        } else {
            throw new Error('Output file not created');
        }
        
    } catch (error) {
        console.error(`Conversion failed for task ${taskId}:`, error);
        task.status = 'failed';
        task.error = error.message;
    }
}

// Get conversion progress endpoint
app.get('/api/conversion/progress', (req, res) => {
    const { taskId } = req.query;
    
    if (!taskId) {
        return res.status(400).json({
            success: false,
            message: 'Missing taskId parameter'
        });
    }

    const task = conversionTasks.get(taskId);
    
    if (!task) {
        return res.status(404).json({
            success: false,
            message: 'Task not found'
        });
    }

    res.json({
        success: true,
        data: {
            taskId: task.taskId,
            status: task.status,
            progress: task.progress,
            error: task.error
        }
    });
});

// Download converted file endpoint
app.get('/api/conversion/:taskId/download', (req, res) => {
    const { taskId } = req.params;
    
    const task = conversionTasks.get(taskId);
    
    if (!task || task.status !== 'completed') {
        return res.status(404).json({
            success: false,
            message: 'Conversion not completed or task not found'
        });
    }

    if (!fs.existsSync(task.outputPath)) {
        return res.status(404).json({
            success: false,
            message: 'Converted file not found'
        });
    }

    const fileExtension = path.extname(task.outputPath).substring(1);
    const mimeType = getMimeType(fileExtension);
    
    res.setHeader('Content-Type', mimeType);
    res.setHeader('Content-Disposition', `attachment; filename="converted_${taskId}.${fileExtension}"`);
    
    const fileStream = fs.createReadStream(task.outputPath);
    fileStream.pipe(res);
});

// Helper function to get MIME type
function getMimeType(extension) {
    const mimeTypes = {
        'mp3': 'audio/mpeg',
        'wav': 'audio/wav',
        'aac': 'audio/aac',
        'ogg': 'audio/ogg',
        'flac': 'audio/flac',
        'm4a': 'audio/mp4',
        'wma': 'audio/x-ms-wma',
        'opus': 'audio/opus',
        'alac': 'audio/alac',
        'mp4': 'video/mp4',
        'avi': 'video/x-msvideo',
        'mov': 'video/quicktime',
        'mkv': 'video/x-matroska',
        'webm': 'video/webm',
        'flv': 'video/x-flv',
        'wmv': 'video/x-ms-wmv',
        'm4v': 'video/mp4',
        'mpeg': 'video/mpeg'
    };
    
    return mimeTypes[extension.toLowerCase()] || 'application/octet-stream';
}

// Start server
app.listen(PORT, () => {
    console.log('Online File Converter Backend with FFmpeg started');
    console.log(`Server running on http://0.0.0.0:${PORT}`);
    console.log('API endpoints:');
    console.log('  GET  /api/health');
    console.log('  POST /api/files/upload');
    console.log('  POST /api/conversion/start');
    console.log('  GET  /api/conversion/progress?taskId={taskId}');
    console.log('  GET  /api/conversion/{taskId}/download');
});