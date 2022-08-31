#!/bin/sh

. Tests/subr_test.rc

sc_boards ioc

cli 'sc trace DI*PROC 4'
cli 'sc trace UAPC 1'
cli 'sc quota add 13'

expmon_prompt

expmon_cmd test_ioc

run
