# R1000.Emulator

This is the beginnings of an emulator for the Rational R1000/400 computer.

https://datamuseum.dk/wiki/Rational/R1000s400

1. Run `make setup`, this clones the Musashi 68k emulator and downloads the EEPROM images.

2. Manually download https://datamuseum.dk/bits/30000551 somewhere, it needs a gigabyte.

3. Edit `TRACE_FILE` and `DISK0_IMAGE` at the top of `Makefile`

4. In a separate window: `tail -F _.console`

5. `make`

6. Wait for who knows how many million simulated instructions and get:

```
     R1000-400 IOC SELFTEST 1.3.2
        512 KB memory ... [OK]
        Memory parity ... [OK]
        I/O bus control ... [OK]
        I/O bus map parity ... [OK]
        I/O bus transactions ... [OK]
        PIT ... [OK]
        Modem DUART channel ... Warning: DUART crystal out of spec! ... [OK]
        Diagnostic DUART channel ... [OK]
        Clock / Calendar ... Warning: Calendar crystal out of spec! ... [OK]
    Checking for RESHA board
        RESHA EEProm Interface ... [OK]
    Downloading RESHA EEProm 0 - TEST  - Warning: Detected Checksum Error
    Downloading RESHA EEProm 1 - LANCE
    Downloading RESHA EEProm 2 - DISK    - Warning: Detected Checksum Error
    Downloading RESHA EEProm 3 - TAPE
        DIAGNOSTIC MODEM ... DISABLED
        RESHA DISK SCSI sub-tests ... [OK]
        RESHA TAPE SCSI sub-tests ... [OK]
        Local interrupts ... [OK]
        Illegal reference protection ... [OK]
        I/O bus parity ... [OK]
        I/O bus spurious interrupts ... [OK]
        Temperature sensors ... [OK]
        IOC diagnostic processor ... [OK]
        Power margining ... [OK]
        Clock margining ... [OK]
    Selftest passed
    
    Restarting R1000-400S February 12th, 2021 at 20:00:00
    
    Logical tape drive 0 is an 8mm cartridge tape drive.
    Logical tape drive 1 is declared non-existent.
    Logical tape drive 2 is declared non-existent.
    Logical tape drive 3 is declared non-existent.
    Booting I/O Processor with Bootstrap version 0.4
    
    Boot from (Tn or Dn)  [D0] :
    Kernel program (0,1,2) [0] :
    File system    (0,1,2) [0] :
    User program   (0,1,2) [0] :
    Initializing M400S I/O Processor Kernel 4_2_18
    Disk  0 is ONLINE and WRITE ENABLED
    Disk  1 is ONLINE and WRITE ENABLED
    IOP Kernel is initialized
    Initializing diagnostic file system ... [OK]
    ====================================================
    Restarting system after loss of AC power
    
    CrashSave has created tombstone file R1000_DUMP1.
    Memory 0 is in configuration but does not respond.
    
    >>> NOTICE: the EPROM WRT PROT switch is OFF (at front of RESHA board) <<<
    >>> WARNING: the system clock or power is margined <<<
    CLI/CRASH MENU - options are:
      1 => enter CLI
      2 => make a CRASHDUMP tape
      3 => display CRASH INFO
      4 => Boot DDC configuration
      5 => Boot EEDB configuration
      6 => Boot STANDARD configuration
    Enter option [enter CLI] : 1
    CLI> 
```

Disassembly of the IOC EEPROM: http://datamuseum.dk/aa/r1k_dfs/be/bed92cf60.html

Disassembly of the RESHA EEPROM: http://datamuseum.dk/aa/r1k_dfs/f3/f3d8d4065.html

Disassembly of the sector0 bootstrap: http://datamuseum.dk/aa/r1k_dfs/82/82a46de15.html

Disassembly of the loaded KERNEL.0: http://datamuseum.dk/aa/r1k_dfs/77/77d6c3277.html

Disassembly of the loaded FS.0: http://datamuseum.dk/aa/r1k_dfs/61/6176fa9c7.html

Disassembly of the loaded PROGRAM.0: http://datamuseum.dk/aa/r1k_dfs/f1/f15447000.html

Hat-Tip to @kstenerud: The 68k20 IOC is emulated with his https://github.com/kstenerud/Musashi

# TODO

Add a cli commands:

* `breakpoint address` -- Stop emulation and dump 68K registers.  Leave CPU stopped.

* `continue` -- Continue after breakpoint

* `step` -- Execute single instruction while stopped

* `examine address` -- Dump 256 bytes of memory.

* `deposit {address|register} data` -- Change state	

* `coredump` -- Drop copy of IOC-RAM to file

*end*
