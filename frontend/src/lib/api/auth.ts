import apiClient from './client'
import type { User, ApiResponse } from '@/types'

export interface LoginRequest {
  email: string
  password: string
}

export interface RegisterRequest {
  email: string
  password: string
  name: string
}

export interface AuthResponse {
  accessToken: string
  refreshToken: string
  tokenType: string
  expiresAt: string
  user: User
}

export const authApi = {
  // Register user
  register: async (request: RegisterRequest): Promise<ApiResponse<AuthResponse>> => {
    const response = await apiClient.post<ApiResponse<AuthResponse>>(
      '/api/auth/register',
      request
    )
    return response.data
  },

  // Login user
  login: async (request: LoginRequest): Promise<ApiResponse<AuthResponse>> => {
    const response = await apiClient.post<ApiResponse<AuthResponse>>(
      '/api/auth/login',
      request
    )
    return response.data
  },

  // Refresh token
  refreshToken: async (refreshToken: string): Promise<ApiResponse<AuthResponse>> => {
    const response = await apiClient.post<ApiResponse<AuthResponse>>(
      '/api/auth/refresh',
      { refreshToken }
    )
    return response.data
  },

  // Logout
  logout: async (): Promise<ApiResponse<void>> => {
    const response = await apiClient.post<ApiResponse<void>>('/api/auth/logout')
    return response.data
  },
}