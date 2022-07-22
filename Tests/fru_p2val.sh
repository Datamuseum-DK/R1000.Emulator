#!/bin/sh

. Tests/subr_test.rc

sc_boards ioc fiu mem0 seq typ val

cli 'sc trace DI*PROC 4'
cli 'sc trace LCNTR 1'
cli 'sc quota add 5'
cli 'sc quota exit'

# Temporary patch to prevent timout of counter overflow test
cli 'dfs patch P2VAL.M200 0x7b8 0xff 0xfe 0xec 0x78'

# 8 => Initialize processor state
# 3 => Execute diagnostics
# 4 => Run a specific test
# 27 => P2VAL
fru_prompt 8 3 4 27

run
