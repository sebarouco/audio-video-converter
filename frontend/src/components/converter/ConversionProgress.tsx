'use client'

import { useEffect, useState } from 'react'
import { CheckCircle, XCircle, Clock, Download } from 'lucide-react'
import { conversionApi } from '@/lib/api/conversion'
import type { ConversionProgress } from '@/types'

interface ConversionProgressProps {
  taskId: string
  onComplete?: (result: any) => void
  onError?: (error: string) => void
}

export default function ConversionProgress({ taskId, onComplete, onError }: ConversionProgressProps) {
  const [progress, setProgress] = useState<ConversionProgress | null>(null)
  const [loading, setLoading] = useState(true)

  useEffect(() => {
    const pollProgress = async () => {
      try {
        const response = await conversionApi.getConversionProgress(taskId)
        
        if (response.success && response.data) {
          setProgress(response.data)
          
          if (response.data.status === 'COMPLETED') {
            setLoading(false)
            onComplete?.(response.data)
          } else if (response.data.status === 'FAILED' || response.data.status === 'CANCELLED') {
            setLoading(false)
            onError?.(response.data.currentOperation || 'Conversion failed')
          } else {
            // Continue polling for active tasks
            setTimeout(pollProgress, 1000)
          }
        }
      } catch (error) {
        console.error('Error fetching progress:', error)
        setLoading(false)
        onError?.('Failed to fetch conversion progress')
      }
    }

    pollProgress()
  }, [taskId, onComplete, onError])

  const getStatusIcon = () => {
    if (!progress) return <Clock className="h-5 w-5 text-gray-400" />
    
    switch (progress.status) {
      case 'COMPLETED':
        return <CheckCircle className="h-5 w-5 text-green-500" />
      case 'FAILED':
      case 'CANCELLED':
        return <XCircle className="h-5 w-5 text-red-500" />
      default:
        return <Clock className="h-5 w-5 text-blue-500 animate-spin" />
    }
  }

  const getStatusColor = () => {
    if (!progress) return 'bg-gray-200'
    
    switch (progress.status) {
      case 'COMPLETED':
        return 'bg-green-500'
      case 'FAILED':
      case 'CANCELLED':
        return 'bg-red-500'
      default:
        return 'bg-blue-500'
    }
  }

  if (loading && !progress) {
    return (
      <div className="bg-white rounded-lg p-6 shadow-md">
        <div className="flex items-center justify-center">
          <div className="animate-spin rounded-full h-8 w-8 border-b-2 border-primary-600"></div>
          <span className="ml-3 text-gray-600">Initializing conversion...</span>
        </div>
      </div>
    )
  }

  return (
    <div className="bg-white rounded-lg p-6 shadow-md">
      <div className="flex items-center justify-between mb-4">
        <div className="flex items-center space-x-3">
          {getStatusIcon()}
          <div>
            <h3 className="font-semibold text-gray-900">Conversion Progress</h3>
            <p className="text-sm text-gray-500">Task ID: {taskId}</p>
          </div>
        </div>
        <span className="text-sm font-medium text-gray-700">
          {progress?.progress || 0}%
        </span>
      </div>

      <div className="mb-4">
        <div className="w-full bg-gray-200 rounded-full h-2">
          <div
            className={`${getStatusColor()} h-2 rounded-full transition-all duration-300`}
            style={{ width: `${progress?.progress || 0}%` }}
          ></div>
        </div>
      </div>

      {progress && (
        <div className="space-y-2 text-sm">
          <div className="flex justify-between">
            <span className="text-gray-600">Status:</span>
            <span className="font-medium text-gray-900">{progress.status}</span>
          </div>
          <div className="flex justify-between">
            <span className="text-gray-600">Current Operation:</span>
            <span className="font-medium text-gray-900">{progress.currentOperation}</span>
          </div>
          {progress.speed > 0 && (
            <div className="flex justify-between">
              <span className="text-gray-600">Speed:</span>
              <span className="font-medium text-gray-900">{progress.speed.toFixed(2)}x</span>
            </div>
          )}
          {progress.eta && (
            <div className="flex justify-between">
              <span className="text-gray-600">ETA:</span>
              <span className="font-medium text-gray-900">{progress.eta}</span>
            </div>
          )}
        </div>
      )}

      {progress?.status === 'COMPLETED' && (
        <button
          onClick={() => conversionApi.downloadConvertedFile(taskId)}
          className="mt-4 w-full flex items-center justify-center space-x-2 bg-primary-600 text-white py-2 rounded-lg hover:bg-primary-700 transition"
        >
          <Download className="h-5 w-5" />
          <span>Download Converted File</span>
        </button>
      )}
    </div>
  )
}