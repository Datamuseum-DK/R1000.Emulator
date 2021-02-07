
OBJS	= main.o cli.o
OBJS	+= elastic.o elastic_fd.o elastic_tcp.o elastic_match.o
OBJS	+= vav.o

OBJS	+= m68kcpu.o m68kdasm.o m68kops.o softfloat.o

OBJS	+= ioc_console.o
OBJS	+= ioc_cli.o
OBJS	+= ioc_duart.o
OBJS	+= ioc_main.o
OBJS	+= ioc_rtc.o
OBJS	+= ioc_scsi_d.o
OBJS	+= ioc_scsi_t.o

CFLAGS	+= -Wall -Werror -pthread -g -O0
CFLAGS	+= -I. -IInfra -IMusashi -IIoc -DMUSASHI_CNF='"musashi_conf.h"'
LDFLAGS	+= -lm

BINFILES += IOC_EEPROM.bin
BINFILES += RESHA_EEPROM.bin

M68K_INCL = \
	Musashi/m68kcpu.h \
	Musashi/m68kmmu.h \
	Musashi/softfloat/softfloat.h \
	Ioc/musashi_conf.h \
	m68kops.h

CLI_INCL = \
	Infra/r1000.h \
	Infra/elastic.h

test:	r1000 ${BINFILES}
	./r1000 \
		-T /critter/_r1000 \
		-t 3 \
		"console serial /dev/nmdm0A" \
		"console > _.console" \
		"duart > _.duart" \
		"reset"

r1000:	${OBJS}
	${CC} -o r1000 ${CFLAGS} ${LDFLAGS} ${OBJS}
	rm -f *.tmp

clean:
	rm -f *.o *.tmp r1000 m68kops.h m68kops.c m68kmake

cli.o:			Infra/r1000.h Infra/vav.h Infra/cli.c Ioc/ioc.h
elastic.o:		Infra/r1000.h Infra/elastic.h Infra/elastic.c
elastic_fd.o:		Infra/r1000.h Infra/elastic.h Infra/elastic_fd.c
elastic_match.o:	Infra/r1000.h Infra/elastic.h Infra/elastic_match.c
elastic_tcp.o:		Infra/r1000.h Infra/elastic.h Infra/elastic_tcp.c
main.o:			Infra/r1000.h Infra/main.c
vav.o:			Infra/r1000.h Infra/vav.c

m68kcpu.o:		${M68K_INCL} Musashi/m68kcpu.c
m68kdasm.o:		${M68K_INCL} Musashi/m68kdasm.c
softfloat.o:		${M68K_INCL} Musashi/softfloat/softfloat.c

ioc_cli.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_cli.c
ioc_duart.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_duart.c
ioc_console.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_console.c
ioc_main.o:		${M68K_INCL} Ioc/ioc.h Ioc/ioc_main.c
ioc_rtc.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_rtc.c
ioc_scsi_d.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_scsi_d.c
ioc_scsi_t.o:		${CLI_INCL} Ioc/ioc.h Ioc/ioc_scsi_t.c

m68kops.o:		Musashi/m68kcpu.h m68kops.h m68kops.c
m68kops.h m68kops.c:	m68kmake Ioc/musashi_conf.h
			./m68kmake `pwd` Musashi/m68k_in.c

m68kmake:		Musashi/m68kmake.c

Musashi/m68kcpu.h:	Musashi/m68k.h

flint:
	flexelint flint.lnt ${CFLAGS} Infra/*.c Ioc/*.c

setup:	${BINFILES}
	git clone https://github.com/kstenerud/Musashi
	(cd Musashi && git apply ../Musashi.patch)

${BINFILES}:
	curl -o IOC_EEPROM.bin https://datamuseum.dk/bits/30000502
	curl -o RESHA_EEPROM.bin https://datamuseum.dk/bits/30000503
