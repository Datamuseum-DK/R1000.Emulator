#!/bin/sh

set -e 

. Tests/subr_test.rc

#cli 'iop memtrace add vme_window'
#cli 'iop memtrace add vme_short'
#cli 'iop memtrace add vme_standard'
#cli 'iop memtrace add resha_status2'
# cli 'iop memtrace add resha_wildcard'


cli iop memtrace add 0xd10 0xd20
cli trace +ioc_instructions
cli trace +ioc_interrupt
cli trace +ioc_modem
cli console serial /dev/nmdm0B
cli_prompt

cli 'console << "x fixmodem"'
cli 'console match expect CLI>'
cli 'quit'

./r1000sim \
		-T ${R1K_WORKDIR}/_r1000 \
		"include ${R1K_CLIFILE}" 2>&1 | tee ${R1K_PFX}.log
