
# Set this to where you want the ~1GB trace output file
TRACE_FILE = "/critter/_r1000"

# Set this to copy of https://datamuseum.dk/bits/30000551 (also ~1GB)
DISK0_IMAGE = "/critter/DDHF/20191107_R1K_TAPES/R1K/PE_R1K_Disk0.dd"

# Set this to copy of https://datamuseum.dk/bits/30000552 (also ~1GB)
DISK1_IMAGE = "/critter/DDHF/20191107_R1K_TAPES/R1K/PE_R1K_Disk1.dd"

# DFS tape copy of https://datamuseum.dk/bits/30000528 (20 MB)
DFS_TAPE = "/critter/BitStoreCache/30000750.bin"

# These are alternate images, phk@ has them (~1GB each)
DISK0B_IMAGE = "/critter/DDHF/R1000/R1K_Seagate/R1K_Seagate0.BIN"
DISK1B_IMAGE = "/critter/DDHF/R1000/R1K_Seagate/R1K_Seagate1.BIN"

VPATH	= Musashi:Musashi/softfloat:Infra:Ioc

OBJS	= main.o callout.o cli.o memory.o
OBJS	+= elastic.o elastic_fd.o elastic_tcp.o elastic_match.o
OBJS	+= vav.o
OBJS	+= vsb.o

OBJS	+= m68kcpu.o m68kdasm.o m68kops.o softfloat.o

OBJS	+= _memcfg.o

OBJS	+= ioc_uart.o
OBJS	+= ioc_cli.o
OBJS	+= ioc_duart.o
OBJS	+= ioc_eeproms.o
OBJS	+= ioc_interrupt.o
OBJS	+= ioc_hotfix.o
OBJS	+= ioc_main.o
OBJS	+= ioc_rtc.o
OBJS	+= ioc_scsi_ctl.o
OBJS	+= ioc_scsi_dev.o
OBJS	+= ioc_syscall.o

OBJS	+= i8052.o

CFLAGSMINUSI += -I. -IInfra -IMusashi -IIoc -IDiag
CFLAGSMINUSD += -DMUSASHI_CNF='"musashi_conf.h"'

CFLAGS	+= -Wall -Werror -pthread -g -O0
CFLAGS	+= ${CFLAGSMINUSD}
CFLAGS	+= ${CFLAGSMINUSI}
LDFLAGS	+= -lm

PARANOIA += -std=gnu99 -Wno-format-zero-length -nobuiltininc 
PARANOIA += -fstack-protector-strong -Wsystem-headers -Werror -Wall 
PARANOIA += -Wno-format-y2k -W -Wno-unused-parameter -Wstrict-prototypes
PARANOIA += -Wmissing-prototypes -Wpointer-arith -Wreturn-type -Wcast-qual
PARANOIA += -Wwrite-strings -Wswitch -Wshadow -Wunused-parameter -Wcast-align
PARANOIA += -Wchar-subscripts -Winline -Wnested-externs -Wredundant-decls
PARANOIA += -Wold-style-definition -Wno-pointer-sign
PARANOIA += -Wmissing-variable-declarations -Wthread-safety -Wno-empty-body
PARANOIA += -Wno-string-plus-int -Wno-unused-const-variable
PARANOIA += -Qunused-arguments

PARANOIA += -Wno-missing-field-initializers

CFLAGS += ${PARANOIA}

BINFILES += IOC_EEPROM.bin
BINFILES += RESHA_EEPROM.bin

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
	Infra/elastic.h

test:	r1000 ${BINFILES}
	./r1000 \
		-T ${TRACE_FILE} \
		-t 0x6a \
		"console > _.console" \
		"console telnet :1400" \
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
		'console match expect "CLI>"' \
		'console << "x novram"' \
		'console match expect "Enter option : "' \
		'console << "1"' \
		'console match expect "Enter option : "' \
		'console << "0"'
		

seagate:	r1000 ${BINFILES}
	./r1000 \
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

tape:	r1000 ${BINFILES}
	./r1000 \
		-T /critter/_r1000 \
		-t 0x29 \
		"syscall" \
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
		

r1000:	${OBJS}
	${CC} -o r1000 ${CFLAGS} ${LDFLAGS} ${OBJS}
	rm -f *.tmp

clean:
	rm -f *.o *.tmp r1000 m68kops.h m68kops.c m68kmake _memcfg.[ch]

callout.o:		Infra/r1000.h Infra/callout.c
cli.o:			Infra/r1000.h Infra/vav.h Infra/cli.c Ioc/ioc.h
elastic.o:		Infra/r1000.h Infra/elastic.h Infra/elastic.c
elastic_fd.o:		Infra/r1000.h Infra/elastic.h Infra/elastic_fd.c
elastic_match.o:	Infra/r1000.h Infra/elastic.h Infra/elastic_match.c
elastic_tcp.o:		Infra/r1000.h Infra/elastic.h Infra/elastic_tcp.c
main.o:			Infra/r1000.h Infra/main.c
memory.o:		Infra/r1000.h Infra/memspace.h Infra/memory.c
vav.o:			Infra/r1000.h Infra/vav.c
vsb.o:			Infra/r1000.h Infra/vsb.c

m68kcpu.o:		${M68K_INCL} Musashi/m68kcpu.c
m68kdasm.o:		${M68K_INCL} Musashi/m68kdasm.c
softfloat.o:		${M68K_INCL} Musashi/softfloat/softfloat.c \
			Musashi/softfloat/softfloat-specialize

_memcfg.o:		_memcfg.c _memcfg.h Infra/memspace.h

ioc_cli.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_cli.c
ioc_duart.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_duart.c
ioc_eeproms.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_eeproms.c
ioc_hotfix.o:		${M68K_INCL} Ioc/ioc.h Ioc/ioc_hotfix.c
ioc_interrupt.o:	${CLI_INCL} Ioc/ioc.h Ioc/ioc_interrupt.c
ioc_main.o:		${M68K_INCL} Ioc/ioc.h Ioc/ioc_main.c
ioc_rtc.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_rtc.c
ioc_scsi_ctl.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_scsi.h Ioc/ioc_scsi_ctl.c
ioc_scsi_dev.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_scsi.h Ioc/ioc_scsi_dev.c
ioc_syscall.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_syscall.c
ioc_uart.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_uart.c

i8052.o:		${CLI_INCL} Diag/i8052.c

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
		_memcfg.c

setup:	${BINFILES}
	git clone https://github.com/Datamuseum-DK/Musashi

${BINFILES}:
	curl -o IOC_EEPROM.bin https://datamuseum.dk/bits/30000502
	curl -o RESHA_EEPROM.bin https://datamuseum.dk/bits/30000503
