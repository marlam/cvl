#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t gray -n 1 -w 9 -h 9 -c r000g000b000 > 000.pgm
$CVTOOL create -t gray -n 1 -w 9 -h 9 -c b127g127r127 > 127.pgm
$CVTOOL create -t gray -n 1 -w 9 -h 9 -c g255r255b255 > 255.pgm

$CVTOOL binarize global -t 0 < 000.pgm > x255.pgm
cmp 255.pgm x255.pgm
$CVTOOL binarize global -t 130 < 127.pgm > x000.pgm
cmp 000.pgm x000.pgm
$CVTOOL binarize global -t 130 < 255.pgm > y255.pgm
cmp 255.pgm y255.pgm

$CVTOOL binarize iterative < 127.pgm > /dev/null
$CVTOOL binarize otsu      < 127.pgm > /dev/null
$CVTOOL binarize hysterese -l 10 -h 30 < 127.pgm > /dev/null
$CVTOOL binarize local -T mean   -k 1 -C 0 < 127.pgm > /dev/null
$CVTOOL binarize local -T median -k 1 -C 0 < 127.pgm > /dev/null
$CVTOOL binarize local -T minmax -k 1 -C 0 < 127.pgm > /dev/null

cmd_tests_cleanup
