#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "8051.hh"

#include "Diag/diagproc.h"

struct scm_8051_state {
	struct ctx ctx;
	struct diagproc_context dctx;
};

#define PORT0(DOMACRO, ARG) \
	do { \
		DOMACRO(pin39, p0, 1<<0, ARG); \
		DOMACRO(pin38, p0, 1<<1, ARG); \
		DOMACRO(pin37, p0, 1<<2, ARG); \
		DOMACRO(pin36, p0, 1<<3, ARG); \
		DOMACRO(pin35, p0, 1<<4, ARG); \
		DOMACRO(pin34, p0, 1<<5, ARG); \
		DOMACRO(pin33, p0, 1<<6, ARG); \
		DOMACRO(pin32, p0, 1<<7, ARG); \
	} while (0)

#define PORT1(DOMACRO, ARG) \
	do { \
		DOMACRO(pin1, p1, 1<<0, ARG); \
		DOMACRO(pin2, p1, 1<<1, ARG); \
		DOMACRO(pin3, p1, 1<<2, ARG); \
		DOMACRO(pin4, p1, 1<<3, ARG); \
		DOMACRO(pin5, p1, 1<<4, ARG); \
		DOMACRO(pin6, p1, 1<<5, ARG); \
		DOMACRO(pin7, p1, 1<<6, ARG); \
		DOMACRO(pin8, p1, 1<<7, ARG); \
	} while (0)

#define PORT2(DOMACRO, ARG) \
	do { \
		DOMACRO(pin21, p2, 1<<0, ARG); \
		DOMACRO(pin22, p2, 1<<1, ARG); \
		DOMACRO(pin23, p2, 1<<2, ARG); \
		DOMACRO(pin24, p2, 1<<3, ARG); \
		DOMACRO(pin25, p2, 1<<4, ARG); \
		DOMACRO(pin26, p2, 1<<5, ARG); \
		DOMACRO(pin27, p2, 1<<6, ARG); \
		DOMACRO(pin28, p2, 1<<7, ARG); \
	} while (0)

#define PORT3(DOMACRO, ARG) \
	do { \
		DOMACRO(pin11, p3, 1<<1, ARG); \
		DOMACRO(pin12, p3, 1<<2, ARG); \
		DOMACRO(pin13, p3, 1<<3, ARG); \
		DOMACRO(pin17, p3, 1<<7, ARG); \
	} while (0)

#define READPORT(pin, port, bit, arg) \
	do { \
		if (!IS_L(pin)) diag_ctrl->port##val |= (bit); \
	} while (0)

#define SETPORT(pin, port, bit, arg) \
	do { \
		if (arg##mask & (bit)) \
			pin = (arg##val & (bit)) ? \
			    sc_logic_Z : sc_logic_0; \
	} while (0)

#define DBG() \
	do { \
		if (state->ctx.do_trace & 1) { \
			TRACE( \
			    << " " << sc_time_stamp() \
			    << " rst " << pin9 \
			    << " cyc " << cycle \
			    << " int0 " << pin12 \
			    << " int1 " << pin13 \
			    << " t0 " << pin14 \
			    << " p1 " << pin8 <<pin7 <<pin6 <<pin5 <<pin4 <<pin3 <<pin2 <<pin1 \
			    << " p2 " << pin28 <<pin27 <<pin26 <<pin25 <<pin24 <<pin23 <<pin22 <<pin21 \
			); \
		} \
	} while(0)

SCM_8051 :: SCM_8051(sc_module_name nm, const char *arg) : sc_module(nm)
{
	(void)arg;

	state = (struct scm_8051_state *)CTX_Get("8051", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	diag_ctrl = DiagProcCreate(this->name(), arg, &state->ctx.do_trace);
	assert(diag_ctrl != NULL);
	cycle = 0;

	SC_METHOD(doit);
	sensitive << pin9 << pin18.pos();

}

void
SCM_8051 :: doit(void)
{
	uint8_t p0val, p0mask;

	state->ctx.activations++;

	if (pin18.posedge()) {
		if (diag_ctrl->do_movx && cycle == 2) {
			p0val = diag_ctrl->movx_data;
			p0mask = 0xff;
			TRACE(<< "P0 data " << std::hex << (unsigned)p0val);
			PORT0(SETPORT, p0);
		}
		if (diag_ctrl->do_movx && cycle == 3) {
			TRACE(<< "WR low");
			pin16 = sc_logic_0;
		}
		if (diag_ctrl->do_movx && cycle == 9) {
			TRACE(<< "WR high");
			pin16 = sc_logic_1;
		}
		if (diag_ctrl->do_movx && cycle == 11) {
			TRACE(<< "P0 back " << std::hex << (unsigned)diag_ctrl->p0val);
			PORT0(SETPORT, diag_ctrl->p0);
		}
		if (++cycle < 12) {
			if (diag_ctrl->do_movx) {
				DBG();
			}
			return;
		}
		pin16 = sc_logic_1;
		cycle = 0;
		diag_ctrl->do_movx = 0;
		diag_ctrl->pin9_reset = IS_H(pin9);
		if (diag_ctrl->next_needs_p1) {
			diag_ctrl->p1val = 0;
			PORT1(READPORT, 0);
			// TRACE(<< "Need P1 " << std::hex << diag_ctrl->p1val);
		}
		if (diag_ctrl->next_needs_p2) {
			diag_ctrl->p2val = 0;
			PORT2(READPORT, 0);
			// TRACE(<< "Need P2 " << std::hex << diag_ctrl->p2val);
		}
		if (diag_ctrl->next_needs_p3) {
			diag_ctrl->p3val = 0;
			PORT3(READPORT, 0);
			// TRACE(<< "Need P3 " << std::hex << diag_ctrl->p3val);
		}
		DiagProcStep(diag_ctrl, &state->dctx);
		if (diag_ctrl->p1mask) {
			TRACE(
			    << "Set P1 "
			    << std::hex << diag_ctrl->p1mask
			    << "::"
			    << std::hex << diag_ctrl->p1val
			);
			PORT1(SETPORT, diag_ctrl->p1);
			diag_ctrl->p1mask = 0;
		}
		if (diag_ctrl->p2mask) {
			TRACE(
			    << "Set P2 "
			    << std::hex << diag_ctrl->p2mask
			    << "::"
			    << std::hex << diag_ctrl->p2val
			);
			PORT2(SETPORT, diag_ctrl->p2);
			diag_ctrl->p2mask = 0;
		}
		if (diag_ctrl->p3mask) {
			TRACE(
			    << "Set P3 "
			    << std::hex << diag_ctrl->p3mask
			    << "::"
			    << std::hex << diag_ctrl->p3val
			);
			TRACE(<< "Set P3");
			PORT3(SETPORT, diag_ctrl->p3);
			diag_ctrl->p3mask = 0;
		}
		if (diag_ctrl->do_movx) {
			TRACE(
				<< "DO MOVX "
				<< std::hex << diag_ctrl->movx_data
				<< " => "
				<< std::hex << diag_ctrl->movx_adr
			);
			/*
			 * As far as I can tell, only the data byte
			 * is used. IOCp63 as the ALE signal but I
			 * cannot see it being used any where.
			 */
			p0val = diag_ctrl->movx_adr;
			p0mask = 0xff;
			TRACE(<< "P0 adr " << std::hex << (unsigned)p0val);
			PORT0(SETPORT, p0);
		}
	}
	DBG();
}
