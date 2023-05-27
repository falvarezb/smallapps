#!/bin/bash

if [[ $# -lt 1 ]]; then
    echo "specify program to build"
    exit 1
else
    PROGRAM=$1
fi


OUTPUT_FOLDER=out
nasm -g -f macho64 "$PROGRAM".asm -o "$OUTPUT_FOLDER"/"$PROGRAM".o 
ld -no_pie -L /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -macosx_version_min 10.16.0 -lSystem -o "$OUTPUT_FOLDER"/"$PROGRAM" "$OUTPUT_FOLDER"/"$PROGRAM".o


# nasm -g -f macho64 "$PROGRAM".asm 
# ld -no_pie -L /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -macosx_version_min 10.16.0 -lSystem -o "$PROGRAM" "$PROGRAM".o


