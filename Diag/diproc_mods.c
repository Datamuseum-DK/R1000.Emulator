
#include "Infra/r1000.h"
#include "Diag/diag.h"

void
diproc1_mod(uint8_t *firmware, unsigned mod)
{

	printf("DIPROC FW1 mod %d\n", mod);
	switch (mod) {
	case 0:
		break;

	case 1:
		// fiu_53 Reorder MAR.NAME[0..7] & F.ADR.NAME[0..7]
		firmware[0xcbc] = 0xe1;
		firmware[0xcc5] = 0xc1;
		firmware[0xcce] = 0xa1;
		firmware[0xcd7] = 0x81;
		firmware[0xce0] = 0x61;
		firmware[0xce9] = 0x41;
		firmware[0xcf2] = 0x21;
		firmware[0xcfb] = 0x01;

		// fiu_53 Reorder MAR.NAME[8..15] & F.ADR.NAME[8..15]
		firmware[0xcbd] = 0xe2;
		firmware[0xcc6] = 0xc2;
		firmware[0xccf] = 0xa2;
		firmware[0xcd8] = 0x82;
		firmware[0xce1] = 0x62;
		firmware[0xcea] = 0x42;
		firmware[0xcf3] = 0x22;
		firmware[0xcfc] = 0x02;

		// fiu_53 Reorder MAR.NAME[16..23] & F.ADR.NAME[16..12]
		firmware[0xcbe] = 0xe3;
		firmware[0xcc7] = 0xc3;
		firmware[0xcd0] = 0xa3;
		firmware[0xcd9] = 0x83;
		firmware[0xce2] = 0x63;
		firmware[0xceb] = 0x43;
		firmware[0xcf4] = 0x23;
		firmware[0xcfd] = 0x03;

		// fiu_53 Reorder MAR.NAME[24..31] & F.ADR.NAME[24..31]
		firmware[0xcbf] = 0xe4;
		firmware[0xcc8] = 0xc4;
		firmware[0xcd1] = 0xa4;
		firmware[0xcda] = 0x84;
		firmware[0xce3] = 0x64;
		firmware[0xcec] = 0x44;
		firmware[0xcf5] = 0x24;
		firmware[0xcfe] = 0x04;

		// fiu_53 Reorder MAR.OFFS[0..7] & F.ADR.OFFS[0..7]
		firmware[0xcc0] = 0xc6;
		firmware[0xcc9] = 0xa6;
		firmware[0xcd2] = 0x86;
		firmware[0xcdb] = 0x66;
		firmware[0xce4] = 0x46;
		firmware[0xced] = 0x26;
		firmware[0xcf6] = 0x06;
		firmware[0xcff] = 0xe5;

		// fiu_53 Reorder MAR.OFFS[8..15] & F.ADR.OFFS[8..15]
		firmware[0xcc1] = 0xc7;
		firmware[0xcca] = 0xa7;
		firmware[0xcd3] = 0x87;
		firmware[0xcdc] = 0x67;
		firmware[0xce5] = 0x47;
		firmware[0xcee] = 0x27;
		firmware[0xcf7] = 0x07;
		firmware[0xd00] = 0xe6;

		// fiu_53 Reorder MAR.OFFS[16..23] & F.ADR.OFFS[16..23]
		firmware[0xcc2] = 0xc8;
		firmware[0xccb] = 0xa8;
		firmware[0xcd4] = 0x88;
		firmware[0xcdd] = 0x68;
		firmware[0xce6] = 0x48;
		firmware[0xcef] = 0x28;
		firmware[0xcf8] = 0x08;
		firmware[0xd01] = 0xe7;

		break;
	default:
		assert("WRONG DIPROC1 mod" == NULL);
	}
}
