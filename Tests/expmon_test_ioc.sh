#!/bin/sh

. Tests/subr_test.rc

if [ "x$1" = "x-quick" ] ; then
	cli 'dfs neuter TEST_WCS_BITS.IOC'
	cli 'dfs neuter TEST_WCS_ADDRESSING.IOC'
fi

sc_boards ioc

cli 'sc trace DI*PROC 4'
cli 'sc quota add 13'
cli 'sc quota exit'

expmon_prompt

expmon_cmd test_ioc

run
