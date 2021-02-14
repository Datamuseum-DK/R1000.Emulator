/*
 * RTC - MM58167B Microprocesor Real Time Clock
 * --------------------------------------------
 *
 * Chip Select: 0xffff8xxx
 *
 */

#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "r1000.h"
#include "ioc.h"

#include "memspace.h"

static pthread_mutex_t rtc_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t rtc_cond = PTHREAD_COND_INITIALIZER;
static pthread_t rtc_thr;

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

static void
ioc_rtc_setclock(void)
{
	struct timeval tv;
	struct tm *tm;

	AZ(gettimeofday(&tv, NULL));
	rtcregs[0] = ((tv.tv_usec / 1000) % 10) * 16;
	rtcregs[1] = (tv.tv_usec / 10000) % 10;
	rtcregs[1] |= ((tv.tv_usec / 100000) % 10) * 16;
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

static void *
ioc_rtc_thread(void *priv)
{
	struct sim *cs = priv;

	AZ(pthread_mutex_lock(&rtc_mtx));
	callout_signal_cond(cs, &rtc_cond, &rtc_mtx, 1000000, 1000000);
	while (1) {
		AZ(pthread_cond_wait(&rtc_cond, &rtc_mtx));
		trace(2, "RTC tick\n");

		rtcregs[0x0e] |= 0x01;

		rtcregs[0] += 0x10;
		if (rtcregs[0] <= 0x90)
			continue;
		rtcregs[0] = 0x00;

		rtcregs[1] += 0x01;
		if ((rtcregs[1] & 0x0f) <= 0x09)
			continue;
		rtcregs[1] += 0x16;
		if ((rtcregs[1] & 0xf0 ) <= 0x9)
			continue;
		rtcregs[1] = 0x00;

		rtcregs[2] += 0x01;
		if ((rtcregs[2] & 0x0f) <= 0x09)
			continue;
		rtcregs[2] += 0x16;
		if ((rtcregs[2] & 0xf0 ) <= 0x50)
			continue;
		rtcregs[2] = 0x00;

		rtcregs[3] += 0x01;
		if ((rtcregs[3] & 0x0f) <= 0x09)
			continue;
		rtcregs[3] += 0x16;
		if ((rtcregs[3] & 0xf0 ) <= 0x50)
			continue;
		rtcregs[3] = 0x00;

		rtcregs[4] += 0x01;
		if (rtcregs[4] <= 0x24) {
			if ((rtcregs[4] & 0x0f) <= 0x09)
				continue;
			rtcregs[4] += 0x16;
			continue;
		}
		rtcregs[4] = 0x00;

		// XXX [5] wday
		// XXX [6] mday
		// XXX [7] mon
	}
}

void v_matchproto_(mem_pre_read)
io_rtc_pre_read(int debug, uint8_t *space, unsigned width, unsigned adr)
{

	if (debug) return;
	assert (width == 1);
	assert (adr < 32);

	AZ(pthread_mutex_lock(&rtc_mtx));
	space[adr] = rtcregs[adr];
	AZ(pthread_mutex_unlock(&rtc_mtx));
	trace(TRACE_IO, "RTC R [%x] -> %x\n", adr, space[adr]);
	if (adr == 0x0e)
		space[0x0e] = 0;
}

void v_matchproto_(mem_post_write)
io_rtc_post_write(int debug, uint8_t *space, unsigned width, unsigned adr)
{

	if (debug) return;
	assert (width == 1);
	assert (adr < 32);
	trace(TRACE_IO, "DUART W [%x] <- %x\n", adr, space[adr]);
	AZ(pthread_mutex_lock(&rtc_mtx));
	rtcregs[adr] = space[adr];
	AZ(pthread_mutex_unlock(&rtc_mtx));
}

void
ioc_rtc_init(struct sim *cs)
{

	ioc_rtc_setclock();
	AZ(pthread_create(&rtc_thr, NULL, ioc_rtc_thread, cs));
}
