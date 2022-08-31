#!/bin/sh

. Tests/subr_test.rc

sc_boards ioc fiu mem0 seq typ val

cli 'sc trace DI*PROC 4'
cli 'sc trace ioc.*DPINV0E 1'
cli 'sc trace ioc.*DPNAN1A 1'
cli 'sc trace ioc.*MPREG 1'
cli 'sc trace ioc.*MPFF[01][AB] 1'
cli 'sc trace ioc_32 1'
cli 'sc trace ioc_36 1'
cli 'sc quota add 2000'
cli 'sc quota exit'

# 3 => Execute diagnostics
# 4 => Run a specific test
# 29 => P3UCODE
# 1 => All
fru_prompt 3 4 29 16

run
