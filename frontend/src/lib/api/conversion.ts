import apiClient from './client'
import type {
  ConversionTask,
  ConversionProgress,
  ConversionResult,
  UploadResponse,
  StartConversionRequest,
  StartConversionResponse,
  ApiResponse,
} from '@/types'

export const conversionApi = {
  // Upload file
  uploadFile: async (file: File): Promise<ApiResponse<UploadResponse>> => {
    const formData = new FormData()
    formData.append('file', file)

    const response = await apiClient.post<ApiResponse<UploadResponse>>(
      '/api/files/upload',
      formData,
      {
        headers: {
          'Content-Type': 'multipart/form-data',
        },
      }
    )
    return response.data
  },

  // Start conversion
  startConversion: async (
    request: StartConversionRequest
  ): Promise<ApiResponse<StartConversionResponse>> => {
    const response = await apiClient.post<ApiResponse<StartConversionResponse>>(
      '/api/conversion/start',
      request
    )
    return response.data
  },

  // Get conversion status
  getConversionStatus: async (taskId: string): Promise<ApiResponse<ConversionTask>> => {
    const response = await apiClient.get<ApiResponse<ConversionTask>>(
      `/api/conversion/${taskId}`
    )
    return response.data
  },

  // Get conversion progress
  getConversionProgress: async (taskId: string): Promise<ApiResponse<ConversionProgress>> => {
    const response = await apiClient.get<ApiResponse<ConversionProgress>>(
      `/api/conversion/${taskId}/progress`
    )
    return response.data
  },

  // Download converted file
  downloadConvertedFile: async (taskId: string): Promise<Blob> => {
    const response = await apiClient.get(`/api/conversion/${taskId}/download`, {
      responseType: 'blob',
    })
    return response.data
  },

  // Cancel conversion
  cancelConversion: async (taskId: string): Promise<ApiResponse<void>> => {
    const response = await apiClient.delete<ApiResponse<void>>(
      `/api/conversion/${taskId}`
    )
    return response.data
  },

  // Get supported formats
  getSupportedFormats: async (): Promise<ApiResponse<string[]>> => {
    const response = await apiClient.get<ApiResponse<string[]>>('/api/formats')
    return response.data
  },
}