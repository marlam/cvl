#!/usr/bin/env bash

. cmd_tests_common.sh

for t in uint8 float; do
  for f in lum color; do
    $CVTOOL create -n 1 -t $t -f $f -w10 -h10 > $f.$t
  done
done
for t in uint8 float; do
  for f in lum color; do
    for ct in uint8 float; do
      for cf in lum color; do
        $CVTOOL convert -t $ct -f $cf < $f.$t > c$cf.$ct
	cmp c$cf.$ct $cf.$ct
      done
    done
  done
done

cmd_tests_cleanup
