#!/bin/bash

# Script de conversión de archivos multimedia usando FFmpeg
# Uso: ./convert.sh <archivo_entrada> <formato_salida> <archivo_salida>

INPUT_FILE="$1"
OUTPUT_FORMAT="$2"
OUTPUT_FILE="$3"

if [ -z "$INPUT_FILE" ] || [ -z "$OUTPUT_FORMAT" ] || [ -z "$OUTPUT_FILE" ]; then
    echo "Uso: $0 <archivo_entrada> <formato_salida> <archivo_salida>"
    exit 1
fi

# Detectar si es conversión de video a audio, video a video, o audio a audio
INPUT_EXT="${INPUT_FILE##*.}"
AUDIO_FORMATS=("mp3" "wav" "flac" "ogg" "m4a" "aac")
VIDEO_FORMATS=("mp4" "avi" "mov" "mkv" "webm")

# Determinar el tipo de conversión
if [[ " ${AUDIO_FORMATS[@]} " =~ " ${INPUT_EXT} " ]] && [[ " ${AUDIO_FORMATS[@]} " =~ " ${OUTPUT_FORMAT} " ]]; then
    # Audio a Audio
    echo "Convirtiendo audio a audio: $INPUT_FILE -> $OUTPUT_FILE"
    ffmpeg -i "$INPUT_FILE" -c:a libmp3lame -q:a 2 "$OUTPUT_FILE" -y 2>&1
    
elif [[ " ${VIDEO_FORMATS[@]} " =~ " ${INPUT_EXT} " ]] && [[ " ${AUDIO_FORMATS[@]} " =~ " ${OUTPUT_FORMAT} " ]]; then
    # Video a Audio (extraer audio)
    echo "Extrayendo audio de video: $INPUT_FILE -> $OUTPUT_FILE"
    ffmpeg -i "$INPUT_FILE" -vn -c:a libmp3lame -q:a 2 "$OUTPUT_FILE" -y 2>&1
    
elif [[ " ${VIDEO_FORMATS[@]} " =~ " ${INPUT_EXT} " ]] && [[ " ${VIDEO_FORMATS[@]} " =~ " ${OUTPUT_FORMAT} " ]]; then
    # Video a Video
    echo "Convirtiendo video a video: $INPUT_FILE -> $OUTPUT_FILE"
    ffmpeg -i "$INPUT_FILE" -c:v libx264 -preset medium -crf 23 -c:a aac -b:a 128k "$OUTPUT_FILE" -y 2>&1
    
else
    echo "Conversión genérica: $INPUT_FILE -> $OUTPUT_FILE"
    ffmpeg -i "$INPUT_FILE" "$OUTPUT_FILE" -y 2>&1
fi

if [ $? -eq 0 ]; then
    echo "Conversión exitosa: $OUTPUT_FILE"
    exit 0
else
    echo "Error en la conversión"
    exit 1
fi