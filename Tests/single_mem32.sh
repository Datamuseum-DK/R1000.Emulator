#!/bin/sh

. Tests/subr_test.rc

sc_boards mem0

cli 'sc trace mem32_32 1'
cli 'sc trace DINAN0A 0'
cli 'sc trace mem32_32.DI*PROC 5'
cli 'sc trace mem32_33 1'
cli 'sc trace DIAOI0B_1 0'
cli 'sc trace DINOR0B 0'
cli 'sc trace DISTPAL 0'
cli 'dfs patch LOAD_CONFIG.M32 0x3c 0x30 0x32'

single mem0 $*
