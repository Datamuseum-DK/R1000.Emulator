# See mkmk.sh

OBJS += Components/tables.o
Components/tables.o: Components/tables.c 
	${CC} -o Components/tables.o -c Components/tables.c

OBJS += Components/1489.o
Components/1489.o: Components/1489.cc \
    Components/1489.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/1489.o Components/1489.cc

OBJS += Components/16KX4.o
Components/16KX4.o: Components/16KX4.cc \
    Components/16KX4.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/16KX4.o Components/16KX4.cc

OBJS += Components/1MEG.o
Components/1MEG.o: Components/1MEG.cc \
    Components/1MEG.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/1MEG.o Components/1MEG.cc

OBJS += Components/2149.o
Components/2149.o: Components/2149.cc \
    Components/2149.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/2149.o Components/2149.cc

OBJS += Components/2167.o
Components/2167.o: Components/2167.cc \
    Components/2167.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/2167.o Components/2167.cc

OBJS += Components/25S10.o
Components/25S10.o: Components/25S10.cc \
    Components/25S10.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/25S10.o Components/25S10.cc

OBJS += Components/25S558.o
Components/25S558.o: Components/25S558.cc \
    Components/25S558.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/25S558.o Components/25S558.cc

OBJS += Components/2661B.o
Components/2661B.o: Components/2661B.cc \
    Components/2661B.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/2661B.o Components/2661B.cc

OBJS += Components/2681.o
Components/2681.o: Components/2681.cc \
    Components/2681.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/2681.o Components/2681.cc

OBJS += Components/28256.o
Components/28256.o: Components/28256.cc \
    Components/28256.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/28256.o Components/28256.cc

OBJS += Components/2KX8.o
Components/2KX8.o: Components/2KX8.cc \
    Components/2KX8.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/2KX8.o Components/2KX8.cc

OBJS += Components/58167.o
Components/58167.o: Components/58167.cc \
    Components/58167.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/58167.o Components/58167.cc

OBJS += Components/64KX1.o
Components/64KX1.o: Components/64KX1.cc \
    Components/64KX1.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/64KX1.o Components/64KX1.cc

OBJS += Components/68020.o
Components/68020.o: Components/68020.cc \
    Components/68020.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/68020.o Components/68020.cc

OBJS += Components/8051.o
Components/8051.o: Components/8051.cc \
    Components/8051.hh \
    Infra/context.h \
    Chassis/r1000sc.h \
    Diag/diagproc.h
	${SC_CC} -o Components/8051.o Components/8051.cc

OBJS += Components/8641.o
Components/8641.o: Components/8641.cc \
    Components/8641.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/8641.o Components/8641.cc

OBJS += Components/93S48.o
Components/93S48.o: Components/93S48.cc \
    Components/93S48.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/93S48.o Components/93S48.cc

OBJS += Components/BUSPAL.o
Components/BUSPAL.o: Components/BUSPAL.cc \
    Components/BUSPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/BUSPAL.o Components/BUSPAL.cc

OBJS += Components/CMDPAL.o
Components/CMDPAL.o: Components/CMDPAL.cc \
    Components/CMDPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/CMDPAL.o Components/CMDPAL.cc

OBJS += Components/CNTRPAL.o
Components/CNTRPAL.o: Components/CNTRPAL.cc \
    Components/CNTRPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/CNTRPAL.o Components/CNTRPAL.cc

OBJS += Components/DELAY.o
Components/DELAY.o: Components/DELAY.cc \
    Components/DELAY.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/DELAY.o Components/DELAY.cc

OBJS += Components/DIBRPAL.o
Components/DIBRPAL.o: Components/DIBRPAL.cc \
    Components/DIBRPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/DIBRPAL.o Components/DIBRPAL.cc

OBJS += Components/DISTPAL.o
Components/DISTPAL.o: Components/DISTPAL.cc \
    Components/DISTPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/DISTPAL.o Components/DISTPAL.cc

OBJS += Components/DLY_35.o
Components/DLY_35.o: Components/DLY_35.cc \
    Components/DLY_35.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/DLY_35.o Components/DLY_35.cc

OBJS += Components/DRADPAL.o
Components/DRADPAL.o: Components/DRADPAL.cc \
    Components/DRADPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/DRADPAL.o Components/DRADPAL.cc

OBJS += Components/DRCPAL.o
Components/DRCPAL.o: Components/DRCPAL.cc \
    Components/DRCPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/DRCPAL.o Components/DRCPAL.cc

OBJS += Components/F00.o
Components/F00.o: Components/F00.cc \
    Components/F00.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F00.o Components/F00.cc

OBJS += Components/F02.o
Components/F02.o: Components/F02.cc \
    Components/F02.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F02.o Components/F02.cc

OBJS += Components/F04.o
Components/F04.o: Components/F04.cc \
    Components/F04.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F04.o Components/F04.cc

OBJS += Components/F08.o
Components/F08.o: Components/F08.cc \
    Components/F08.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F08.o Components/F08.cc

OBJS += Components/F10.o
Components/F10.o: Components/F10.cc \
    Components/F10.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F10.o Components/F10.cc

OBJS += Components/F113.o
Components/F113.o: Components/F113.cc \
    Components/F113.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F113.o Components/F113.cc

OBJS += Components/F133.o
Components/F133.o: Components/F133.cc \
    Components/F133.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F133.o Components/F133.cc

OBJS += Components/F138.o
Components/F138.o: Components/F138.cc \
    Components/F138.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F138.o Components/F138.cc

OBJS += Components/F139.o
Components/F139.o: Components/F139.cc \
    Components/F139.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F139.o Components/F139.cc

OBJS += Components/F148.o
Components/F148.o: Components/F148.cc \
    Components/F148.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F148.o Components/F148.cc

OBJS += Components/F151.o
Components/F151.o: Components/F151.cc \
    Components/F151.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F151.o Components/F151.cc

OBJS += Components/F153.o
Components/F153.o: Components/F153.cc \
    Components/F153.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F153.o Components/F153.cc

OBJS += Components/F157.o
Components/F157.o: Components/F157.cc \
    Components/F157.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F157.o Components/F157.cc

OBJS += Components/F158.o
Components/F158.o: Components/F158.cc \
    Components/F158.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F158.o Components/F158.cc

OBJS += Components/F163.o
Components/F163.o: Components/F163.cc \
    Components/F163.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F163.o Components/F163.cc

OBJS += Components/F169.o
Components/F169.o: Components/F169.cc \
    Components/F169.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F169.o Components/F169.cc

OBJS += Components/F174.o
Components/F174.o: Components/F174.cc \
    Components/F174.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F174.o Components/F174.cc

OBJS += Components/F175.o
Components/F175.o: Components/F175.cc \
    Components/F175.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F175.o Components/F175.cc

OBJS += Components/F181.o
Components/F181.o: Components/F181.cc \
    Components/F181.hh \
    Components/tables.h \
    Infra/context.h \
    Chassis/r1000sc.h \
    Components/F181_tbl.h
	${SC_CC} -o Components/F181.o Components/F181.cc

OBJS += Components/F182.o
Components/F182.o: Components/F182.cc \
    Components/F182.hh \
    Components/tables.h \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F182.o Components/F182.cc

OBJS += Components/F189.o
Components/F189.o: Components/F189.cc \
    Components/F189.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F189.o Components/F189.cc

Components/F194.hh:: Components/F194_pins.hh
	@touch Components/F194.hh

OBJS += Components/F194.o
Components/F194.o: Components/F194.cc \
    Components/F194.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F194.o Components/F194.cc

OBJS += Components/F20.o
Components/F20.o: Components/F20.cc \
    Components/F20.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F20.o Components/F20.cc

OBJS += Components/F240.o
Components/F240.o: Components/F240.cc Components/xbuf_doit.h \
    Components/F240.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F240.o Components/F240.cc

OBJS += Components/F244.o
Components/F244.o: Components/F244.cc Components/xbuf_doit.h \
    Components/F244.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F244.o Components/F244.cc

OBJS += Components/F245.o
Components/F245.o: Components/F245.cc \
    Components/F245.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F245.o Components/F245.cc

Components/F251.hh:: Components/F251_pins.hh
	@touch Components/F251.hh

OBJS += Components/F251.o
Components/F251.o: Components/F251.cc \
    Components/F251.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F251.o Components/F251.cc

OBJS += Components/F257.o
Components/F257.o: Components/F257.cc \
    Components/F257.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F257.o Components/F257.cc

OBJS += Components/F258.o
Components/F258.o: Components/F258.cc \
    Components/F258.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F258.o Components/F258.cc

OBJS += Components/F260.o
Components/F260.o: Components/F260.cc \
    Components/F260.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F260.o Components/F260.cc

OBJS += Components/F280.o
Components/F280.o: Components/F280.cc \
    Components/F280.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F280.o Components/F280.cc

OBJS += Components/F283.o
Components/F283.o: Components/F283.cc \
    Components/F283.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F283.o Components/F283.cc

Components/F299.hh:: Components/F299_pins.hh
	@touch Components/F299.hh

OBJS += Components/F299.o
Components/F299.o: Components/F299.cc \
    Components/F299.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F299.o Components/F299.cc

OBJS += Components/F30.o
Components/F30.o: Components/F30.cc \
    Components/F30.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F30.o Components/F30.cc

OBJS += Components/F37.o
Components/F37.o: Components/F37.cc \
    Components/F37.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F37.o Components/F37.cc

OBJS += Components/F373.o
Components/F373.o: Components/F373.cc \
    Components/F373.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F373.o Components/F373.cc

Components/F374.hh:: Components/F374_pins.hh
	@touch Components/F374.hh

OBJS += Components/F374.o
Components/F374.o: Components/F374.cc \
    Components/F374.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F374.o Components/F374.cc

OBJS += Components/F38.o
Components/F38.o: Components/F38.cc \
    Components/F38.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F38.o Components/F38.cc

OBJS += Components/F381.o
Components/F381.o: Components/F381.cc \
    Components/F381.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F381.o Components/F381.cc

OBJS += Components/F40.o
Components/F40.o: Components/F40.cc \
    Components/F40.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F40.o Components/F40.cc

OBJS += Components/F51.o
Components/F51.o: Components/F51.cc \
    Components/F51.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F51.o Components/F51.cc

OBJS += Components/F521.o
Components/F521.o: Components/F521.cc \
    Components/F521.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F521.o Components/F521.cc

OBJS += Components/F579.o
Components/F579.o: Components/F579.cc \
    Components/F579.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F579.o Components/F579.cc

OBJS += Components/F64.o
Components/F64.o: Components/F64.cc \
    Components/F64.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F64.o Components/F64.cc

OBJS += Components/F652.o
Components/F652.o: Components/F652.cc \
    Components/F652.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F652.o Components/F652.cc

OBJS += Components/F74.o
Components/F74.o: Components/F74.cc \
    Components/F74.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F74.o Components/F74.cc

OBJS += Components/F85.o
Components/F85.o: Components/F85.cc \
    Components/F85.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F85.o Components/F85.cc

OBJS += Components/F86.o
Components/F86.o: Components/F86.cc \
    Components/F86.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F86.o Components/F86.cc

OBJS += Components/LS125.o
Components/LS125.o: Components/LS125.cc \
    Components/LS125.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/LS125.o Components/LS125.cc

OBJS += Components/LUXXPAL.o
Components/LUXXPAL.o: Components/LUXXPAL.cc \
    Components/LUXXPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/LUXXPAL.o Components/LUXXPAL.cc

OBJS += Components/MARPAL.o
Components/MARPAL.o: Components/MARPAL.cc \
    Components/MARPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/MARPAL.o Components/MARPAL.cc

OBJS += Components/MUXLPAL.o
Components/MUXLPAL.o: Components/MUXLPAL.cc \
    Components/MUXLPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/MUXLPAL.o Components/MUXLPAL.cc

OBJS += Components/NOVRAM.o
Components/NOVRAM.o: Components/NOVRAM.cc \
    Components/NOVRAM.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/NOVRAM.o Components/NOVRAM.cc

OBJS += Components/P2K8R.o
Components/P2K8R.o: Components/P2K8R.cc \
    Components/P2K8R.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/P2K8R.o Components/P2K8R.cc

OBJS += Components/PAxxx.o
Components/PAxxx.o: Components/PAxxx.cc \
    Components/PAxxx.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/PAxxx.o Components/PAxxx.cc

Components/PBxxx.o: Components/PBxxx.cc \
    Components/PBxxx.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/PBxxx.o Components/PBxxx.cc

OBJS += Components/PHITPAL.o
Components/PHITPAL.o: Components/PHITPAL.cc \
    Components/PHITPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/PHITPAL.o Components/PHITPAL.cc

OBJS += Components/RDRPAL.o
Components/RDRPAL.o: Components/RDRPAL.cc \
    Components/RDRPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/RDRPAL.o Components/RDRPAL.cc

OBJS += Components/SETPAL.o
Components/SETPAL.o: Components/SETPAL.cc \
    Components/SETPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/SETPAL.o Components/SETPAL.cc

OBJS += Components/TAGAPAL.o
Components/TAGAPAL.o: Components/TAGAPAL.cc \
    Components/TAGAPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/TAGAPAL.o Components/TAGAPAL.cc

OBJS += Components/TPARPAL.o
Components/TPARPAL.o: Components/TPARPAL.cc \
    Components/TPARPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/TPARPAL.o Components/TPARPAL.cc

OBJS += Components/TRACPAL.o
Components/TRACPAL.o: Components/TRACPAL.cc \
    Components/TRACPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/TRACPAL.o Components/TRACPAL.cc

OBJS += Components/TSCPAL.o
Components/TSCPAL.o: Components/TSCPAL.cc \
    Components/TSCPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/TSCPAL.o Components/TSCPAL.cc

OBJS += Components/TSXXPAL.o
Components/TSXXPAL.o: Components/TSXXPAL.cc \
    Components/TSXXPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/TSXXPAL.o Components/TSXXPAL.cc

Components/XADD8.hh:: Components/XADD8_pins.hh
	@touch Components/XADD8.hh

OBJS += Components/XADD8.o
Components/XADD8.o: Components/XADD8.cc Components/xbuf_doit.h \
    Components/XADD8.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XADD8.o Components/XADD8.cc

OBJS += Components/XADD16.o
Components/XADD16.o: Components/XADD16.cc Components/xbuf_doit.h \
    Components/XADD16.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XADD16.o Components/XADD16.cc

OBJS += Components/XALU8.o
Components/XALU8.o: Components/XALU8.cc Components/xbuf_doit.h \
    Components/XALU8.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XALU8.o Components/XALU8.cc

Components/XBUF4.hh:: Components/XBUF4_pins.hh
	@touch Components/XBUF4.hh

OBJS += Components/XBUF4.o
Components/XBUF4.o: Components/XBUF4.cc Components/xbuf_doit.h \
    Components/XBUF4.hh \
    Components/XBUF_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XBUF4.o Components/XBUF4.cc

Components/XBUF8.hh:: Components/XBUF8_pins.hh
	@touch Components/XBUF8.hh

OBJS += Components/XBUF8.o
Components/XBUF8.o: Components/XBUF8.cc Components/xbuf_doit.h \
    Components/XBUF8.hh \
    Components/XBUF_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XBUF8.o Components/XBUF8.cc

Components/XBUF16.hh:: Components/XBUF16_pins.hh
	@touch Components/XBUF16.hh

OBJS += Components/XBUF16.o
Components/XBUF16.o: Components/XBUF16.cc Components/xbuf_doit.h \
    Components/XBUF16.hh \
    Components/XBUF_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XBUF16.o Components/XBUF16.cc

Components/XBUF20.hh:: Components/XBUF20_pins.hh
	@touch Components/XBUF20.hh

OBJS += Components/XBUF20.o
Components/XBUF20.o: Components/XBUF20.cc Components/xbuf_doit.h \
    Components/XBUF20.hh \
    Components/XBUF_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XBUF20.o Components/XBUF20.cc

Components/XBUF32.hh:: Components/XBUF32_pins.hh
	@touch Components/XBUF32.hh

OBJS += Components/XBUF32.o
Components/XBUF32.o: Components/XBUF32.cc Components/xbuf_doit.h \
    Components/XBUF32.hh \
    Components/XBUF_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XBUF32.o Components/XBUF32.cc

Components/XBUF48.hh:: Components/XBUF48_pins.hh
	@touch Components/XBUF48.hh

OBJS += Components/XBUF48.o
Components/XBUF48.o: Components/XBUF48.cc Components/xbuf_doit.h \
    Components/XBUF48.hh \
    Components/XBUF_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XBUF48.o Components/XBUF48.cc

Components/XBUF64.hh:: Components/XBUF64_pins.hh
	@touch Components/XBUF64.hh

OBJS += Components/XBUF64.o
Components/XBUF64.o: Components/XBUF64.cc Components/xbuf_doit.h \
    Components/XBUF64.hh \
    Components/XBUF_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XBUF64.o Components/XBUF64.cc

OBJS += Components/XCTR16.o
Components/XCTR16.o: Components/XCTR16.cc \
    Components/XCTR16.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XCTR16.o Components/XCTR16.cc

OBJS += Components/XDRAM.o
Components/XDRAM.o: Components/XDRAM.cc \
    Components/XDRAM.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XDRAM.o Components/XDRAM.cc

OBJS += Components/XDRAM9.o
Components/XDRAM9.o: Components/XDRAM9.cc \
    Components/XDRAM9.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XDRAM9.o Components/XDRAM9.cc

Components/XEQ16.hh:: Components/XEQ16_pins.hh
	@touch Components/XEQ16.hh

OBJS += Components/XEQ16.o
Components/XEQ16.o: Components/XEQ16.cc \
    Components/XEQ16.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XEQ16.o Components/XEQ16.cc

Components/XEQ20.hh:: Components/XEQ20_pins.hh
	@touch Components/XEQ20.hh

OBJS += Components/XEQ20.o
Components/XEQ20.o: Components/XEQ20.cc \
    Components/XEQ20.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XEQ20.o Components/XEQ20.cc

Components/XEQ32.hh:: Components/XEQ32_pins.hh
	@touch Components/XEQ32.hh

OBJS += Components/XEQ32.o
Components/XEQ32.o: Components/XEQ32.cc \
    Components/XEQ32.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XEQ32.o Components/XEQ32.cc

Components/XEQ40.hh:: Components/XEQ40_pins.hh
	@touch Components/XEQ40.hh

OBJS += Components/XEQ40.o
Components/XEQ40.o: Components/XEQ40.cc \
    Components/XEQ40.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XEQ40.o Components/XEQ40.cc

OBJS += Components/XIOCRAM.o
Components/XIOCRAM.o: Components/XIOCRAM.cc \
    Components/XIOCRAM.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XIOCRAM.o Components/XIOCRAM.cc

Components/XLAT16.hh:: Components/XLAT16_pins.hh
	@touch Components/XLAT16.hh

OBJS += Components/XLAT16.o
Components/XLAT16.o: Components/XLAT16.cc \
    Components/XLAT16.hh \
    Components/XLAT_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XLAT16.o Components/XLAT16.cc

Components/XLAT64.hh:: Components/XLAT64_pins.hh
	@touch Components/XLAT64.hh

OBJS += Components/XLAT64.o
Components/XLAT64.o: Components/XLAT64.cc \
    Components/XLAT64.hh \
    Components/XLAT_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XLAT64.o Components/XLAT64.cc

Components/XMUX6.hh:: Components/XMUX6_pins.hh
	@touch Components/XMUX6.hh

OBJS += Components/XMUX6.o
Components/XMUX6.o: Components/XMUX6.cc \
    Components/XMUX6.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XMUX6.o Components/XMUX6.cc

Components/XMUX7.hh:: Components/XMUX7_pins.hh
	@touch Components/XMUX7.hh

OBJS += Components/XMUX7.o
Components/XMUX7.o: Components/XMUX7.cc \
    Components/XMUX7.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XMUX7.o Components/XMUX7.cc

Components/XMUX16.hh:: Components/XMUX16_pins.hh
	@touch Components/XMUX16.hh

OBJS += Components/XMUX16.o
Components/XMUX16.o: Components/XMUX16.cc \
    Components/XMUX16.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XMUX16.o Components/XMUX16.cc

Components/XMUX232.hh:: Components/XMUX232_pins.hh
	@touch Components/XMUX232.hh

OBJS += Components/XMUX232.o
Components/XMUX232.o: Components/XMUX232.cc \
    Components/XMUX232.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XMUX232.o Components/XMUX232.cc

OBJS += Components/XMUXI16.o
Components/XMUXI16.o: Components/XMUXI16.cc \
    Components/XMUXI16.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XMUXI16.o Components/XMUXI16.cc

OBJS += Components/XPAXXXL.o

Components/XPAXXXL.hh:: Components/XPAXXXL_pins.hh
	@touch Components/XPAXXXL.hh

Components/XPAXXXL.o: Components/XPAXXXL.cc \
    Components/XPAXXXL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XPAXXXL.o Components/XPAXXXL.cc

OBJS += Components/PBxxx.o

OBJS += Components/XPAR64.o
Components/XPAR64.o: Components/XPAR64.cc \
    Components/XPAR64.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XPAR64.o Components/XPAR64.cc

OBJS += Components/XPAR67.o
Components/XPAR67.o: Components/XPAR67.cc \
    Components/XPAR67.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XPAR67.o Components/XPAR67.cc

Components/XREG16.hh:: Components/XREG16_pins.hh
	@touch Components/XREG16.hh

OBJS += Components/XREG16.o
Components/XREG16.o: Components/XREG16.cc \
    Components/XREG16.hh \
    Components/XREG_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XREG16.o Components/XREG16.cc

Components/XREG20.hh:: Components/XREG20_pins.hh
	@touch Components/XREG20.hh

OBJS += Components/XREG20.o
Components/XREG20.o: Components/XREG20.cc \
    Components/XREG20.hh \
    Components/XREG_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XREG20.o Components/XREG20.cc

Components/XREG32.hh:: Components/XREG32_pins.hh
	@touch Components/XREG32.hh

OBJS += Components/XREG32.o
Components/XREG32.o: Components/XREG32.cc \
    Components/XREG32.hh \
    Components/XREG_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XREG32.o Components/XREG32.cc

Components/XREG64.hh:: Components/XREG64_pins.hh
	@touch Components/XREG64.hh

OBJS += Components/XREG64.o
Components/XREG64.o: Components/XREG64.cc \
    Components/XREG64.hh \
    Components/XREG_doit.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XREG64.o Components/XREG64.cc

Components/XRFRAM.hh:: Components/XRFRAM_pins.hh
	@touch Components/XRFRAM.hh

OBJS += Components/XRFRAM.o
Components/XRFRAM.o: Components/XRFRAM.cc \
    Components/XRFRAM.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XRFRAM.o Components/XRFRAM.cc

Components/XSR12.hh:: Components/XSR12_pins.hh
	@touch Components/XSR12.hh

Components/XSR8.hh:: Components/XSR8_pins.hh
	@touch Components/XSR8.hh

OBJS += Components/XSR8.o
Components/XSR8.o: Components/XSR8.cc \
    Components/XSR8.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XSR8.o Components/XSR8.cc

OBJS += Components/XSR12.o
Components/XSR12.o: Components/XSR12.cc \
    Components/XSR12.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XSR12.o Components/XSR12.cc

Components/XSR16.hh:: Components/XSR16_pins.hh
	@touch Components/XSR16.hh

OBJS += Components/XSR16.o
Components/XSR16.o: Components/XSR16.cc \
    Components/XSR16.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XSR16.o Components/XSR16.cc

Components/R1000.Emulator/Components.hh:: Components/R1000.Emulator/Components_pins.hh
	@touch Components/R1000.Emulator/Components.hh

OBJS += Components/XSTKRAM.o
Components/XSTKRAM.o: Components/XSTKRAM.cc \
    Components/XSTKRAM.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XSTKRAM.o Components/XSTKRAM.cc

OBJS += Components/XTAGRAM.o
Components/XTAGRAM.o: Components/XTAGRAM.cc \
    Components/XTAGRAM.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XTAGRAM.o Components/XTAGRAM.cc

OBJS += Components/XUAMUX.o
Components/XUAMUX.o: Components/XUAMUX.cc \
    Components/XUAMUX.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XUAMUX.o Components/XUAMUX.cc

Components/XWCSRAM.hh:: Components/XWCSRAM_pins.hh
	@touch Components/XWCSRAM.hh

OBJS += Components/XWCSRAM.o
Components/XWCSRAM.o: Components/XWCSRAM.cc \
    Components/XWCSRAM.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XWCSRAM.o Components/XWCSRAM.cc

OBJS += Components/XXCV9.o
Components/XXCV9.o: Components/XXCV9.cc \
    Components/XXCV9.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XXCV9.o Components/XXCV9.cc

OBJS += Components/XXCV64.o
Components/XXCV64.o: Components/XXCV64.cc \
    Components/XXCV64.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XXCV64.o Components/XXCV64.cc

OBJS += Components/XZCNT.o
Components/XZCNT.o: Components/XZCNT.cc \
    Components/XZCNT.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XZCNT.o Components/XZCNT.cc
