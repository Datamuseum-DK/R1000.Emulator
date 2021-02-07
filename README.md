# R1000.Emulator

This is the beginnings of an emulator for the Rational R1000/400 computer.

https://datamuseum.dk/wiki/Rational/R1000s400

1. Run "make setup", this clones the Musashi 68k emulator and downloads the EEPROM images.

2. Manually download https://datamuseum.dk/bits/30000551 somewhere, it needs a gigabyte.

3. Set the path in Ioc/scsi_d.c to that file.

4. `kldload nmdm` if not already loaded

5. In a separate window: `cu -l /dev/nmdm0B`

6. Fix `/critter/_r1000` in Makefile to where you want a gigabyte of logfile

7. `make`

8. Type return in the `cu` window four times

9. Wait for approx 11 million instructions and get:

	I/O Processor Kernel Crash: error 0792 (hex) at PC=00009B14
	Trapped into debugger.
	RD0 00000000  RD1 00000300  RD2 00000000  RD3 00000003 
	RD4 00000006  RD5 00000000  RD6 00000000  RD7 0000FB04 
	RA0 0000FB04  RA1 00000BBC  RA2 8000259B  RA3 0000800C 
	RA4 0000FC00  RA5 00000000  RA6 000786BE  ISP 0000FACC 
	 PC 0000A158  USP 00000000  ISP 0000FACC  MSP 00000000  SR 2704 
	VBR 00000000 ICCR 00000009 ICAR 00000000 XSFC 0 XDFC 0 

The 68k20 IOC is emulated using https://github.com/kstenerud/Musashi

Disassembly of the IOC EEPROM: http://datamuseum.dk/aa/r1k_dfs/be/bed92cf60.html

Disassembly of the RESHA EEPROM: http://datamuseum.dk/aa/r1k_dfs/f3/f3d8d4065.html

Disassembly of the loaded kernel: http://datamuseum.dk/aa/r1k_dfs/77/77d6c3277.html

*end*
