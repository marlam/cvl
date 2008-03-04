#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -h 100 -w 100 -c r0g0b0 > black.pnm 
$CVTOOL create -h 100 -w 100 -c r255 > red.pnm 

STYLE1="--width=2 --dash=2 --line-cap=butt --line-join=miter "
STYLE1="$STYLE1 --border-style=none --border-color=r0g0b0 "
STYLE1="$STYLE1 --fill-style=pattern --fill-pattern=red.pnm "
STYLE1="$STYLE1 --antialias --justify-x=center"

STYLE2="--width=0.2 --dash=2,10,5.7 --line-cap=round --line-join=round "
STYLE2="$STYLE2 --border-style=color --border-color=green "
STYLE2="$STYLE2 --fill-style=multipattern --fill-pattern=red.pnm "
STYLE2="$STYLE2 --antialias=off --justify-y=top --justify-x=right"

STYLE3="--width=0.1 --line-cap=square --line-join=bevel "
STYLE3="$STYLE3 --border-style=linear-gradient --border-gradient=10,10,green,90,90,black,0.5,yellow,0.7,blue "
STYLE3="$STYLE3 --fill-style=radial-gradient --fill-gradient=20,20,20,green,80,80,40,black,0.5,yellow,0.7,blue "
STYLE3="$STYLE3 -a0 "
STYLE3="$STYLE3 --justify-y=center --font-slant=italic -F 0.1 -F 0.1,0.2 --font-weight=bold"

$CVTOOL draw $STYLE1 -- rectangle 10,10,80,80 circle 15,15,2 75,75,2 ellipse 20,20,60,60 arc 30,30,30,90,270 \
	move_to 50,50 line_to 40,50 40,40 curve_to 50,30,60,40,60,50 close \
	rel_move_to -15,+30 rel_line_to +5,-5 rel_curve_to -10,-10,0,20,30,30 close \
	text "Hello world" \
	< black.pnm > x1.pnm
$CVTOOL draw $STYLE2 -- rectangle 10,10,80,80 circle 15,15,2 75,75,2 ellipse 20,20,60,60 arc 30,30,30,90,270 \
	move_to 50,50 line_to 40,50 40,40 curve_to 50,30,60,40,60,50 close \
	rel_move_to -15,+30 rel_line_to +5,-5 rel_curve_to -10,-10,0,20,30,30 close \
	text "Hello world" \
	< black.pnm > x2.pnm
$CVTOOL draw $STYLE3 -u -- rectangle 0.10,0.10,0.80,0.80 circle 0.15,0.15,0.2 0.75,0.75,0.2 \
	ellipse 0.20,0.20,0.60,0.60 arc 0.30,0.30,0.30,90,270 \
	move_to 0.50,0.50 line_to 0.40,0.50 0.40,0.40 curve_to 0.50,0.30,0.60,0.40,0.60,0.50 close \
	rel_move_to -0.15,+0.30 rel_line_to +0.5,-0.5 rel_curve_to -0.10,-0.10,0.0,0.20,0.30,0.30 close \
	text "Hello world" \
	< black.pnm > x3.pnm

cmd_tests_cleanup
