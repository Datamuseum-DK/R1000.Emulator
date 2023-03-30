#!/bin/sh

. Tests/subr_test.rc

sc_boards seq fiu

cli 'sc trace DI*PROC 4'
cli 'sc quota add 20'
# cli 'dfs sed TEST_SEQ.EM /seq_wcs_tests/d'

expmon_prompt

expmon_cmd test_seq

run
