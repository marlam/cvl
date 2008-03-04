#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -w20 -h10 -cred   > w20-h10-red.pnm	
$CVTOOL create -w10 -h20 -cgreen > w10-h20-green.pnm	
$CVTOOL create -w15 -h15 -cblue  > w15-h15-blue.pnm	

# 1. leftright, top
$CVTOOL create -w45 -h20 -cwhite \
| $CVTOOL blend -s w20-h10-red.pnm \
| $CVTOOL blend -s w10-h20-green.pnm -x20 \
| $CVTOOL blend -s w15-h15-blue.pnm -x30 \
> combine-lrt-gt.pnm	
$CVTOOL combine --method leftright --justify top --color white \
w20-h10-red.pnm w10-h20-green.pnm w15-h15-blue.pnm \
> combine-lrt.pnm	
cmp combine-lrt-gt.pnm combine-lrt.pnm 

# 2. leftright, center
$CVTOOL create -w45 -h20 -cwhite \
| $CVTOOL blend -s w20-h10-red.pnm -y5 \
| $CVTOOL blend -s w10-h20-green.pnm -x20 \
| $CVTOOL blend -s w15-h15-blue.pnm -x30 -y2 \
> combine-lrc-gt.pnm	
$CVTOOL combine --method leftright --justify center --color white \
w20-h10-red.pnm w10-h20-green.pnm w15-h15-blue.pnm \
> combine-lrc.pnm	
cmp combine-lrc-gt.pnm combine-lrc.pnm 

# 3. leftright, bottom
$CVTOOL create -w45 -h20 -cwhite \
| $CVTOOL blend -s w20-h10-red.pnm -y10 \
| $CVTOOL blend -s w10-h20-green.pnm -x20 \
| $CVTOOL blend -s w15-h15-blue.pnm -x30 -y5 \
> combine-lrb-gt.pnm	
$CVTOOL combine --method leftright --justify bottom --color white \
w20-h10-red.pnm w10-h20-green.pnm w15-h15-blue.pnm \
> combine-lrb.pnm	
cmp combine-lrb-gt.pnm combine-lrb.pnm 

# 4. topbottom, left
$CVTOOL create -w20 -h45 -cwhite \
| $CVTOOL blend -s w20-h10-red.pnm \
| $CVTOOL blend -s w10-h20-green.pnm -y10 \
| $CVTOOL blend -s w15-h15-blue.pnm -y30 \
> combine-tbl-gt.pnm	
$CVTOOL combine --method topbottom --justify left --color white \
w20-h10-red.pnm w10-h20-green.pnm w15-h15-blue.pnm \
> combine-tbl.pnm	
cmp combine-tbl-gt.pnm combine-tbl.pnm 

# 5. topbottom, center
$CVTOOL create -w20 -h45 -cwhite \
| $CVTOOL blend -s w20-h10-red.pnm \
| $CVTOOL blend -s w10-h20-green.pnm -y10 -x5 \
| $CVTOOL blend -s w15-h15-blue.pnm -y30 -x2 \
> combine-tbc-gt.pnm	
$CVTOOL combine --method topbottom --justify center --color white \
w20-h10-red.pnm w10-h20-green.pnm w15-h15-blue.pnm \
> combine-tbc.pnm	
cmp combine-tbc-gt.pnm combine-tbc.pnm 

# 6. topbottom, right
$CVTOOL create -w20 -h45 -cwhite \
| $CVTOOL blend -s w20-h10-red.pnm \
| $CVTOOL blend -s w10-h20-green.pnm -y10 -x10 \
| $CVTOOL blend -s w15-h15-blue.pnm -y30 -x5 \
> combine-tbr-gt.pnm	
$CVTOOL combine --method topbottom --justify right --color white \
w20-h10-red.pnm w10-h20-green.pnm w15-h15-blue.pnm \
> combine-tbr.pnm	
cmp combine-tbr-gt.pnm combine-tbr.pnm 

cmd_tests_cleanup
