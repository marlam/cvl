#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL version 2> version.txt 

cmd_tests_cleanup
