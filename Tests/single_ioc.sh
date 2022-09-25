#!/bin/sh

. Tests/subr_test.rc

cli 'sc force_reset'

sc_boards ioc

cli 'sc trace MREG0 3'

single ioc $*
