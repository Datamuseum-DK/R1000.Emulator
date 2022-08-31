'''
   Produce _memspace.[ch]
   ----------------------

   Ideas:
      SFP qualifiers
      Value return function for r/o
'''

from makemem_class import Range, System

def main():
    ''' IOC memory layout '''
    ioc = System("ioc", 0, 1<<32)

    ioc += Range("ram", 0x00000000, 0x00080000)

    ioc += Range("ioc_eeprom", 0x80000000, 0x80008000)

    ioc += Range("resha_page", 0x9303e00a, 0x3, post_write = True)

    ioc += Range("resha_eeprom", 0x9303e300, 0xff, pre_read = True)

    ioc += Range("scsi_dma", 0x9303e100, 0xf, post_write = True)

    ioc += Range("scsi_ctl", 0x9303e000, 0xf, pre_read = True, post_write = True)

    ioc += Range("scsi_d", 0x9303e800, 0x9303e900, mask = 0x1f, pre_read = 1, post_write = 1)

    ioc += Range("scsi_t", 0x9303ec00, 0x9303ec20, mask = 0x1f, pre_read = 1, post_write = 1)

    ioc += Range("io_map", 0xa1000000, 0xa1002000)

    # 32 bit writes on 0x400 boundaries, so only shift down 8.
    ioc += Range("xx_map", 0xa2000000, 0xa3000000, shift = 8)

    ioc += Range("io_rtc", 0xffff8000, 0x1f, pre_read = 1, post_write = 1)

    ioc += Range("io_uart", 0xffff9000, 0x7, bidir = False, pre_read = 1, post_write = 1)

    ioc += Range("io_duart", 0xffffa000, 0xf, bidir = False, pre_read = 1, post_write = 1)

    # INT MODEM
    ioc += Range("io_mosart", 0xffffb000, 0xf, pre_read = True, post_write = True)

    # IO_CLR_RUN (reset IOC RUNNING LED)
    ioc += Range("f000", 0xfffff000, 0x3)

    # IO_CPU_GET_REQUEST_p69
    ioc += Range("fifo_req_latch", 0xfffff100, 0x3, pre_read = True)

    # IO_FRONT_PANEL_LED_p27
    ioc += Range("pnlreg", 0xfffff200, 0x3, post_write = True, sc_write = True)

    # SENREG ioc_15
    ioc += Range("senreg", 0xfffff300, 0x3, sc_write = True)

    # DREG5 ioc_14
    ioc += Range("io_sreg4", 0xfffff400, 0x3, sc_write = True)

    # IO_FIFO_INIT_p68_p69
    ioc += Range("fifo_init", 0xfffff500, 0x3, sc_write = True)

    # IO_CPU_RESPONSE_p68
    ioc += Range("fifo_response", 0xfffff600, 0x3, post_write = True)

    # IO_CPU_REQUEST_p69
    ioc += Range("fifo_request", 0xfffff700, 0x3, post_write = True)

    # IO_READ_STATUS_p24
    ioc += Range("io_sreg8", 0xfffff800, 0x3, pre_read = True)

    # IO_CLEAR_BERR_p24
    ioc += Range("f900", 0xfffff900, 0x3, sc_write = True)

    # IO_CONTROL_p28
    ioc += Range("fc00", 0xfffffc00, 0x3)

    # IO_CLR_PFINT_p23
    ioc += Range("pfint", 0xfffffd00, 0x3, sc_write = True)

    # IO_CPU_CONTROL_PSU_MARGIN_DREG4_p13
    ioc += Range("dreg4", 0xfffffe00, 0x3, sc_write = True)

    # IO_READ_SENSE_p25
    ioc += Range("irq_vector", 0xffffff00, 0xff, pre_read = True)

    ioc.produce_c("Iop/_memcfg")
    ioc.produce_h("Iop/_memcfg")

if __name__ == "__main__":
    main()
