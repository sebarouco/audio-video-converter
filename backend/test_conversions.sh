#!/bin/bash

FILE_ID="file_1788915877822"
API_URL="http://127.0.0.1:8080"

# Array of formats to test
FORMATS=("mp3" "wav" "flac" "ogg" "m4a" "mp4" "avi" "mov" "webm" "mkv")

echo "Starting conversion tests for all formats..."
echo "File ID: $FILE_ID"
echo "=============================================="

for format in "${FORMATS[@]}"; do
    echo ""
    echo "Testing conversion to $format..."
    
    # Start conversion
    response=$(curl -s -X POST "$API_URL/api/conversion/start" \
        -H "Content-Type: application/json" \
        -d "{\"fileId\":\"$FILE_ID\",\"targetFormat\":\"$format\"}")
    
    echo "Start response: $response"
    
    # Extract task ID
    task_id=$(echo $response | grep -o '"taskId":"[^"]*"' | cut -d'"' -f4)
    
    if [ -z "$task_id" ]; then
        echo "❌ Failed to start conversion to $format"
        continue
    fi
    
    echo "Task ID: $task_id"
    
    # Poll for completion
    max_attempts=60
    attempt=0
    status="processing"
    
    while [ "$attempt" -lt "$max_attempts" ] && [ "$status" != "completed" ] && [ "$status" != "failed" ]; do
        sleep 2
        progress_response=$(curl -s "$API_URL/api/conversion/$task_id/progress")
        status=$(echo $progress_response | grep -o '"status":"[^"]*"' | cut -d'"' -f4)
        progress=$(echo $progress_response | grep -o '"progress":[0-9]*' | cut -d':' -f2)
        
        echo "Progress: $progress% - Status: $status"
        attempt=$((attempt + 1))
    done
    
    if [ "$status" = "completed" ]; then
        echo "✅ Conversion to $format completed successfully"
        
        # Check if file exists
        if [ -f "data/converted/${task_id}.${format}" ]; then
            file_size=$(stat -f%z "data/converted/${task_id}.${format}" 2>/dev/null || stat -c%s "data/converted/${task_id}.${format}" 2>/dev/null)
            echo "   File size: $file_size bytes"
        else
            echo "❌ Output file not found for $format"
        fi
    else
        echo "❌ Conversion to $format failed with status: $status"
    fi
    
    echo "----------------------------------------------"
done

echo ""
echo "=============================================="
echo "Conversion tests completed"