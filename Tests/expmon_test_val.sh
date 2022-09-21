#!/bin/sh

. Tests/subr_test.rc

if [ "x$1" = "x-quick" ] ; then
	cli 'dfs neuter POUND_WCS_ASCENDING.VAL'
	cli 'dfs neuter POUND_WCS_DESCENDING.VAL'
fi

sc_boards val

cli 'sc trace DI*PROC 4'
cli 'sc quota add 12'

expmon_prompt

expmon_cmd test_val

run
