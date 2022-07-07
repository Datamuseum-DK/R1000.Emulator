#!/bin/sh

. Tests/subr_test.rc

sc_boards mem0

cli 'sc trace DI*PROC 4'
cli 'sc quota add 280'

expmon_prompt

expmon_cmd "test_mem32 0 0" 

run
