#!/bin/sh

. Tests/subr_test.rc

sc_boards fiu

cli 'sc trace HRAM 1'

single fiu $*
