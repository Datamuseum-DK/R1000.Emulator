/*
 * This file contains binary hotfixes to improve the emulation experience.
 *
 * Mostly this is about shortening delay loops
 */

#include <stdint.h>
#include <string.h>
#include "r1000.h"
#include "ioc.h"
#include "vend.h"

void
Ioc_HotFix_Ioc(uint8_t *eeprom)
{
	/*
	 * 80000060 20 3c 00 00 82 35          MOVE.L  #0x00008235,D0
	 * 80000066 51 c8 ff fe                DBF     D0,0x80000066
	 */
	vbe32enc(eeprom + 0x0062, 0x3);

	/*
	 * 800000fe 20 3c 00 01 04 6a          MOVE.L  #0x0001046a,D0
	 * 80000104 53 80                      SUBQ.L  #0x1,D0
	 */
	vbe32enc(eeprom + 0x0100, 0x3);

	/*
	 * 80000132 20 3c 00 00 82 35          MOVE.L  #0x00008235,D0
	 * 80000138 51 c8 ff fe                DBF     D0,0x80000138
	 */
	vbe32enc(eeprom + 0x0134, 0x3);

	/*
	 * 80000338 20 3c 00 00 82 35          MOVE.L  #0x00008235,D0
	 * 8000033e 51 c8 ff fe                DBF     D0,0x8000033e
	 */
	vbe32enc(eeprom + 0x033a, 0x3);

	/*
	 * 80000348 20 3c 00 00 82 35          MOVE.L  #0x00008235,D0
	 * 8000034e 51 c8 ff fe                DBF     D0,0x8000034e
	 */
	vbe32enc(eeprom + 0x034a, 0x3);

	/*
	 * 80003822 4e 96                      OUTTEXT
	 * 80003824 20 31 39 00                .TXT    ' 19'
	 */
	vbe32enc(eeprom + 0x3825, 0x20323000); // Y2K
}

void
Ioc_HotFix_Resha(uint8_t *eeprom)
{
	/*
	 * 00077176 24 3c 00 0b 42 0f          MOVE.L  #0x000b420f,D2
	 * 0007717c 08 38 00 01 90 01          BTST.B  #0x1,IO_UART_STATUS
	 * 00077182 67 1e                      BEQ     0x771a2
	 */
	vbe32enc(eeprom + 0x7178, 0x3);

	/*
	 * 00074588 20 3c 00 00 05 00          MOVE.L  #0x00000500,D0
	 * 0007458e 53 80                      SUBQ.L  #0x1,D0
	 * 00074590 66 fc                      BNE     0x7458e
	 */
	vbe32enc(eeprom + 0x458a, 0x3);

        /*
	 * 00077386                    SCSI_T_AWAIT_INTERRUPT():
	 * 00077386 2f 00                      MOVE.L  D0,-(A7)
	 * 00077388 20 3c 00 00 05 00          MOVE.L  #0x00000500,D0
	 * 0007738e 53 80                      SUBQ.L  #0x1,D0
	 * 00077390 66 fc                      BNE     0x7738e
         */
	vbe32enc(eeprom + 0x738a, 0x3);
}

void
Ioc_HotFix_Bootloader(uint8_t *ram)
{

	/*
	 * 000541ee 32 3c 7f ff                MOVE.W  #0x7fff,D1
	 * 000541f2 20 3c 00 00 05 00          MOVE.L  #0x00000500,D0
	 * 000541f8 53 80                      SUBQ.L  #0x1,D0
	 * 000541fa 66 fc                      BNE     0x541f8
	 * 000541fc 08 39 00 07 93 03 e8 1f    BTST.B  #0x7,IO_SCSI_D_1f_AUX_STATUS
	 * 00054204 66 1c                      BNE     0x54222
	 * 00054206 51 c9 ff ea                DBF     D1,0x541f2
	 */
	if (vbe32dec(ram + 0x541ee) == 0x323c7fff)
		vbe16enc(ram + 0x541f0, 3);
	if (vbe32dec(ram + 0x541f4) == 0x500)
		vbe32enc(ram + 0x541f4, 3);
}

static void
hotfix_kernel_4_2_18(uint8_t *ram)
{
	/*
	 * 00005d14                    DELAY_LOOP(D1):
	 * 00005d14 2f 00                      MOVE.L  D0,-(A7)
	 * 00005d16 20 3c 00 00 05 00          MOVE.L  #0x00000500,D0
	 * 00005d1c 53 80                      SUBQ.L  #0x1,D0
	 * 00005d1e 66 fc                      BNE     0x5d1c
	 * 00005d20 51 c9 ff f4                DBF     D1,0x5d16
	 * 00005d24 20 1f                      MOVE.L  (A7)+,D0
	 * 00005d26 4e 75                      RTS
	 */
	vbe32enc(ram + 0x5d18, 3);

	/*
	 * 000066d0 20 3c 00 23 7c 4d     MOVE.L  #0x00237c4d,D0
	 * 000066d6 53 80                 SUBQ.L  #0x1,D0
	 * 000066d8 66 fc                 BNE     0x66d6
	 */
	vbe32enc(ram + 0x66d2, 3);

	/*
	 * 00009818 20 3c 00 03 ff ff          MOVE.L  #0x0003ffff,D0
	 * 0000981e 13 fc 00 0a 93 03 ec 00    MOVE.B  #0x0a,IO_SCSI_T_00_OWN_ID_CDB_SIZE
	 * 00009826 13 fc 00 00 93 03 ec 18    MOVE.B  #0x00,IO_SCSI_T_18_CMD
	 * 0000982e 4e b9 00 00 97 f4          JSR     0x97f4
	 */
	vbe32enc(ram + 0x981a, 3);


	/*
	 * 00009af6 20 3c 00 07 ff ff          MOVE.L  #0x0007ffff,D0
	 * 00009afc 33 fc 00 30 93 03 e0 08    MOVE.W  #0x0030,IO_RESHA_RES_CTL
	 * 00009b04 4e b9 00 00 97 f4          JSR     0x97f4
	 */
	vbe32enc(ram + 0x9af8, 3);
}

void
Ioc_HotFix_Kernel(uint8_t *ram)
{
	unsigned u, v;
	char buf[BUFSIZ];

	for (u = 0; u < 0x800; u++)
		if (ram[u] == 0x40 && vbe32dec(ram+u) == 0x40282329)
			break;
        if (u == 0x800) {
		printf("Kernel not recognized, no patches applied.\n");
		return;
	}
	for (v = 0; v < BUFSIZ; v++) {
		switch(ram[u + v]) {
		case 0x00:
		case 0x0a:
		case 0x22:
		case 0x3e:
		case 0x5c:
			buf[v] = '\0';
			break;
		default:
			buf[v] = ram[u + v];
			continue;
		}
		break;
	}
	if (!strcmp(buf, "@(#)400S IOP KERNEL,4_2_18,92/08/06,16:15:00")) {
		hotfix_kernel_4_2_18(ram);
	} else {
		printf("No hotfix for kernel %s\n", buf);
		return;
	}
	printf("Hotfixes applied to kernel %s\n", buf);
}
