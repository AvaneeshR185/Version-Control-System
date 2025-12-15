#!/bin/bash

# name of source file and output program
SRC="main.cpp"
OUT="main"

# compile
g++ -std=c++17 -O2 "$SRC" -o "$OUT"

# if compilation succeeded, run the program
if [ $? -eq 0 ]; then
    echo "Compilation successful! Running program..."
    echo "----------------------------------------"
    ./"$OUT"
else
    echo "Compilation failed!"
fi
