#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL version 2> version.txt 

cmd_tests_cleanup
