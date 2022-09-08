#!/bin/sh

set -e 

. Tests/subr_test.rc

cli console serial /dev/nmdm0B
cli_prompt

./r1000sim \
		-T ${R1K_WORKDIR}/_r1000 \
		"include ${R1K_CLIFILE}" 2>&1 | tee ${R1K_PFX}.log
