#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

for w in 1 19 32; do
	for h in 1 12 32; do
		$CVTOOL create -n 1 -w $w -h $h -c 0xff0000 > r.pnm.$w.$h
		$CVTOOL create -n 1 -w $w -h $h -c 0x00ff00 > g.pnm.$w.$h 
		$CVTOOL create -n 1 -w $w -h $h -c 0x0000ff > b.pnm.$w.$h 
		$CVTOOL create -n 1 -w $w -h $h -c 0xff00ff > rb.pnm.$w.$h
		$CVTOOL create -n 1 -w $w -h $h -c 0x000000 > 0.pnm.$w.$h
		$CVTOOL merge -o merge1.txt.$w.$h r.pnm.$w.$h g.pnm.$w.$h b.pnm.$w.$h > rgb.pnm.$w.$h
		$CVTOOL merge -o merge2.txt.$w.$h b.pnm.$w.$h g.pnm.$w.$h r.pnm.$w.$h > bgr.pnm.$w.$h 
		$CVTOOL merge -o merge3.txt.$w.$h 0.pnm.$w.$h 0.pnm.$w.$h 0.pnm.$w.$h > 000.pnm.$w.$h 
		$CVTOOL merge -o merge4.txt.$w.$h rb.pnm.$w.$h 0.pnm.$w.$h rb.pnm.$w.$h > rb0rb.pnm.$w.$h

		$CVTOOL diff -s -o d1.txt.$w.$h rgb.pnm.$w.$h rgb.pnm.$w.$h > x000.pnm.$w.$h
		cmp 000.pnm.$w.$h x000.pnm.$w.$h
		$CVTOOL diff -s rgb.pnm.$w.$h bgr.pnm.$w.$h > xrb0rb.pnm.$w.$h 2> d2.txt.$w.$h
		cmp rb0rb.pnm.$w.$h xrb0rb.pnm.$w.$h

		$CVTOOL diff -s -o - rgb.pnm.$w.$h rgb.pnm.$w.$h > d3.txt.$w.$h
		cmp d1.txt.$w.$h d3.txt.$w.$h
	done
done

cmd_tests_cleanup
