#!/bin/sh

if [ "x$1" == "x" ] ; then
	echo "Specify run-name as argument" 1>&2
	exit 1
fi

if [ -d $1 ] ; then
	echo "Run-name ($1) directory already exists" 1>&2
	exit 1
fi

rundir=$1

mkdir -p ${rundir}

make -j 7 && make -j 7

. Tests/subr_test.rc

sc_boards ioc fiu mem0 seq typ val

cli 'sc trace DI*PROC 0'
cli 'sc quota add 5000'
cli 'sc quota exit'


# LOADER microcode get started 2s -> 1.5s
cli 'dfs patch LOADER.M200 0x122e 0x00 0x00 0x72 0x70'

# LOADER copyright post-delay 10s -> 0.1s
cli 'dfs patch LOADER.M200 0x0820 0x00 0x00 0x07 0xa1'

# MONITOR poll-rate 2s -> 0.1s
cli 'dfs patch MONITOR.M200 0x0c40 0x00 0x00 0x0f 0x42'

cli 'sc trace ioc_32 1'
cli 'sc trace seq.*EMADV 1'
cli 'sc trace ioc.*FFFF0B 1'
cli 'sc trace ioc.*EVNAN0D 1'

cli 'iop memtrace add fifo_req_latch'
cli 'iop memtrace add fifo_request'
cli 'iop memtrace add fifo_response'
cli 'iop memtrace add fifo_init'

cli 'iop syscall internal'

cli iop reset

cli 'console match expect "Boot from (Tn or Dn)  [D0] : "'
cli 'console << ""'
cli 'console match expect "Kernel program (0,1,2) [0] : "'
cli 'console << ""'
cli 'console match expect "File system    (0,1,2) [0] : "'
cli 'console << ""'
cli 'console match expect "User program   (0,1,2) [0] : "'
cli 'console << ""'
cli 'console match expect "Enter option [enter CLI] : "'
cli 'console << "6"'

./r1000sim \
	-T ${rundir}/_r1000 \
	"include ${R1K_CLIFILE}" 2>&1 | tee ${rundir}/_r1000.log
