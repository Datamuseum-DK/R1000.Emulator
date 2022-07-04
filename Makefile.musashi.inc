
M68K_INCL = \
	Musashi/m68kcpu.h \
	Musashi/m68kmmu.h \
	Musashi/softfloat/softfloat.h \
	Iop/musashi_conf.h \
	Musashi/m68kops.h

Musashi/m68kcpu.o:	${M68K_INCL} Musashi/m68kcpu.c
Musashi/m68kdasm.o:	${M68K_INCL} Musashi/m68kdasm.c
Musashi/softfloat/softfloat.o:	${M68K_INCL} Musashi/softfloat/softfloat.c \

Musashi/m68kops.o:	Musashi/m68kcpu.h Musashi/m68kops.h Musashi/m68kops.c
Musashi/m68kops.h Musashi/m68kops.c:	Musashi/m68kmake Iop/musashi_conf.h Musashi/m68k_in.c
			Musashi/m68kmake Musashi Musashi/m68k_in.c

Musashi/m68kmake:	Musashi/m68kmake.c

Musashi/m68kcpu.h:	Musashi/m68k.h

OBJS	+= Musashi/m68kcpu.o Musashi/m68kdasm.o Musashi/m68kops.o Musashi/softfloat/softfloat.o

CLEANFILES += Musashi/m68kops.h Musashi/m68kops.c Musashi/m68kmake
