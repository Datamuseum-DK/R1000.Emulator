#!/bin/sh

. Tests/subr_test.rc

if [ "x$1" = "x-quick" ] ; then
	cli 'dfs neuter TEST_WCS_LOCATIONS.VAL'
fi

sc_boards ioc fiu mem0 seq typ val

cli 'sc trace DI*PROC 4'
cli 'sc trace LCNTR 1'
cli 'sc quota add 5'
cli 'sc quota exit'

# 8 => Initialize processor state
# 3 => Execute diagnostics
# 4 => Run a specific test
# 27 => P2VAL
fru_prompt 8 3 4 27

run
