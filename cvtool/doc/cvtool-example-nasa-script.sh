#!/usr/bin/env bash

# Downlad the video
wget -O nasa.mov http://spaceflight.nasa.gov/gallery/video/shuttle/sts-114/qtime/114_fdh05_clip3.mov
# Convert to YUV4MPEG and extract the example scene
mkfifo stream.yuv
mplayer -nosound -noframedrop -vo yuv4mpeg nasa.mov &
cvtool select 800-1250 < stream.yuv > nasa.y4m
rm stream.yuv

# Determine the optical flow in both directions
cvtool opticalflow bm-sad    -k 8 -M 5 -D 0.01 -L 0.5 < nasa.y4m > flow-fw.txt
cvtool opticalflow bm-sad -b -k 8 -M 5 -D 0.01 -L 0.5 < nasa.y4m > flow-bw.txt
# Improve the optical flow with a consistency check
cvtool opticalflow cc -t 1 -f flow-bw.txt < flow-fw.txt > flow-fw-improved.txt
cvtool opticalflow cc -t 1 -f flow-fw.txt < flow-bw.txt > flow-bw-improved.txt

# Depth tracking
cvtool trackdepth -n 451 -d 0,225,450 \
	-f flow-fw-improved.txt -F flow-bw-improved.txt \
	nasad000.pgm nasad225.pgm nasad450.pgm > nasad.pgm

# Smooth the depth maps for better image quality
cvtool filter gauss -k 3 < nasad.pgm > nasad-smoothed.pgm

# Depth image based rendering (DIBR)
cvtool dibr -d nasad-smoothed.pgm -b 8 < nasa.y4m > nasa-stereo.y4m

# Create a video that can be viewed with red-cyan glasses.
cvtool color -g 1.4 < nasa-stereo.y4m \
| cvtool stereoview anaglyph -c --glasses red-cyan \
| cvtool scale --factor 1.5 \
| mpeg2enc -f4 -o nasa-stereo-redcyan.mpg
