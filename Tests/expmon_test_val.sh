#!/bin/sh

. Tests/subr_test.rc

sc_boards val

cli 'sc trace DI*PROC 4'
cli 'sc quota add 12'

expmon_prompt

expmon_cmd test_val

run
