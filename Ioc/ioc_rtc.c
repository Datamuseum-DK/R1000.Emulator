/*
 * RTC - MM58167B Microprocesor Real Time Clock
 * --------------------------------------------
 *
 * Chip Select: 0xffff8xxx
 *
 */

#include <time.h>
#include <sys/time.h>

#include "r1000.h"
#include "ioc.h"

static uint8_t rtcregs[32] = {
	0x00,	// Counter - Milliseconds
	0x00,	// Counter - Hundredths and Tenths of Seconds
	0x00,	// Counter - Seconds
	0x00,	// Counter - Minutes

	0x00,	// Counter - Hours
	0x00,	// Counter - Day of Week
	0x00,	// Counter - Day of Month
	0x00,	// Counter - Month

	0x00,	// NVRAM - Milliseconds
	20,	// NVRAM - (Year - 1)
	0x00,	// NVRAM - Seconds
	0x00,	// NVRAM - Minutes

	0x00,	// NVRAM - Hours
	0x00,	// NVRAM - Day of Week
	0x00,	// NVRAM - Day of Month
	0x00,	// NVRAM - Month

	0x00,	// Interrupt Status Register
	0x00,	// Interrupt Control Register
	0x00,	// Counters Reset
	0x00,	// RAM Reset

	0x00,	// Status Bit
	0x00,	// GO Command
	0x00,	// STANDBY INTERRUPT~
	0x00,	// Undefined

	0x00,	// Undefined
	0x00,	// Undefined
	0x00,	// Undefined
	0x00,	// Undefined

	0x00,	// Undefined
	0x00,	// Undefined
	0x00,	// Undefined
	0x00,	// Test Mode
};

unsigned int v_matchproto_(iofunc_f)
io_rtc(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{
	int reg;
	struct timeval tv;
	struct tm *tm;

	IO_TRACE_WRITE(2, "RTC");

	reg = address & 0x1f;
	if (op[0] == 'R' && reg < 0x8) {
		AZ(gettimeofday(&tv, NULL));
		rtcregs[0] = ((tv.tv_usec / 1000) % 10) * 16;
		rtcregs[1] = (tv.tv_usec / 10000) % 10;
		rtcregs[1] |= ((tv.tv_usec / 100000) % 10) * 16;
		if (reg > 1) {
			tm = gmtime(&tv.tv_sec);
			rtcregs[2] = (tm->tm_sec % 10);
			rtcregs[2] |= (tm->tm_sec / 10) * 16;
			rtcregs[3] = (tm->tm_min % 10);
			rtcregs[3] |= (tm->tm_min / 10) * 16;
			rtcregs[4] = (tm->tm_hour % 10);
			rtcregs[4] |= (tm->tm_hour / 10) * 16;
			rtcregs[5] = tm->tm_wday;
			rtcregs[6] = ((tm->tm_mday+1) % 10);
			rtcregs[6] |= ((tm->tm_mday+1) / 10) * 16;
			rtcregs[7] = ((tm->tm_mon+1) % 10);
			rtcregs[7] |= ((tm->tm_mon+1) / 10) * 16;
		}
	}
	value = func(op, rtcregs, address & 0x1f, value);

	IO_TRACE_READ(2, "RTC");

	return (value);
}
