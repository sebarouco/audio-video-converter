'use client'

import { useState } from 'react'
import FileUpload from '@/components/converter/FileUpload'
import ConversionProgress from '@/components/converter/ConversionProgress'
import Button from '@/components/ui/Button'
import { Download, Music, Video } from 'lucide-react'

export default function Home() {
  const [selectedFile, setSelectedFile] = useState<File | null>(null)
  const [targetFormat, setTargetFormat] = useState('')
  const [taskId, setTaskId] = useState<string | null>(null)
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState<string>('')
  const [downloadReady, setDownloadReady] = useState(false)

  const handleFileSelect = (file: File) => {
    setSelectedFile(file)
    setError('')
  }

  const handleStartConversion = async () => {
    if (!selectedFile) return

    setLoading(true)
    setError('')

    try {
      console.log('Starting conversion for file:', selectedFile.name)
      
      // Upload file first
      const formData = new FormData()
      formData.append('file', selectedFile)
      
      console.log('Uploading file to backend...')
      const uploadResponse = await fetch('http://127.0.0.1:8080/api/files/upload', {
        method: 'POST',
        body: formData,
      })

      if (!uploadResponse.ok) {
        throw new Error('File upload failed')
      }

      const uploadData = await uploadResponse.json()
      console.log('Upload response:', uploadData)
      
      if (!uploadData.success || !uploadData.data) {
        throw new Error(uploadData.message || 'File upload failed')
      }

      console.log('File uploaded successfully, fileId:', uploadData.data.fileId)

      // Start conversion
      console.log('Starting conversion with fileId:', uploadData.data.fileId)
      const conversionResponse = await fetch('http://127.0.0.1:8080/api/conversion/start', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          fileId: uploadData.data.fileId,
          targetFormat: targetFormat,
        }),
      })

      if (!conversionResponse.ok) {
        throw new Error('Conversion start failed')
      }

      const conversionData = await conversionResponse.json()
      console.log('Conversion response:', conversionData)
      
      if (!conversionData.success || !conversionData.data) {
        throw new Error(conversionData.message || 'Conversion start failed')
      }

      setTaskId(conversionData.data.taskId)
      
      // Poll for conversion progress
      const pollProgress = async () => {
        try {
          const progressResponse = await fetch(`http://127.0.0.1:8080/api/conversion/${conversionData.data.taskId}/progress`)
          const progressData = await progressResponse.json()
          
          console.log('Progress:', progressData)
          
          if (progressData.success && progressData.data) {
            if (progressData.data.status === 'completed' || progressData.data.status === 'COMPLETED') {
              setDownloadReady(true)
              setLoading(false)
            } else if (progressData.data.status === 'failed' || progressData.data.status === 'FAILED') {
              throw new Error(progressData.data.error || 'Conversion failed')
            } else {
              // Continue polling
              setTimeout(pollProgress, 2000)
            }
          }
        } catch (error) {
          console.error('Progress polling error:', error)
          throw error
        }
      }
      
      setTimeout(pollProgress, 2000)
      
    } catch (err: any) {
      console.error('Conversion error:', err)
      setError(err.message || 'An error occurred during conversion')
      setLoading(false)
    }
  }

  const handleDownload = async () => {
    if (taskId) {
      try {
        const downloadResponse = await fetch(`http://127.0.0.1:8080/api/conversion/${taskId}/download`)
        
        if (!downloadResponse.ok) {
          throw new Error('Download failed')
        }

        const blob = await downloadResponse.blob()
        const url = window.URL.createObjectURL(blob)
        const link = document.createElement('a')
        link.href = url
        
        const originalName = selectedFile?.name.substring(0, selectedFile.name.lastIndexOf('.')) || 'converted'
        link.download = `${originalName}.${targetFormat}`
        link.click()
        window.URL.revokeObjectURL(url)
        
      } catch (err: any) {
        setError(err.message || 'Download failed')
      }
    }
  }

  const handleReset = () => {
    setSelectedFile(null)
    setTaskId(null)
    setDownloadReady(false)
    setError('')
  }

  const formatOptions = [
    { value: 'webm', label: 'WEBM' },
    { value: 'mov', label: 'MOV' },
    { value: 'avi', label: 'AVI' },
    { value: 'mkv', label: 'MKV' },
    { value: 'mp3', label: 'MP3 (Audio)' },
    { value: 'wav', label: 'WAV (Audio)' },
    { value: 'aac', label: 'AAC (Audio)' },
    { value: 'ogg', label: 'OGG (Audio)' },
    { value: 'flac', label: 'FLAC (Audio)' },
  ]

  return (
    <div style={{ minHeight: '100vh', backgroundColor: '#181E2A', display: 'flex', alignItems: 'center', justifyContent: 'center', padding: '16px' }}>
      <div style={{ width: '100%', textAlign: 'center' }}>
        <div style={{ backgroundColor: '#27313D', borderRadius: '14px', boxShadow: '0 8px 30px rgba(0, 0, 0, 0.20)', padding: '40px', width: '850px', maxWidth: 'calc(100% - 40px)', margin: '0 auto', textAlign: 'center' }}>
          {/* Header */}
          <div style={{ marginBottom: '32px' }}>
            <h1 style={{ fontSize: '32px', fontWeight: '700', color: '#D9DDE3', marginBottom: '16px', textAlign: 'center' }}>
              Audio & Video Converter
            </h1>
            <p style={{ fontSize: '20px', fontWeight: '400', color: '#A8AFBA', textAlign: 'center' }}>
              Convert your audio and video files to different formats
            </p>
          </div>

          {!downloadReady ? (
            <div style={{ display: 'flex', flexDirection: 'column', gap: '32px' }}>
              {/* File Upload Area */}
              <FileUpload
                onFileSelect={handleFileSelect}
                acceptedFormats={['.mp3', '.mp4', '.avi', '.mov', '.wav', '.flac', '.ogg', '.webm', '.mkv']}
              />

              {selectedFile && (
                <div style={{ display: 'flex', flexDirection: 'column', gap: '28px' }}>
                  {/* Format Selection */}
                  <div>
                    <select
                      value={targetFormat}
                      onChange={(e) => setTargetFormat(e.target.value)}
                      style={{ width: '100%', padding: '14px', height: '52px', border: '1px solid #596577', borderRadius: '6px', backgroundColor: '#3A4350', color: '#D0D5DC', fontSize: '18px' }}
                    >
                      <option value="">Select format</option>
                      {formatOptions.map(option => (
                        <option key={option.value} value={option.value}>
                          {option.label}
                        </option>
                      ))}
                    </select>
                  </div>

                  {/* Convert Button */}
                  <button
                    onClick={handleStartConversion}
                    disabled={!selectedFile || !targetFormat || loading}
                    style={{ width: '100%', height: '52px', fontSize: '18px', fontWeight: '600', backgroundColor: '#3459B5', color: '#DDE3EC', borderRadius: '7px', border: 'none', cursor: 'pointer', opacity: (!selectedFile || !targetFormat || loading) ? 0.5 : 1 }}
                  >
                    {loading ? 'Converting...' : 'Convert File'}
                  </button>
                </div>
              )}

              {error && (
                <div style={{ backgroundColor: 'rgba(127, 29, 29, 0.5)', border: '1px solid #991b1b', color: '#fecaca', padding: '16px 24px', borderRadius: '8px', textAlign: 'center' }}>
                  {error}
                </div>
              )}
            </div>
          ) : (
            <div style={{ display: 'flex', flexDirection: 'column', gap: '24px' }}>
              {/* Success Message */}
              <div style={{ backgroundColor: 'rgba(34, 197, 94, 0.2)', border: '2px solid #22c55e', borderRadius: '8px', padding: '24px', textAlign: 'center' }}>
                <div style={{ display: 'inline-flex', alignItems: 'center', justifyContent: 'center', width: '64px', height: '64px', backgroundColor: '#22c55e', borderRadius: '50%', marginBottom: '16px' }}>
                  <svg style={{ width: '32px', height: '32px', color: 'white' }} fill="none" stroke="currentColor" viewBox="0 0 24 24">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M5 13l4 4L19 7"></path>
                  </svg>
                </div>
                <h3 style={{ fontSize: '20px', fontWeight: '600', color: '#86efac', marginBottom: '8px' }}>
                  Conversion Complete!
                </h3>
                <p style={{ color: '#bbf7d0' }}>
                  Your file has been successfully converted to {targetFormat.toUpperCase()}
                </p>
              </div>

              {/* Download Button */}
              <button
                onClick={handleDownload}
                style={{ width: '100%', height: '52px', fontSize: '18px', fontWeight: '600', backgroundColor: '#3459B5', color: '#DDE3EC', borderRadius: '7px', border: 'none', cursor: 'pointer', display: 'flex', alignItems: 'center', justifyContent: 'center', gap: '8px' }}
              >
                <Download style={{ height: '20px', width: '20px' }} />
                Download Converted File
              </button>

              {/* Convert Another Button */}
              <button
                onClick={handleReset}
                style={{ width: '100%', height: '52px', fontSize: '18px', fontWeight: '600', backgroundColor: '#3459B5', color: '#DDE3EC', borderRadius: '7px', border: 'none', cursor: 'pointer' }}
              >
                Convert Another File
              </button>
            </div>
          )}
        </div>
      </div>
    </div>
  )
}