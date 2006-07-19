#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -n 3 -t gray -w10 -h10 > gray.pgm 
$CVTOOL create -n 3 -t rgb  -w10 -h10 > rgb.pnm  
$CVTOOL create -n 3 -t yuv  -w10 -h10 > yuv.y4m  

# 1. pgm -> pnm
$CVTOOL convert -o pnm --gray=false < gray.pgm > 1.pnm 
cmp 1.pnm rgb.pnm 

# 2. pgm -> yuv
$CVTOOL convert -o y4m < gray.pgm > 2.pnm 
cmp 2.pnm yuv.y4m 

# 3. pnm -> pgm
$CVTOOL convert -o pnm --gray < rgb.pnm > 3.pgm 
cmp 3.pgm gray.pgm 

# 4. pnm -> yuv
$CVTOOL convert -o y4m < rgb.pnm > 4.y4m 
cmp 4.y4m yuv.y4m 

# 5. yuv -> pgm
$CVTOOL convert -o pnm --gray < yuv.y4m > 5.pgm 
cmp 5.pgm gray.pgm 

# 6. yuv -> pnm
$CVTOOL convert -o pnm < yuv.y4m > 6.pnm 
cmp 6.pnm rgb.pnm 

# 7. yuv420jpeg -> yuv444 -> yuv420jpeg
$CVTOOL convert -o y4m -C 444 < yuv.y4m | $CVTOOL convert -o y4m -C 420jpeg > 7.y4m 
cmp 7.y4m yuv.y4m 

cmd_tests_cleanup
