# R1000.Emulator

This is the beginnings of an emulator for the Rational R1000/400 computer.

https://datamuseum.dk/wiki/Rational/R1000s400

1. Run `make setup`, this clones the Musashi 68k emulator and downloads the EEPROM images.

2. Manually download https://datamuseum.dk/bits/30000551 somewhere, it needs a gigabyte.

3. Set the path in `Ioc/scsi_d.c` to that file.

4. `kldload nmdm` if not already loaded

5. In a separate window: `cu -l /dev/nmdm0B`

6. Fix `/critter/_r1000` in Makefile to where you want a gigabyte of logfile

7. `make`

8. Wait for 16195351 million instructions and get:

```
	Boot from (Tn or Dn)  [D0] : ^@
	Kernel program (0,1,2) [0] : 
	File system    (0,1,2) [0] : 
	User program   (0,1,2) [0] : 
	Initializing M400S I/O Processor Kernel 4_2_18
	IOP Kernel is initialized
	
	I/O Processor Kernel Crash: error 0806 (hex) at PC=000049F8
	Trapped into debugger.
	RD0 00000000  RD1 00000002  RD2 00000002  RD3 0000000A 
	RD4 00000013  RD5 00000006  RD6 0000001E  RD7 00000005 
	RA0 0000E800  RA1 0003FADA  RA2 00000954  RA3 0003FAD8 
	RA4 0003FFF4  RA5 0003FA96  RA6 0003FB70  ISP 0000FAB8 
	 PC 0000A158  USP 0003FA96  ISP 0000FAB8  MSP 00000000  SR 2704 
	VBR 00000000 ICCR 00000009 ICAR 00000000 XSFC 0 XDFC 0 
	@
```

At this point the kernel is initialized, as jumped to FS which has cleared it's
data segment and moved on to `PROGRAM.0` which calls something in the FS/KERNEL
which does not yet work.

Disassembly of the IOC EEPROM: http://datamuseum.dk/aa/r1k_dfs/be/bed92cf60.html

Disassembly of the RESHA EEPROM: http://datamuseum.dk/aa/r1k_dfs/f3/f3d8d4065.html

Disassembly of the loaded KERNEL.0: http://datamuseum.dk/aa/r1k_dfs/77/77d6c3277.html

Disassembly of the loaded FS.0: http://datamuseum.dk/aa/r1k_dfs/44/442c504ed.html

Disassembly of the loaded PROGRAM.0: http://datamuseum.dk/aa/r1k_dfs/f1/f15447000.html

Hat-Tip to @kstenerud: The 68k20 IOC is emulated with his https://github.com/kstenerud/Musashi

*end*
