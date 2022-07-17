#!/bin/sh

set -e 

. Tests/subr_test.rc

sc_boards fiu ioc mem0 seq typ val

cli 'sc trace DI*PROC 4'
cli 'sc quota add 50'
cli 'sc quota exit'

cli_prompt
cli 'console << "x novram"'
cli 'console match expect "Enter option : "'
cli 'console << "1"'
cli 'console match expect "Enter option : "'
cli 'console << "0"'
cli 'console match expect "CLI>"'
cli 'sc rate'
cli 'exit'

run
