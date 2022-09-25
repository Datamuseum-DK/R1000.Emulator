#!/bin/sh

. Tests/subr_test.rc

sc_boards mem0

single mem0 $*
