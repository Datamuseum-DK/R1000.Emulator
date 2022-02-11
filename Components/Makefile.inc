# See mkmk.sh

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
    Infra/context.h \
    Chassis/r1000sc.h \
    Components/F181_tbl.h
	${SC_CC} -o Components/F181.o Components/F181.cc

OBJS += Components/F182.o
Components/F182.o: Components/F182.cc \
    Components/F182.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F182.o Components/F182.cc

OBJS += Components/F189.o
Components/F189.o: Components/F189.cc \
    Components/F189.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F189.o Components/F189.cc

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
Components/F240.o: Components/F240.cc \
    Components/F240.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F240.o Components/F240.cc

OBJS += Components/F244.o
Components/F244.o: Components/F244.cc \
    Components/F244.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F244.o Components/F244.cc

OBJS += Components/F245AB.o
Components/F245AB.o: Components/F245AB.cc \
    Components/F245AB.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F245AB.o Components/F245AB.cc

OBJS += Components/F245BA.o
Components/F245BA.o: Components/F245BA.cc \
    Components/F245BA.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F245BA.o Components/F245BA.cc

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

OBJS += Components/PAxxxL.o
Components/PAxxxL.o: Components/PAxxxL.cc \
    Components/PAxxxL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/PAxxxL.o Components/PAxxxL.cc

OBJS += Components/PBxxx.o
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

OBJS += Components/XBUF64.o
Components/XBUF64.o: Components/XBUF64.cc \
    Components/XBUF64.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XBUF64.o Components/XBUF64.cc

OBJS += Components/XBUFI32.o
Components/XBUFI32.o: Components/XBUFI32.cc \
    Components/XBUFI32.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XBUFI32.o Components/XBUFI32.cc

OBJS += Components/XBUFI64.o
Components/XBUFI64.o: Components/XBUFI64.cc \
    Components/XBUFI64.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XBUFI64.o Components/XBUFI64.cc

OBJS += Components/XIOCRAM.o
Components/XIOCRAM.o: Components/XIOCRAM.cc \
    Components/XIOCRAM.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XIOCRAM.o Components/XIOCRAM.cc

OBJS += Components/XREG64.o
Components/XREG64.o: Components/XREG64.cc \
    Components/XREG64.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XREG64.o Components/XREG64.cc

OBJS += Components/XRFRAM.o
Components/XRFRAM.o: Components/XRFRAM.cc \
    Components/XRFRAM.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XRFRAM.o Components/XRFRAM.cc

OBJS += Components/XWCSRAM.o
Components/XWCSRAM.o: Components/XWCSRAM.cc \
    Components/XWCSRAM.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XWCSRAM.o Components/XWCSRAM.cc
