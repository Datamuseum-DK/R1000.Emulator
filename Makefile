
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

OBJS	= main.o callout.o cli.o memory.o trace.o
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
OBJS	+= ioc_rtc.o
OBJS	+= ioc_scsi_ctl.o
OBJS	+= ioc_scsi_dev.o
OBJS	+= ioc_syscall.o

OBJS	+= i8052.o

CFLAGSMINUSI += -I. -IInfra -IMusashi -IIoc -IDiag
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

#######################################################################
# To include SystemC simulator, download more firmware with:
#	make setup_systemc
# and uncomment this `include` line:
include SystemC/Makefile.inc
#######################################################################

cli:	r1000sim ${BINFILES}
	(cd SystemC && make)
	./r1000sim \
		-T ${TRACE_FILE} \
		-t 0x0 \
		"trace -diagbus_bytes" \
		"trace -ioc_interrupt" \
		"trace -ioc_dma" \
		"trace -ioc_instructions" \
		"ioc memtrace add -lo 0x00000 -hi 0x00000" \
		"ioc syscall" \
		"console > _.console" \
		"console telnet localhost:1400" \
		"modem > _.modem" \
		"ioc diagbus > _.diag" \
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

test:	r1000sim ${BINFILES}
	(cd SystemC && make)
	./r1000sim \
		-T ${TRACE_FILE} \
		-t 0x0 \
		"trace -diagbus_bytes" \
		"trace -ioc_interrupt" \
		"trace -ioc_dma" \
		"trace -ioc_instructions" \
		"ioc memtrace add -lo 0x00000 -hi 0x00000" \
		"ioc syscall" \
		"console > _.console" \
		"console telnet localhost:1400" \
		"modem > _.modem" \
		"ioc diagbus > _.diag" \
		"scsi_tape" \
		"scsi_disk 0 ${DISK0_IMAGE}" \
		"scsi_disk 1 ${DISK1_IMAGE}" \
		"dummy_diproc seq fiu ioc typ val mem0" \
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
		'console << "x novram"' \
		'console match expect "Enter option : "' \
		'console << "1"' \
		'console match expect "Enter option : "' \
		'console << "0"' \
		'console match expect "CLI>"'
		

seagate:	r1000sim ${BINFILES}
	./r1000sim \
		-T ${TRACE_FILE} \
		-t 254 \
		"console > _.console" \
		"console telnet :1400" \
		"ioc diagbus > _.diag" \
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
	./r1000sim \
		-T ${TRACE_FILE} \
		-t 0x0 \
		"ioc syscall" \
		"scsi_tape ${DFS_TAPE}" \
		"scsi_disk 0 ${DISK0_IMAGE}" \
		"scsi_disk 1 ${DISK1_IMAGE}" \
		"console telnet :1400" \
		"console > _.console" \
		"reset" \
		'console match expect "Boot from (Tn or Dn)  [D0] : "' \
		'console << "T0"' \
		'console match expect "Select files to boot [D=DEFAULT, O=OPERATOR_SUPPLIED] : [D]"' \
		'console << "D"' \
		'console match expect "Enable line printer for console output [N] ? "' \
		'console << "N"' \
		'console match expect "Enter option : "' \
		'console << "5"' \
		'console match expect "Enter unit number of disk to format/build/scan (usually 0) : "' \
		'console << "0"' \
		'console match expect "Tape drive unit number : "' \
		'console << "0"'
		

r1000sim:	${OBJS}
	${CC} -o r1000sim ${CFLAGS} ${LDFLAGS} ${OBJS} ${SIMLDFLAGS}
	rm -f *.tmp

clean:
	rm -f *.o *.tmp r1000sim m68kops.h m68kops.c m68kmake _memcfg.[ch]

callout.o:		Infra/r1000.h Infra/callout.c
cli.o:			Infra/r1000.h Infra/vav.h Infra/cli.c Ioc/ioc.h
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

i8052.o:		${CLI_INCL} Diag/i8052.c

sc.o:			${CLI_INCL} SystemC/sc.c

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
		_memcfg.c

setup:	${BINFILES}
	git clone https://github.com/Datamuseum-DK/Musashi

setup_systemc:
	python3 -u fetch_firmware.py all

${BINFILES}:
	python3 -u fetch_firmware.py
