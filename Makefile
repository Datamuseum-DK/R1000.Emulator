
# You can override the defaults by creating a Makefile.local
-include Makefile.local

SC_BRANCH ?= main

NETLISTS ?= *.net

OBJDIR ?= _obj

EXP_PATH ?= /critter/R1K/Old/hack/X/

# Set this to where you want the ~1GB trace output file
TRACE_FILE ?= "/tmp/_r1000"

# Set this to copy of https://datamuseum.dk/bits/30000551 (also ~1GB)
DISK0_IMAGE ?= R1K_PAM_Disk_0_Image.BIN

# Set this to copy of https://datamuseum.dk/bits/30000552 (also ~1GB)
DISK1_IMAGE ?= R1K_PAM_Disk_1_Image.BIN

CFLAGSMINUSI += -I.
CFLAGSMINUSD += -DMUSASHI_CNF='"Iop/musashi_conf.h"'
CFLAGSMINUSD += -DFIRMWARE_PATH='"${FIRMWARE_PATH}"'

CFLAGS	+= -Wall -Werror -pthread -g -O0
CFLAGS	+= ${CFLAGSMINUSD}
CFLAGS	+= ${CFLAGSMINUSI}
LDFLAGS	+= -lm

PARANOIA != sh cflags.sh "${CC}"

# SANITIZE = -fsanitize=address

CFLAGS += ${PARANOIA} ${SANITIZE}

SC_OPT = -O2
SC_WARN = -Wall -Werror ${PARANOIA} -Wno-cast-qual -Wno-cast-align -Wno-unused-parameter
SC_WARN += ${SANITIZE}

SC_CC = ${CXX} ${SC_OPT} ${SC_WARN} -pthread -c
SC_CC += -I/usr/local/include -I.

R1000DEP = Infra/r1000.h Infra/vqueue.h Infra/trace.h

all:	netlist
	@echo "# of automatic symbols: `grep OBJS -c Chassis/${SC_BRANCH}/Makefile.inc`"
	${MAKE} r1000sim

branchname:
	@echo ${SC_BRANCH}

netlist:
	-mkdir -p ${OBJDIR}
	python3 -u NetList/process_kicad_netlists.py ${SC_BRANCH} ${NETLISTS}

include Infra/Makefile.inc
include Dfs/Makefile.inc
include Diag/Makefile.inc
include Makefile.musashi.inc
include Iop/Makefile.inc
include Chassis/Makefile.inc
include Components/Makefile.inc
-include Emu/${SC_BRANCH}/Makefile.inc
-include Fiu/${SC_BRANCH}/Makefile.inc
-include Ioc/${SC_BRANCH}/Makefile.inc
-include Mem32/${SC_BRANCH}/Makefile.inc
-include Seq/${SC_BRANCH}/Makefile.inc
-include Typ/${SC_BRANCH}/Makefile.inc
-include Val/${SC_BRANCH}/Makefile.inc
-include Chassis/${SC_BRANCH}/Makefile.inc

R1000SIM=${OBJDIR}/r1000sim.${SC_BRANCH}

${R1000SIM}:	netlist ${OBJS}
	@${CXX} -o ${R1000SIM} ${CFLAGS} ${LDFLAGS} ${OBJS} \
		-L /usr/local/lib -lsystemc

size: r1000sim
	@size ${OBJS} | sort -n
	@size r1000sim

.PHONY: r1000sim

r1000sim: ${R1000SIM}
	-rm -f r1000sim
	ln -s ${R1000SIM} r1000sim

clean:
	rm -f ${OBJS} ${CLEANFILES} *.tmp r1000sim*

flint:
	flexelint flint.lnt \
		${CFLAGSMINUSD} \
		${CFLAGSMINUSI} \
		Dfs/*.c \
		Diag/*.c \
		Infra/*.c \
		Iop/*.c \
		Chassis/*.c \

setup:
	git clone https://github.com/Datamuseum-DK/Musashi
	python3 -u fetch_firmware.py all

cli:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace -diagbus_bytes" \
		"trace -ioc_interrupt" \
		"trace -ioc_dma" \
		"trace -ioc_io" \
		"trace -ioc_pit" \
		"trace +scsi_cmd" \
		"trace -ioc_instructions" \
		"iop syscall internal" \
		"console > _.console" \
		"console telnet localhost:1400" \
		"console serial /dev/nmdm0B" \
		"modem > _.modem" \
		"modem serial /dev/nmdm1B" \
		"diagbus > _.diag" \
		"disk mount 0 ${DISK0_IMAGE}" \
		"disk mount 1 ${DISK1_IMAGE}" \
		"iop reset" \
		'console match expect "Boot from (Tn or Dn)  [D0] : "' \
		'console << ""' \
		'console match expect "Kernel program (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "File system    (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "User program   (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "Enter option [enter CLI] : "' \
		'console << "1"' \
		'console match expect "CLI>"'

dfs:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"scsi_disk 0 ${DISK0_IMAGE}" \
		"scsi_disk 1 ${DISK1_IMAGE}" \
		"help dfs" \
		"help dfs cat" \
		"help dfs dir" \
		"help dfs read" \
		"dfs dir KERN* PROG*" \
		"dfs cat PATCH.HLP _patch.hlp" \
		"dfs read KERNEL_0.M200 _kernel_0.m200" \
		"dfs sed PATCH.HLP s/SWITCH/FOOBAR/" \
		"dfs cat PATCH.HLP" \
		"dfs write Dfs/Makefile.inc PATCH.HLP" \
		"dfs cat FOOBAR" \
		exit

IOC_TEST=TEST_MACRO_EVENT_DELAY.IOC
IOC_TEST=TEST_WCS_ADDRESSING.IOC

test_ioc:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		'sc watchdog 10' \
		'sc force_reset' \
		"trace +diagbus_bytes" \
		"diagbus > _.diag" \
		'trace +systemc' \
		'sc launch ioc ' \
		'sc trace "DI*PROC" 4' \
		'sc quota exit' \
		'sc quota add 5' \
		"diproc experiment ioc ${EXP_PATH}/${IOC_TEST}" \
		"diproc wait ioc" \
		"diproc status ioc" \
		"exit"

FIU_TEST=TEST_OREG_PARITY.FIU

test_fiu:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch fiu' \
		'sc trace "DI*PROC" 6' \
		'sc trace "DUIRG[1-6]" 1' \
		'sc q exit' \
		'sc q 5' \
		"diag fiu experiment ${EXP_PATH}/${FIU_TEST}" \
		"diag fiu wait" \
		"diag fiu check" \
		"exit"

TYP_TEST=TEST_WCS_ADDRESS.TYP

test_typ:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch typ' \
		'sc trace "DI*PROC" 6' \
		'sc trace "DUIRG[1-6]" 1' \
		'sc q exit' \
		'sc q 5' \
		"diag typ experiment ${EXP_PATH}/${TYP_TEST}" \
		"diag typ wait" \
		"diag typ check" \
		"exit"


VAL_TEST=TEST_WCS_ADDRESS.VAL
VAL_TEST=TEST_LOOP_CNTR_OVERFLOW.VAL

test_val:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diagbus > _.diag" \
		'trace +systemc' \
		'sc launch val' \
		'sc trace "DI*PROC" 4' \
		'sc trace "DUIRG[0-6]" 1' \
		'sc trace "LCNTR" 1' \
		'sc quota exit' \
		'sc quota add 5' \
		"diproc experiment val ${EXP_PATH}/${VAL_TEST}" \
		"diproc wait val" \
		"diproc status val" \
		"exit"


SEQ_TEST=LATCHED_STACK_BIT_1_FRU.SEQ

test_seq:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch seq' \
		'sc trace "DI*PROC" 6' \
		'sc trace "DUIRG[1-6]" 1' \
		'sc q exit' \
		'sc q 5' \
		"diag seq experiment ${EXP_PATH}/${SEQ_TEST}" \
		"diag seq wait" \
		"diag seq check" \
		"exit"


MEM_TEST=TEST_PARALLEL_SERIAL.M32
MEM_TEST=TEST_TAGSTORE_PARITY_2.M32

test_mem:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch mem0' \
		'sc trace "DI*PROC" 6' \
		'sc trace 'DFSM' 1' \
		'sc q exit' \
		'sc q 5' \
		"diag mem0 experiment ${EXP_PATH}/${MEM_TEST}" \
		"diag mem0 wait" \
		"diag mem0 check" \
		"exit"



hack:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'sc launch ioc' \
		"dummy_diproc fiu seq val typ mem0 mem2" \
		'trace +systemc' \
		'sc trace "DI*PROC" 6' \
		'sc trace "DFREG" 1' \
		'sc q exit' \
		'sc q 1' \
		"diag ioc tx 0x1a5 { 20 0x1b 0 0 0 0 0 0 0 0x1b 0 10 0x86 0x1a 0x51 0 0 0x19 0x23 0x18 0x60 }"  \
		"diag ioc wait" \
		"diag ioc tx 0x1a5 { 2 0x01 0x15 }"  \


novram:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"#trace -ioc_interrupt" \
		"#trace -ioc_dma" \
		"#trace -ioc_io" \
		"#trace +ioc_pit" \
		"#trace -ioc_instructions" \
		"# ioc memtrace add io_duart" \
		"# ioc memtrace list" \
		"# ioc memtrace del 1" \
		"# ioc memtrace list" \
		"iop syscall internal" \
		"console > _.console" \
		"console telnet localhost:1400" \
		"console serial /dev/nmdm0B" \
		"modem > _.modem" \
		"diagbus > _.diag" \
		"disk mount 0 ${DISK0_IMAGE}" \
		"disk mount 1 ${DISK1_IMAGE}" \
		"iop reset" \
		'console match expect "Boot from (Tn or Dn)  [D0] : "' \
		'console << ""' \
		'console match expect "Kernel program (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "File system    (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "User program   (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "Enter option [enter CLI] : "' \
		'console << "1"' \
		'console match expect "CLI>"' \
		'trace +systemc' \
		'sc launch ioc fiu val typ seq mem0' \
		'diproc dummy -TIMEOUT mem1 mem2 mem3' \
		'sc trace "NOVRAM" on' \
		'sc trace "DI*PROC" 6' \
		'sc quota exit' \
		'sc quota add 3' \
		'console << "x novram"' \
		'console match expect "Enter option : "' \
		'console << "1"' \
		'console match expect "Enter option : "' \
		'console << "0"' \
		'console match expect "CLI>"' \
		'sc rate' \
		exit

EXPERIMENT=write [xeq ioc TEST_WCS_ADDRESSING]
EXPERIMENT=write [xeq ioc TEST_COUNTER_DATA]
EXPERIMENT=write [xeq fiu READ_NOVRAM 00]
EXPERIMENT=TEST_FIU
EXPERIMENT=write [xeq ioc TEST_RESET]

expmon:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace -diagbus_bytes" \
		"trace -ioc_interrupt" \
		"trace -ioc_dma" \
		"trace -ioc_pit" \
		"trace -ioc_instructions" \
		"ioc syscall internal" \
		"ioc memtrace add -lo 0x00000 -hi 0x00000" \
		"console > _.console" \
		"console telnet localhost:1400" \
		"console serial /dev/nmdm0B" \
		"modem > _.modem" \
		"diag > _.diag" \
		"scsi_tape" \
		"scsi_disk 0 ${DISK0_IMAGE}" \
		"scsi_disk 1 ${DISK1_IMAGE}" \
		"ioc reset" \
		'console match expect "Boot from (Tn or Dn)  [D0] : "' \
		'console << ""' \
		'console match expect "Kernel program (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "File system    (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "User program   (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "Enter option [enter CLI] : "' \
		'console << "1"' \
		'console match expect "CLI>"' \
		'trace +systemc' \
		'sc launch ioc fiu' \
		"dummy_diproc seq val typ mem0 mem2" \
		'sc trace "DI*PROC" 4' \
		'sc q exit' \
		'sc q 1' \
		'console << "x expmon"' \
		'console match expect "EM>"' \
		'console << "[set FIRST_PASS true]"' \
		'console match expect "EM>"' \
		'console << "${EXPERIMENT}"' \
		'console match expect "EM>"' \
		'sc rate' \
		exit

fru:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace -diagbus_bytes" \
		"trace -ioc_interrupt" \
		"trace -ioc_dma" \
		"trace -ioc_pit" \
		"trace -ioc_instructions" \
		"iop syscall internal" \
		"console > _.console" \
		"console telnet localhost:1400" \
		"console serial /dev/nmdm0B" \
		"modem > _.modem" \
		"diagbus > _.diag" \
		"disk mount 0 ${DISK0_IMAGE}" \
		"disk mount 1 ${DISK1_IMAGE}" \
		'trace +systemc' \
		"diproc dummy -TIMEOUT mem1 mem2 mem3" \
		'sc launch all' \
		'sc trace "DI*PROC" 4' \
		'sc quota add 3000' \
		"iop reset" \
		'console match expect "Boot from (Tn or Dn)  [D0] : "' \
		'console << ""' \
		'console match expect "Kernel program (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "File system    (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "User program   (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "Enter option [enter CLI] : "' \
		'console << "1"' \
		'console match expect "CLI>"' \
		'console << "x fru"'

$DIAG="TEST IOA"

rdiag:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"trace -diagbus_bytes" \
		"trace -ioc_interrupt" \
		"trace -ioc_dma" \
		"trace -ioc_io" \
		"trace -ioc_instructions" \
		"ioc memtrace add -lo 0x00000 -hi 0x00000" \
		"ioc syscall" \
		"console > _.console" \
		"console telnet localhost:1400" \
		"console serial /dev/nmdm0B" \
		"modem > _.modem" \
		"diag > _.diag" \
		"scsi_tape" \
		"scsi_disk 0 ${DISK0_IMAGE}" \
		"scsi_disk 1 ${DISK1_IMAGE}" \
		"ioc reset" \
		'console match expect "Boot from (Tn or Dn)  [D0] : "' \
		'console << ""' \
		'console match expect "Kernel program (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "File system    (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "User program   (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "Enter option [enter CLI] : "' \
		'console << "1"' \
		'console match expect "CLI>"' \
		'sc launch ioc ' \
		"dummy_diproc seq fiu val typ mem0 mem2" \
		'trace +systemc' \
		'sc trace "DI*PROC" 4' \
		'sc q exit' \
		'sc q 300' \
		'console << "x rdiag"' \
		'console match expect "DIAG>"' \
		'console << "${DIAG}"' \
		'console match expect "EM>"' \
		'sc rate' \
		exit


seagate:	${R1000SIM}
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"console > _.console" \
		"console telnet :1400" \
		"diag > _.diag" \
		"scsi_disk 0 ${DISK0B_IMAGE}" \
		"scsi_disk 1 ${DISK1B_IMAGE}" \
		"ioc reset" \
		'console match expect "Boot from (Tn or Dn)  [D0] : "' \
		'console << ""' \
		'console match expect "Kernel program (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "File system    (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "User program   (0,1,2) [0] : "' \
		'console << ""' 

tape:	${R1000SIM}
	truncate -s 1143936000 /critter/_r1000.d0
	truncate -s 1143936000 /critter/_r1000.d1
	${R1000SIM} \
		-T ${TRACE_FILE} \
		"ioc syscall" \
		"scsi_tape ${DFS_TAPE}" \
		"scsi_disk 0 /critter/_r1000.d0" \
		"scsi_disk 1 /critter/_r1000.d1" \
		"console telnet :1400" \
		"console serial /dev/nmdm0B" \
		"console > _.console" \
		"ioc reset" \
		'console match expect "Boot from (Tn or Dn)  [D0] : "' \
		'console << "T0"' \
		'console match expect "Select files to boot [D=DEFAULT, O=OPERATOR_SUPPLIED] : [D]"' \


foo:
		'console << "D"' \
		'console match expect "Enable line printer for console output [N] ? "' \
		'console << "N"' \
		'console match expect "Enter option : "' \
		'console << "5"' \
		'console match expect "Enter unit number of disk to format/build/scan (usually 0) : "' \
		'console << "0"' \
		'console match expect "Tape drive unit number : "' \
		'console << "0"'
