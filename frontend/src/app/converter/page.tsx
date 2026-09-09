'use client'

import { useState } from 'react'
import { useAuth } from '@/lib/hooks/useAuth'
import { conversionApi } from '@/lib/api/conversion'
import FileUpload from '@/components/converter/FileUpload'
import ConversionProgress from '@/components/converter/ConversionProgress'
import Button from '@/components/ui/Button'
import { ArrowLeft, Download } from 'lucide-react'
import Link from 'next/link'

export default function ConverterPage() {
  // const { isAuthenticated, user } = useAuth() // Temporarily disabled auth
  const [selectedFile, setSelectedFile] = useState<File | null>(null)
  const [targetFormat, setTargetFormat] = useState('mp3')
  const [taskId, setTaskId] = useState<string | null>(null)
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState<string>('')

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
      console.log('File size:', selectedFile.size)
      console.log('File type:', selectedFile.type)
      
      // Upload file
      console.log('About to call uploadFile API...')
      const uploadResponse = await conversionApi.uploadFile(selectedFile)
      console.log('Upload response received:', uploadResponse)
      
      if (!uploadResponse.success || !uploadResponse.data) {
        console.error('Upload failed:', uploadResponse)
        throw new Error(uploadResponse.error || 'File upload failed')
      }

      console.log('File uploaded successfully, fileId:', uploadResponse.data.fileId)

      // Start conversion
      console.log('About to call startConversion API...')
      const conversionResponse = await conversionApi.startConversion({
        fileId: uploadResponse.data.fileId,
        targetFormat,
      })

      console.log('Conversion response:', conversionResponse)

      if (!conversionResponse.success || !conversionResponse.data) {
        console.error('Conversion start failed:', conversionResponse)
        throw new Error(conversionResponse.error || 'Conversion start failed')
      }

      setTaskId(conversionResponse.data.taskId)
    } catch (err: any) {
      console.error('Conversion error:', err)
      console.error('Error details:', err.response?.data || err.message)
      setError(err.message || 'An error occurred during conversion')
    } finally {
      setLoading(false)
    }
  }

  const handleConversionComplete = (result: any) => {
    console.log('Conversion completed:', result)
  }

  const handleConversionError = (error: string) => {
    setError(error)
  }

  const handleReset = () => {
    setSelectedFile(null)
    setTaskId(null)
    setError('')
  }

  // if (!isAuthenticated) {
  //   return (
  //     <div className="min-h-screen bg-gradient-to-br from-blue-50 to-indigo-100 flex items-center justify-center">
  //       <div className="bg-white p-8 rounded-lg shadow-md max-w-md">
  //         <h2 className="text-2xl font-bold mb-4">Authentication Required</h2>
  //         <p className="text-gray-600 mb-6">
  //           Please log in to use the file converter.
  //         </p>
  //         <div className="flex gap-4">
  //           <Link href="/login" className="flex-1">
  //             <Button className="w-full">Login</Button>
  //           </Link>
  //           <Link href="/register" className="flex-1">
  //             <Button variant="secondary" className="w-full">Register</Button>
  //           </Link>
  //         </div>
  //       </div>
  //     </div>
  //   )
  // }

  return (
    <div className="min-h-screen bg-gradient-to-br from-blue-50 to-indigo-100">
      {/* Header */}
      <header className="bg-white shadow-sm">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-4">
          <div className="flex justify-between items-center">
            <Link href="/" className="flex items-center space-x-2 text-gray-700 hover:text-gray-900">
              <ArrowLeft className="h-5 w-5" />
              <span>Back to Home</span>
            </Link>
            <div className="flex items-center space-x-4">
              <span className="text-gray-700">Audio & Video Convert</span>
            </div>
          </div>
        </div>
      </header>

      {/* Main Content */}
      <main className="max-w-4xl mx-auto px-4 sm:px-6 lg:px-8 py-12">
        <div className="bg-white rounded-lg shadow-md p-8">
          <h1 className="text-3xl font-bold text-gray-900 mb-6">File Converter</h1>

          {!taskId ? (
            <>
              <div className="space-y-6">
                <FileUpload 
                  onFileSelect={handleFileSelect}
                  acceptedFormats={['.mp3', '.mp4', '.avi', '.mov', '.wav', '.flac', '.ogg', '.webm', '.mkv']}
                />

                {selectedFile && (
                  <div className="space-y-4">
                    <div>
                      <label className="block text-sm font-medium text-gray-700 mb-2">
                        Target Format
                      </label>
                      <select
                        value={targetFormat}
                        onChange={(e) => setTargetFormat(e.target.value)}
                        className="w-full px-4 py-2 border border-gray-300 rounded-lg focus:ring-2 focus:ring-primary-500 focus:border-transparent"
                      >
                        <option value="mp3">MP3</option>
                        <option value="wav">WAV</option>
                        <option value="flac">FLAC</option>
                        <option value="ogg">OGG</option>
                        <option value="m4a">M4A</option>
                        <option value="mp4">MP4</option>
                        <option value="avi">AVI</option>
                        <option value="mov">MOV</option>
                        <option value="webm">WebM</option>
                        <option value="mkv">MKV</option>
                      </select>
                    </div>

                    <Button
                      onClick={handleStartConversion}
                      isLoading={loading}
                      disabled={!selectedFile}
                      className="w-full"
                    >
                      Start Conversion
                    </Button>
                  </div>
                )}

                {error && (
                  <div className="bg-red-50 border border-red-200 text-red-700 px-4 py-3 rounded">
                    {error}
                  </div>
                )}
              </div>
            </>
          ) : (
            <div className="space-y-4">
              <ConversionProgress
                taskId={taskId}
                onComplete={handleConversionComplete}
                onError={handleConversionError}
              />

              {error && (
                <div className="bg-red-50 border border-red-200 text-red-700 px-4 py-3 rounded">
                  {error}
                </div>
              )}

              <Button
                onClick={handleReset}
                variant="secondary"
                className="w-full"
              >
                Convert Another File
              </Button>
            </div>
          )}
        </div>
      </main>
    </div>
  )
}