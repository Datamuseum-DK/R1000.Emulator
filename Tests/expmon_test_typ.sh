#!/bin/sh

. Tests/subr_test.rc

sc_boards typ

cli 'sc trace DI*PROC 4'
cli 'sc quota add 23'

expmon_prompt

expmon_cmd test_typ

run
