#!/usr/bin/env bash

. cmd_tests_common.sh

for type in uint8 float; do

	$CVTOOL create -t $type -w 10 -h 10 -c red   > r.$type
	$CVTOOL create -t $type -w 10 -h 10 -c green > g.$type
	$CVTOOL create -t $type -w 10 -h 10 -c blue  > b.$type
	$CVTOOL combine r.$type g.$type b.$type > rgb.$type
	$CVTOOL create -t $type -w 20 -h 20 -c red   > R.$type
	$CVTOOL create -t $type -w 20 -h 20 -c green > G.$type
	$CVTOOL create -t $type -w 20 -h 20 -c blue  > B.$type
	$CVTOOL combine R.$type G.$type B.$type > RGB.$type

	$CVTOOL affine -m 1,0,0,1 -i none < rgb.$type > xrgb.$type
	cmp rgb.$type xrgb.$type

	$CVTOOL affine -m 2,0,0,2 -i none < rgb.$type > xRGB.$type
	cmp RGB.$type xRGB.$type

done

cmd_tests_cleanup
