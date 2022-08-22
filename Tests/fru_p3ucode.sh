#!/bin/sh

. Tests/subr_test.rc

sc_boards ioc fiu mem0 seq typ val

cli 'sc trace DI*PROC 4'
cli 'sc quota add 2000'
cli 'sc quota exit'
cli 'sc trace ioc_32 1'

# 3 => Execute diagnostics
# 4 => Run a specific test
# 29 => P3UCODE
# 1 => All
fru_prompt 3 4 29 1

run
