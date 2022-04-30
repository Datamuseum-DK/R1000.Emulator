SC_BRANCH ?= megacomp2
SC_BRANCH ?= main
NETLISTS ?= /critter/R1K/R1000.HwDoc/${SC_BRANCH}/Schematics/*/*.net

EXP_PATH ?= /critter/R1K/Old/hack/X/

# Set this to where you want the ~1GB trace output file
TRACE_FILE ?= "/critter/_r1000"

# Set this to copy of https://datamuseum.dk/bits/30000551 (also ~1GB)
DISK0_IMAGE ?= "/critter/R1K/DiskImages/PE_R1K_Disk0.dd"

# Set this to copy of https://datamuseum.dk/bits/30000552 (also ~1GB)
DISK1_IMAGE ?= "/critter/R1K/DiskImages/PE_R1K_Disk1.dd"

# DFS tape copy of https://datamuseum.dk/bits/30000528 (20 MB)
DFS_TAPE ?= "/critter/BitStoreCache/30000750.bin"

# These are alternate images, phk@ has them (~1GB each)
DISK0B_IMAGE = "/critter/DDHF/R1000/R1K_Seagate/R1K_Seagate0.BIN"
DISK1B_IMAGE = "/critter/DDHF/R1000/R1K_Seagate/R1K_Seagate1.BIN"

# Cache directory for firmware files, (relative to this directory)
FIRMWARE_PATH = _Firmware


CFLAGSMINUSI += -I.
CFLAGSMINUSD += -DMUSASHI_CNF='"Ioc/musashi_conf.h"'
CFLAGSMINUSD += -DFIRMWARE_PATH='"${FIRMWARE_PATH}"'

CFLAGS	+= -Wall -Werror -pthread -g -O0
CFLAGS	+= ${CFLAGSMINUSD}
CFLAGS	+= ${CFLAGSMINUSI}
LDFLAGS	+= -lm

PARANOIA != sh cflags.sh "${CC}"

CFLAGS += ${PARANOIA}

SC_OPT = -O2
SC_WARN = -Wall -Werror ${PARANOIA} -Wno-cast-qual -Wno-cast-align -Wno-unused-parameter
SC_CC = ${CXX} ${SC_OPT} ${SC_WARN} -pthread -c
SC_CC += -I/usr/local/include -I.

R1000DEP = Infra/r1000.h Infra/vqueue.h Infra/trace.h

all:	netlist
	${MAKE} r1000sim

branchname:
	@echo ${SC_BRANCH}

netlist:
	python3 -u NetList/process_kicad_netlists.py ${SC_BRANCH} ${NETLISTS}

include Infra/Makefile.inc
include Diag/Makefile.inc
include Makefile.musashi.inc
include Ioc/Makefile.inc
include Chassis/Makefile.inc
include Components/Makefile.inc
-include Fiu/${SC_BRANCH}/Makefile.inc
-include Ioc/${SC_BRANCH}/Makefile.inc
-include Mem32/${SC_BRANCH}/Makefile.inc
-include Seq/${SC_BRANCH}/Makefile.inc
-include Typ/${SC_BRANCH}/Makefile.inc
-include Val/${SC_BRANCH}/Makefile.inc
-include Chassis/${SC_BRANCH}/Makefile.inc

r1000sim.${SC_BRANCH}:	netlist ${OBJS}
	@${CXX} -o r1000sim.${SC_BRANCH} ${CFLAGS} ${LDFLAGS} ${OBJS} \
		-L /usr/local/lib -lsystemc

size: r1000sim
	@size ${OBJS} | sort -n
	@size r1000sim

.PHONY: r1000sim

r1000sim:
	${MAKE} r1000sim.${SC_BRANCH}
	cp r1000sim.${SC_BRANCH} r1000sim

clean:
	rm -f ${OBJS} ${CLEANFILES} *.tmp r1000sim*

flint:
	flexelint flint.lnt \
		${CFLAGSMINUSD} \
		${CFLAGSMINUSI} \
		Diag/*.c \
		Infra/*.c \
		Ioc/*.c \
		SystemC/sc.c \
		diagbus.c

setup:
	git clone https://github.com/Datamuseum-DK/Musashi
	python3 -u fetch_firmware.py all
cli:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
		-T ${TRACE_FILE} \
		"trace -diagbus_bytes" \
		"trace -ioc_interrupt" \
		"trace -ioc_dma" \
		"trace -ioc_io" \
		"trace -ioc_pit" \
		"trace +scsi_cmd" \
		"trace -ioc_instructions" \
		"ioc syscall internal" \
		"console > _.console" \
		"console telnet localhost:1400" \
		"console serial /dev/nmdm0B" \
		"modem > _.modem" \
		"modem serial /dev/nmdm1B" \
		"diag > _.diag" \
		"scsi_tape" \
		"scsi_disk 0 ${DISK0_IMAGE}" \
		"scsi_disk 1 ${DISK1_IMAGE}" \
		"reset" \
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

IOC_TEST=TEST_MACRO_EVENT_DELAY.IOC

test_ioc:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
		-T ${TRACE_FILE} \
		'sc watchdog 10' \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch ioc ' \
		'sc trace "." 0' \
		'sc trace "DI*PROC" 6' \
		'sc trace "DFREG" 1' \
		'sc q exit' \
		'sc q 5' \
		"diag ioc experiment ${EXP_PATH}/${IOC_TEST}" \
		"diag ioc wait" \
		"diag ioc check" \
		"exit"

FIU_TEST=TEST_OREG_PARITY.FIU

test_fiu:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
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

test_typ:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
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

test_val:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch val' \
		'sc trace "DI*PROC" 6' \
		'sc trace "DUIRG[0-6]" 1' \
		'sc q exit' \
		'sc q 5' \
		"diag val experiment ${EXP_PATH}/${VAL_TEST}" \
		"diag val wait" \
		"diag val check" \
		"exit"


SEQ_TEST=LATCHED_STACK_BIT_1_FRU.SEQ

test_seq:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
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

test_mem:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
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



hack:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
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


novram:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"trace -ioc_interrupt" \
		"trace -ioc_dma" \
		"trace -ioc_io" \
		"trace +ioc_pit" \
		"trace -ioc_instructions" \
		"ioc memtrace add -lo 0x00000 -hi 0x00000" \
		"ioc syscall internal" \
		"console > _.console" \
		"console telnet localhost:1400" \
		"console serial /dev/nmdm0B" \
		"modem > _.modem" \
		"diag > _.diag" \
		"scsi_tape" \
		"scsi_disk 0 ${DISK0_IMAGE}" \
		"scsi_disk 1 ${DISK1_IMAGE}" \
		"reset" \
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
		'sc trace "NOVRAM" on' \
		'sc trace "DI*PROC" 6' \
		'sc q exit' \
		'sc q 3' \
		'console << "x novram"' \
		'console match expect "Enter option : "' \
		'console << "1"' \
		'console match expect "Enter option : "' \
		'console << "0"' \
		'console match expect "CLI>"' \
		exit

EXPERIMENT=write [xeq ioc TEST_WCS_ADDRESSING]
EXPERIMENT=write [xeq ioc TEST_COUNTER_DATA]
EXPERIMENT=write [xeq fiu READ_NOVRAM 00]
EXPERIMENT=TEST_FIU
EXPERIMENT=write [xeq ioc TEST_RESET]

expmon:	all
	./r1000sim.${SC_BRANCH} \
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
		"reset" \
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

$DIAG="TEST IOA"

rdiag:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
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
		"reset" \
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


seagate:	r1000sim.${SC_BRANCH}
	./r1000sim.${SC_BRANCH} \
		-T ${TRACE_FILE} \
		"console > _.console" \
		"console telnet :1400" \
		"diag > _.diag" \
		"scsi_disk 0 ${DISK0B_IMAGE}" \
		"scsi_disk 1 ${DISK1B_IMAGE}" \
		"reset" \
		'console match expect "Boot from (Tn or Dn)  [D0] : "' \
		'console << ""' \
		'console match expect "Kernel program (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "File system    (0,1,2) [0] : "' \
		'console << ""' \
		'console match expect "User program   (0,1,2) [0] : "' \
		'console << ""' 

tape:	r1000sim.${SC_BRANCH}
	truncate -s 1143936000 /critter/_r1000.d0
	truncate -s 1143936000 /critter/_r1000.d1
	./r1000sim.${SC_BRANCH} \
		-T ${TRACE_FILE} \
		"ioc syscall" \
		"scsi_tape ${DFS_TAPE}" \
		"scsi_disk 0 /critter/_r1000.d0" \
		"scsi_disk 1 /critter/_r1000.d1" \
		"console telnet :1400" \
		"console serial /dev/nmdm0B" \
		"console > _.console" \
		"reset" \
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
