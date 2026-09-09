const express = require('express');
const cors = require('cors');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const multer = require('multer');

const app = express();
const PORT = 8080;
const UPLOAD_DIR = './data/uploads';
const CONVERTED_DIR = './data/converted';

// Create directories if they don't exist
[UPLOAD_DIR, CONVERTED_DIR].forEach(dir => {
  if (!fs.existsSync(dir)) {
    fs.mkdirSync(dir, { recursive: true });
  }
});

app.use(cors());
app.use(express.json({ limit: '50mb' }));

// Configure multer for file uploads
const storage = multer.diskStorage({
  destination: (req, file, cb) => {
    cb(null, UPLOAD_DIR);
  },
  filename: (req, file, cb) => {
    const fileId = `file_${Date.now()}`;
    const ext = path.extname(file.originalname);
    cb(null, `${fileId}_${file.originalname}`);
  }
});

const upload = multer({ 
  storage: storage,
  limits: { fileSize: 500 * 1024 * 1024 } // 500MB limit
});

// Health check
app.get('/api/health', (req, res) => {
  res.json({ success: true, message: 'Server is running' });
});

// Get supported formats
app.get('/api/formats', (req, res) => {
  res.json({
    success: true,
    message: 'Supported formats retrieved successfully',
    data: ['mp3', 'wav', 'flac', 'ogg', 'm4a', 'mp4', 'avi', 'mov', 'webm', 'mkv']
  });
});

// File upload endpoint
app.post('/api/files/upload', upload.single('file'), (req, res) => {
  try {
    if (!req.file) {
      return res.status(400).json({
        success: false,
        message: 'No file uploaded',
        error: 'File is required'
      });
    }
    
    const fileId = `file_${Date.now()}`;
    const filePath = req.file.path;
    const fileName = req.file.originalname;
    const fileSize = req.file.size;
    const format = path.extname(fileName).slice(1);
    
    // Rename file to include fileId
    const newFilePath = path.join(UPLOAD_DIR, `${fileId}_${fileName}`);
    fs.renameSync(filePath, newFilePath);
    
    res.json({
      success: true,
      message: 'File uploaded successfully',
      data: {
        fileId,
        fileName,
        filePath: newFilePath,
        fileSize,
        format
      }
    });
  } catch (error) {
    console.error('Upload error:', error);
    res.status(500).json({
      success: false,
      message: 'File upload failed',
      error: error.message
    });
  }
});

// Start conversion
app.post('/api/conversion/start', (req, res) => {
  try {
    const { fileId, targetFormat } = req.body;
    
    if (!fileId || !targetFormat) {
      return res.status(400).json({
        success: false,
        message: 'Missing required fields',
        error: 'fileId and targetFormat are required'
      });
    }
    
    const taskId = `task_${Date.now()}`;
    
    // Find the uploaded file using fileId
    const files = fs.readdirSync(UPLOAD_DIR);
    const uploadedFile = files.find(file => file.startsWith(fileId));
    
    if (!uploadedFile) {
      return res.status(404).json({
        success: false,
        message: 'Uploaded file not found',
        error: `File with ID: ${fileId}`
      });
    }
    
    const inputFilePath = path.join(UPLOAD_DIR, uploadedFile);
    const outputFilePath = path.join(CONVERTED_DIR, `${taskId}.${targetFormat}`);
    
    // Store task for later retrieval
    if (!global.conversionTasks) {
      global.conversionTasks = new Map();
    }
    global.conversionTasks.set(taskId, {
      inputFilePath,
      outputFilePath,
      targetFormat,
      status: 'PROCESSING',
      progress: 0
    });
    
    // Start FFmpeg conversion in background
    const ffmpegCmd = `ffmpeg -i "${inputFilePath}" "${outputFilePath}" -y`;
    console.log('Starting FFmpeg:', ffmpegCmd);
    exec(ffmpegCmd, (error, stdout, stderr) => {
      const task = global.conversionTasks.get(taskId);
      if (task) {
        if (error) {
          console.error('FFmpeg error:', error);
          task.status = 'FAILED';
          task.error = error.message;
        } else {
          console.log('FFmpeg completed successfully');
          task.status = 'COMPLETED';
          task.progress = 100;
        }
      }
    });
    
    res.json({
      success: true,
      message: 'Conversion started successfully',
      data: {
        taskId,
        status: 'PROCESSING',
        message: 'Conversion started'
      }
    });
  } catch (error) {
    console.error('Conversion start error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to start conversion',
      error: error.message
    });
  }
});

// Get conversion progress
app.get('/api/conversion/:id/progress', (req, res) => {
  try {
    const { id } = req.params;
    const task = global.conversionTasks?.get(id);
    
    if (!task) {
      return res.status(404).json({
        success: false,
        message: 'Conversion task not found',
        error: `Task ID: ${id}`
      });
    }
    
    res.json({
      success: true,
      message: 'Conversion progress retrieved successfully',
      data: {
        taskId: id,
        progress: task.progress,
        currentOperation: task.status === 'PROCESSING' ? 'Converting' : 
                           task.status === 'COMPLETED' ? 'Completed' : 'Failed',
        speed: 1.0,
        eta: 'Calculating...',
        status: task.status.toLowerCase()
      }
    });
  } catch (error) {
    console.error('Progress error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get conversion progress',
      error: error.message
    });
  }
});

// Get conversion status
app.get('/api/conversion/:id', (req, res) => {
  try {
    const { id } = req.params;
    const task = global.conversionTasks?.get(id);
    
    if (!task) {
      return res.status(404).json({
        success: false,
        message: 'Conversion task not found',
        error: `Task ID: ${id}`
      });
    }
    
    res.json({
      success: true,
      message: 'Conversion status retrieved successfully',
      data: {
        taskId: id,
        status: task.status,
        progress: task.progress,
        message: task.status === 'PROCESSING' ? 'Conversion in progress' :
                  task.status === 'COMPLETED' ? 'Conversion completed' : 'Conversion failed'
      }
    });
  } catch (error) {
    console.error('Status error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get conversion status',
      error: error.message
    });
  }
});

// Download converted file
app.get('/api/conversion/:id/download', (req, res) => {
  try {
    const { id } = req.params;
    const task = global.conversionTasks?.get(id);
    
    if (!task || task.status !== 'COMPLETED') {
      return res.status(404).json({
        success: false,
        message: 'Converted file not found or conversion not completed',
        error: 'Task not found or incomplete'
      });
    }
    
    if (!fs.existsSync(task.outputFilePath)) {
      return res.status(404).json({
        success: false,
        message: 'Converted file not found',
        error: 'File path: ' + task.outputFilePath
      });
    }
    
    const ext = path.extname(task.outputFilePath);
    const contentType = getContentType(ext);
    
    res.download(task.outputFilePath, `converted_${id}${ext}`);
  } catch (error) {
    console.error('Download error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to download file',
      error: error.message
    });
  }
});

// Cancel conversion
app.delete('/api/conversion/:id', (req, res) => {
  try {
    const { id } = req.params;
    
    if (global.conversionTasks?.has(id)) {
      global.conversionTasks.get(id).status = 'CANCELLED';
    }
    
    res.json({
      success: true,
      message: 'Conversion cancelled successfully'
    });
  } catch (error) {
    console.error('Cancel error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to cancel conversion',
      error: error.message
    });
  }
});

function getContentType(ext) {
  const types = {
    '.mp3': 'audio/mpeg',
    '.wav': 'audio/wav',
    '.flac': 'audio/flac',
    '.ogg': 'audio/ogg',
    '.m4a': 'audio/mp4',
    '.mp4': 'video/mp4',
    '.avi': 'video/x-msvideo',
    '.mov': 'video/quicktime',
    '.webm': 'video/webm',
    '.mkv': 'video/x-matroska'
  };
  return types[ext] || 'application/octet-stream';
}

app.listen(PORT, '127.0.0.1', () => {
  console.log(`Proxy server running on http://127.0.0.1:${PORT}`);
});