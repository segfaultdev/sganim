#!/usr/bin/sh

sh build.sh
mkdir -p output

./render

cd output
ffmpeg -framerate 50 -pattern_type glob -i "*.bmp" -c:v libx264 -pix_fmt yuv420p ../output.mp4
cd ..

rm -rf output
