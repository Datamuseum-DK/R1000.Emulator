SC_BRANCH = main
NETLISTS =/critter/R1K/R1000.HwDoc/${SC_BRANCH}/Schematics/*/*.net

# Set this to where you want the ~1GB trace output file
TRACE_FILE = "/critter/_r1000"

# Set this to copy of https://datamuseum.dk/bits/30000551 (also ~1GB)
DISK0_IMAGE = "/critter/R1K/DiskImages/PE_R1K_Disk0.dd"

# Set this to copy of https://datamuseum.dk/bits/30000552 (also ~1GB)
DISK1_IMAGE = "/critter/R1K/DiskImages/PE_R1K_Disk1.dd"

# DFS tape copy of https://datamuseum.dk/bits/30000528 (20 MB)
DFS_TAPE = "/critter/BitStoreCache/30000750.bin"

# These are alternate images, phk@ has them (~1GB each)
DISK0B_IMAGE = "/critter/DDHF/R1000/R1K_Seagate/R1K_Seagate0.BIN"
DISK1B_IMAGE = "/critter/DDHF/R1000/R1K_Seagate/R1K_Seagate1.BIN"

# Cache directory for firmware files, (relative to this directory)
FIRMWARE_PATH = _Firmware

VPATH	= Musashi:Musashi/softfloat:Infra:Ioc:Diag

OBJS	= main.o callout.o cli.o context.o memory.o trace.o
OBJS	+= elastic.o elastic_fd.o elastic_tcp.o elastic_match.o
OBJS	+= rpn.o
OBJS	+= vav.o
OBJS	+= vsb.o

OBJS	+= m68kcpu.o m68kdasm.o m68kops.o softfloat.o

OBJS	+= _memcfg.o

OBJS	+= ioc_uart.o
OBJS	+= ioc_cli.o
OBJS	+= ioc_debug.o
OBJS	+= ioc_duart.o
OBJS	+= ioc_eeproms.o
OBJS	+= ioc_interrupt.o
OBJS	+= ioc_hotfix.o
OBJS	+= ioc_main.o
OBJS	+= ioc_memtrace.o
OBJS	+= ioc_mosart.o
OBJS	+= ioc_rtc.o
OBJS	+= ioc_scsi_ctl.o
OBJS	+= ioc_scsi_dev.o
OBJS	+= ioc_syscall.o

CFLAGSMINUSI += -I. -IInfra -IMusashi -IIoc 
CFLAGSMINUSD += -DMUSASHI_CNF='"musashi_conf.h"'
CFLAGSMINUSD += -DFIRMWARE_PATH='"${FIRMWARE_PATH}"'

CFLAGS	+= -Wall -Werror -pthread -g -O0
CFLAGS	+= ${CFLAGSMINUSD}
CFLAGS	+= ${CFLAGSMINUSI}
LDFLAGS	+= -lm

PARANOIA != sh cflags.sh "${CC}"

CFLAGS += ${PARANOIA}

BINFILES += ${FIRMWARE_PATH}/IOC_EEPROM.bin
BINFILES += ${FIRMWARE_PATH}/RESHA_EEPROM.bin

SC_CC = ${CXX} ${SC_OPT} ${SC_WARN} -pthread -c
SC_CC += -I/usr/local/include -I.

M68K_INCL = \
	_memcfg.h \
	Musashi/m68kcpu.h \
	Musashi/m68kmmu.h \
	Musashi/softfloat/softfloat.h \
	Ioc/musashi_conf.h \
	Infra/memspace.h \
	m68kops.h

CLI_INCL = \
	_memcfg.h \
	Infra/r1000.h \
	Infra/trace.h \
	Infra/elastic.h
all:
	python3 -u NetList/process_kicad_netlists.py ${SC_BRANCH} ${NETLISTS}
	make r1000sim

netlist:
	python3 -u NetList/process_kicad_netlists.py ${SC_BRANCH} ${NETLISTS}

include Diag/Makefile.inc
include Chassis/Makefile.inc
include Components/Makefile.inc
-include Fiu/${SC_BRANCH}/Makefile.inc
-include Ioc/${SC_BRANCH}/Makefile.inc
-include Mem32/${SC_BRANCH}/Makefile.inc
-include Seq/${SC_BRANCH}/Makefile.inc
-include Typ/${SC_BRANCH}/Makefile.inc
-include Val/${SC_BRANCH}/Makefile.inc

cli:	r1000sim ${BINFILES}
	./r1000sim \
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

EXP_PATH=/critter/R1K/Old/hack/X/

IOC_TEST=TEST_MACRO_EVENT_DELAY.IOC
IOC_TEST=TEST_MACRO_EVENT_SLICE.IOC
IOC_TEST=TEST_COUNTER_DATA.IOC

test_ioc:	all ${BINFILES}
	./r1000sim \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch ioc ' \
		'sc trace "DI*PROC" 6' \
		'sc trace "DFREG" 1' \
		'sc trace "RAND[0-3]" 0' \
		'sc trace "DPROM" 0' \
		'sc trace "DUMSC" 0' \
		'sc trace "TXCVO" 0' \
		'sc trace "RTCTR" 0' \
		'sc q exit' \
		'sc q 1' \
		"diag ioc experiment ${EXP_PATH}/${IOC_TEST}" \
		"diag ioc wait" \
		"diag ioc check" \
		"exit"

FIU_TEST=WCS_ADDRESS_TEST.FIU

FIU_TEST=PHK.FIU
FIU_TEST=TEST_EXTRACT_MERGE.FIU
FIU_TEST=TEST_WCS.FIU
FIU_TEST=TEST_OREG_PARITY.FIU
FIU_TEST=TEST_CSA_OOR.FIU
FIU_TEST=TEST_MAR_DRIVER.FIU
FIU_TEST=TEST_ABUS_PARITY.FIU
FIU_TEST=TEST_INC_MAR.FIU
FIU_TEST=TEST_SIGN_EXTRACT.FIU
FIU_TEST=TEST_CSA_OOR.FIU

test_fiu:	r1000sim ${BINFILES}
	./r1000sim \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch fiu' \
		'sc trace "DI*PROC" 4' \
		'sc trace "DUIRG[1-6]" 0' \
		'sc q exit' \
		'sc q 7' \
		"diag fiu experiment ${EXP_PATH}/${FIU_TEST}" \
		"diag fiu wait" \
		"diag fiu check" \
		"exit"

TYP_TEST=TEST_RESET.TYP
TYP_TEST=PHK.TYP
TYP_TEST=TEST_UIR.TYP
TYP_TEST=TEST_WCS_DATA.TYP
TYP_TEST=TEST_SCAN_CSA_REG.TYP
TYP_TEST=TEST_WCS_PARITY.TYP
TYP_TEST=TEST_SCAN_PAREG.TYP
TYP_TEST=TEST_RF_DATA_LINES.TYP
# Good:
TYP_TEST=TEST_LOAD_WDR.TYP
# Bad:
TYP_TEST=TEST_FIU_BUS.TYP
TYP_TEST=TEST_LOAD_LOOP_COUNTER.TYP
TYP_TEST=TEST_COUNT_LOOP_COUNTER.TYP
TYP_TEST=TEST_RF_DATA_LINES.TYP
TYP_TEST=TEST_READ_GP_ADR.TYP
TYP_TEST=TEST_RD_CSA_BOT_ADR.TYP
TYP_TEST=TEST_NOT_B_OP.TYP
TYP_TEST=TEST_LOAD_TOP.TYP
		
test_typ:	r1000sim ${BINFILES}
	./r1000sim \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch typ' \
		'sc trace "DI*PROC" 6' \
		'sc trace "DUIRG[1-6]" 1' \
		'sc trace "DIDEC0" 1' \
		'sc q exit' \
		'sc q .01' \
		"diag typ experiment ${EXP_PATH}/${TYP_TEST}" \
		"diag typ wait" \
		"diag typ check" \
		"exit"
		

VAL_TEST=TEST_FIU_BUS.VAL
VAL_TEST=POUND_WCS_DESCENDING.VAL
VAL_TEST=POUND_WCS_ASCENDING.VAL
VAL_TEST=INIT_RF.VAL
VAL_TEST=TEST_LOAD_WDR.VAL

test_val:	r1000sim ${BINFILES}
	./r1000sim \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch val' \
		'sc trace "DI*PROC" 6' \
		'sc trace "DUIRG[1-6]" 0' \
		'sc q exit' \
		'sc q 5' \
		"diag val experiment ${EXP_PATH}/${VAL_TEST}" \
		"diag val wait" \
		"diag val check" \
		"exit"
		

SEQ_TEST=SIMPLE_DFSM_FRU.SEQ
SEQ_TEST=DEC_SCAN_CHAIN_FRU.SEQ
SEQ_TEST=TEST_UIR.SEQ
SEQ_TEST=IBUFF_FRU.SEQ
		
test_seq:	r1000sim ${BINFILES}
	./r1000sim \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch seq' \
		'sc trace "DI*PROC" 6' \
		'sc trace "DUIRG[1-6]" 0' \
		'sc trace "WCS" 1' \
		'sc q exit' \
		'sc q 7' \
		"diag seq experiment ${EXP_PATH}/${SEQ_TEST}" \
		"diag seq wait" \
		"diag seq check" \
		"exit"
		

MEM_TEST=TEST_LAR.M32
MEM_TEST=TEST_EXT_FLAG.M32
MEM_TEST=TEST_PARALLEL_SERIAL.M32

test_mem:	r1000sim ${BINFILES}
	./r1000sim \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"diag > _.diag" \
		'trace +systemc' \
		'sc launch mem0' \
		'sc trace "DI*PROC" 6' \
		'sc trace 'PAL' 0' \
		'sc trace 'DFSM' 1' \
		'sc q exit' \
		'sc q .1' \
		"diag mem0 experiment ${EXP_PATH}/${MEM_TEST}" \
		"diag mem0 wait" \
		"diag mem0 check" \
		"exit"
		


hack:	r1000sim ${BINFILES}
	./r1000sim \
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


novram:	r1000sim ${BINFILES}
	./r1000sim \
		-T ${TRACE_FILE} \
		"trace +diagbus_bytes" \
		"trace -ioc_interrupt" \
		"trace -ioc_dma" \
		"trace -ioc_io" \
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
		'sc launch ioc fiu val typ seq mem0 mem2' \
		'sc trace "NOVRAM" on' \
		'sc trace "DI*PROC" 6' \
		'sc trace "IDNTDRV" 1' \
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

expmon:	all ${BINFILES}
	./r1000sim \
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

rdiag:	r1000sim ${BINFILES}
	./r1000sim \
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
		

seagate:	r1000sim ${BINFILES}
	./r1000sim \
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

tape:	r1000sim ${BINFILES}
	truncate -s 1143936000 /critter/_r1000.d0
	truncate -s 1143936000 /critter/_r1000.d1
	./r1000sim \
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
		

r1000sim.${SC_BRANCH}:	${OBJS}
	${CXX} -o r1000sim.${SC_BRANCH} ${CFLAGS} ${LDFLAGS} ${OBJS} \
		-L /usr/local/lib -lsystemc
	rm -f *.tmp

r1000sim: r1000sim.${SC_BRANCH}
	cp r1000sim.${SC_BRANCH} r1000sim

clean:
	rm -f ${OBJS} *.tmp r1000sim m68kops.h m68kops.c m68kmake _memcfg.[ch]

callout.o:		Infra/r1000.h Infra/callout.c
cli.o:			Infra/r1000.h Infra/vav.h Infra/cli.c Ioc/ioc.h
context.o:		Infra/r1000.h Infra/context.c Infra/context.h
elastic.o:		Infra/r1000.h Infra/elastic.h Infra/elastic.c
elastic_fd.o:		Infra/r1000.h Infra/elastic.h Infra/elastic_fd.c
elastic_match.o:	Infra/r1000.h Infra/elastic.h Infra/elastic_match.c
elastic_tcp.o:		Infra/r1000.h Infra/elastic.h Infra/elastic_tcp.c
memory.o:		Infra/r1000.h Infra/memspace.h Infra/memory.c
rpn.o:			Infra/r1000.h Infra/rpn.c
trace.o:		Infra/r1000.h Infra/trace.h Infra/trace.c
vav.o:			Infra/r1000.h Infra/vav.c
vsb.o:			Infra/r1000.h Infra/vsb.c

m68kcpu.o:		${M68K_INCL} Musashi/m68kcpu.c
m68kdasm.o:		${M68K_INCL} Musashi/m68kdasm.c
softfloat.o:		${M68K_INCL} Musashi/softfloat/softfloat.c \
			Musashi/softfloat/softfloat-specialize

_memcfg.o:		_memcfg.c _memcfg.h Infra/memspace.h

ioc_cli.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_cli.c
ioc_debug.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_debug.c
ioc_duart.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_duart.c
ioc_eeproms.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_eeproms.c
ioc_hotfix.o:		${M68K_INCL} Ioc/ioc.h Ioc/ioc_hotfix.c
ioc_interrupt.o:	${CLI_INCL} Ioc/ioc.h Ioc/ioc_interrupt.c
ioc_main.o:		${M68K_INCL} Ioc/ioc.h Ioc/ioc_main.c
ioc_memtrace.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_memtrace.c
ioc_rtc.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_rtc.c
ioc_scsi_ctl.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_scsi.h Ioc/ioc_scsi_ctl.c
ioc_scsi_dev.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_scsi.h Ioc/ioc_scsi_dev.c
ioc_syscall.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_syscall.c
ioc_uart.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_uart.c

m68kops.o:		Musashi/m68kcpu.h m68kops.h m68kops.c
m68kops.h m68kops.c:	m68kmake Ioc/musashi_conf.h Musashi/m68k_in.c
			./m68kmake `pwd` Musashi/m68k_in.c

Infra/memspace.h:	_memcfg.h

_memcfg.h:		makemem.py makemem_class.py
			python3 makemem.py

m68kmake:		Musashi/m68kmake.c

Musashi/m68kcpu.h:	Musashi/m68k.h

flint:
	flexelint flint.lnt \
		${CFLAGSMINUSD} \
		${CFLAGSMINUSI} \
		Diag/*.c \
		Infra/*.c \
		Ioc/*.c \
		SystemC/sc.c \
		diagbus.c
		_memcfg.c

setup:
	git clone https://github.com/Datamuseum-DK/Musashi
	python3 -u fetch_firmware.py all
