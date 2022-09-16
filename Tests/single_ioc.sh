#!/bin/sh

. Tests/subr_test.rc

cli 'sc force_reset'

single ioc $*
