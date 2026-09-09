export interface User {
  id: string
  email: string
  name: string
  createdAt: string
  updatedAt: string
}

export interface ConversionTask {
  id: string
  userId: string
  originalFileName: string
  originalFilePath: string
  convertedFilePath: string
  sourceFormat: string
  targetFormat: string
  status: 'PENDING' | 'QUEUED' | 'PROCESSING' | 'COMPLETED' | 'FAILED' | 'CANCELLED' | 'TIMEOUT'
  progress: number
  errorMessage: string
  createdAt: string
  startedAt: string
  completedAt: string
}

export interface ConversionProgress {
  taskId: string
  progress: number
  currentOperation: string
  speed: number
  eta: string
  status: string
}

export interface ConversionResult {
  taskId: string
  success: boolean
  outputFilePath: string
  errorMessage: string
  outputFileSize: number
  completedAt: string
}

export interface ApiResponse<T> {
  success: boolean
  message: string
  data?: T
  error?: string
  timestamp: string
}

export interface UploadResponse {
  fileId: string
  fileName: string
  filePath: string
  fileSize: number
  format: string
}

export interface StartConversionRequest {
  fileId: string
  targetFormat: string
  options?: string[]
}

export interface StartConversionResponse {
  taskId: string
  status: string
  message: string
}