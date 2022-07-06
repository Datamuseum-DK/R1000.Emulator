#!/bin/sh

. Tests/subr_test.rc

sc_boards seq fiu

cli 'sc trace DI*PROC 4'
cli 'sc quota add 30'

expmon_prompt

expmon_cmd test_seq

run
