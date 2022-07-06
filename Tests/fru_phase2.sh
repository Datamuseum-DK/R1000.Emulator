#!/bin/sh

. Tests/subr_test.rc

sc_boards ioc fiu mem0 seq typ val

cli 'sc trace DI*PROC 4'
cli 'sc quota add 2000'
cli 'sc quota exit'

# 3 => Execute diagnostics
# 2 => Run all tests
# 2 => Please enter maximum test phase (1-3)
fru_prompt 3 2 2

run
