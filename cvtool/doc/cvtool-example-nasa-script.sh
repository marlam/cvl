#!/usr/bin/env bash

# Create the three files nasad000.pgm, nasad225.pgm, and nasad450.pgm
# in the directory in which you start this script.

# Downlad the video
wget -O nasa.mov http://spaceflight.nasa.gov/gallery/video/shuttle/sts-114/qtime/114_fdh05_clip3.mov
# Convert to YUV4MPEG and extract the example scene
mkfifo stream.yuv
mplayer -nosound -noframedrop -vo yuv4mpeg nasa.mov &
cvtool select 800-1250 < stream.yuv > nasa.y4m
rm stream.yuv

# Determine the optical flow in both directions
cvtool -v opticalflow bm-sad    -k 8 -M 5 -D 0.01 -L 0.5 < nasa.y4m > flow-fw.dat
cvtool -v opticalflow bm-sad -b -k 8 -M 5 -D 0.01 -L 0.5 < nasa.y4m > flow-bw.dat
# Improve the optical flow with a consistency check
cvtool -v opticalflow cc -t 1 -f flow-bw.dat < flow-fw.dat > flow-fw-improved.dat
cvtool -v opticalflow cc -t 1 -f flow-fw.dat < flow-bw.dat > flow-bw-improved.dat

# Depth tracking
cvtool -v trackdepth -n 451 -d 0,225,450 \
	-f flow-fw-improved.dat -F flow-bw-improved.dat \
	nasad000.pgm nasad225.pgm nasad450.pgm > nasad.pgm

# Smooth the depth maps for better image quality
cvtool -v filter gauss -k 3 < nasad.pgm > nasad-smoothed.pgm

# Depth image based rendering (DIBR)
cvtool -v dibr -d nasad-smoothed.pgm -b 8 < nasa.y4m > nasa-stereo.y4m

# Create a video that can be viewed with red-cyan glasses.
cvtool -v color -g 1.4 < nasa-stereo.y4m \
| cvtool -v stereoview anaglyph -c --glasses red-cyan \
| cvtool -v scale --factor 1.5 \
| mpeg2enc -F 5 -f4 -o nasa-stereo-redcyan.mpg
