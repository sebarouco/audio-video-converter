'use client'

import { useState, useCallback } from 'react'
import { Upload, X, FileAudio, FileVideo } from 'lucide-react'
import { cn } from '@/lib/utils/cn'

interface FileUploadProps {
  onFileSelect: (file: File) => void
  acceptedFormats?: string[]
  maxSize?: number // in bytes
}

export default function FileUpload({ 
  onFileSelect, 
  acceptedFormats = ['.mp3', '.mp4', '.avi', '.mov', '.wav', '.flac', '.ogg', '.webm', '.mkv'],
  maxSize = 536870912 // 512MB default
}: FileUploadProps) {
  const [isDragging, setIsDragging] = useState(false)
  const [selectedFile, setSelectedFile] = useState<File | null>(null)
  const [error, setError] = useState<string>('')

  const handleDragOver = useCallback((e: React.DragEvent) => {
    e.preventDefault()
    setIsDragging(true)
  }, [])

  const handleDragLeave = useCallback((e: React.DragEvent) => {
    e.preventDefault()
    setIsDragging(false)
  }, [])

  const handleDrop = useCallback((e: React.DragEvent) => {
    e.preventDefault()
    setIsDragging(false)
    setError('')

    const files = Array.from(e.dataTransfer.files)
    if (files.length > 0) {
      validateAndSelectFile(files[0])
    }
  }, [maxSize])

  const handleFileSelect = useCallback((e: React.ChangeEvent<HTMLInputElement>) => {
    setError('')
    if (e.target.files && e.target.files.length > 0) {
      validateAndSelectFile(e.target.files[0])
    }
  }, [maxSize])

  const validateAndSelectFile = (file: File) => {
    // Check file size
    if (file.size > maxSize) {
      setError(`File size exceeds ${maxSize / 1024 / 1024}MB limit`)
      return
    }

    // Check file extension
    const fileExtension = '.' + file.name.split('.').pop()?.toLowerCase()
    if (!acceptedFormats.includes(fileExtension)) {
      setError(`File format not supported. Accepted formats: ${acceptedFormats.join(', ')}`)
      return
    }

    setSelectedFile(file)
    onFileSelect(file)
  }

  const clearFile = () => {
    setSelectedFile(null)
    setError('')
  }

  const getFileIcon = (fileName: string) => {
    const extension = fileName.split('.').pop()?.toLowerCase()
    const audioFormats = ['mp3', 'wav', 'flac', 'ogg', 'aac', 'm4a']
    const videoFormats = ['mp4', 'avi', 'mov', 'mkv', 'webm']
    
    if (audioFormats.includes(extension || '')) {
      return <FileAudio className="h-8 w-8 text-white" />
    } else if (videoFormats.includes(extension || '')) {
      return <FileVideo className="h-8 w-8 text-white" />
    }
    return <Upload className="h-8 w-8 text-white" />
  }

  return (
    <div style={{ width: '100%' }}>
      {!selectedFile ? (
        <div
          onDragOver={handleDragOver}
          onDragLeave={handleDragLeave}
          onDrop={handleDrop}
          style={{
            border: '2px dashed #596577',
            borderRadius: '6px',
            padding: '48px',
            textAlign: 'center',
            transition: 'all 0.3s',
            backgroundColor: '#3A4350',
            cursor: 'pointer'
          }}
        >
          <input
            type="file"
            onChange={handleFileSelect}
            accept={acceptedFormats.join(',')}
            style={{ display: 'none' }}
            id="file-upload"
          />
          <label htmlFor="file-upload" style={{ cursor: 'pointer' }}>
            <Upload style={{ height: '48px', width: '48px', margin: '0 auto 16px', color: '#596577' }} />
            <p style={{ fontSize: '18px', fontWeight: '500', color: '#D0D5DC', marginBottom: '8px' }}>
              Drop your file here or click to browse
            </p>
            <p style={{ fontSize: '14px', color: '#A8AFBA' }}>
              Supported formats: {acceptedFormats.join(', ')}
            </p>
          </label>
        </div>
      ) : (
        <div style={{ border: '1px solid #596577', borderRadius: '6px', padding: '24px', backgroundColor: '#3A4350', transition: 'all 0.3s' }}>
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: '16px' }}>
              {getFileIcon(selectedFile.name)}
              <div>
                <p style={{ fontWeight: '500', color: '#D0D5DC', fontSize: '16px' }}>{selectedFile.name}</p>
                <p style={{ fontSize: '14px', color: '#A8AFBA' }}>
                  {(selectedFile.size / 1024 / 1024).toFixed(2)} MB
                </p>
              </div>
            </div>
            <button
              onClick={clearFile}
              style={{ padding: '8px', borderRadius: '6px', border: 'none', backgroundColor: 'transparent', cursor: 'pointer' }}
            >
              <X style={{ height: '20px', width: '20px', color: '#A8AFBA' }} />
            </button>
          </div>
        </div>
      )}
      
      {error && (
        <div style={{ marginTop: '12px', fontSize: '14px', color: '#fca5a5', backgroundColor: 'rgba(127, 29, 29, 0.3)', border: '1px solid #991b1b', padding: '16px', borderRadius: '6px' }}>
          {error}
        </div>
      )}
    </div>
  )
}