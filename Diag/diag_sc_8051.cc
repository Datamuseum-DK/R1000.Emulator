
#define PORT0(DOMACRO, ARG) \
	do { \
		DOMACRO(PIN_C7, p0, 1<<0, ARG); \
		DOMACRO(PIN_C6, p0, 1<<1, ARG); \
		DOMACRO(PIN_C5, p0, 1<<2, ARG); \
		DOMACRO(PIN_C4, p0, 1<<3, ARG); \
		DOMACRO(PIN_C3, p0, 1<<4, ARG); \
		DOMACRO(PIN_C2, p0, 1<<5, ARG); \
		DOMACRO(PIN_C1, p0, 1<<6, ARG); \
		DOMACRO(PIN_C0, p0, 1<<7, ARG); \
	} while (0)

#define PORT1(DOMACRO, ARG) \
	do { \
		DOMACRO(PIN_D7, p1, 1<<0, ARG); \
		DOMACRO(PIN_D6, p1, 1<<1, ARG); \
		DOMACRO(PIN_D5, p1, 1<<2, ARG); \
		DOMACRO(PIN_D4, p1, 1<<3, ARG); \
		DOMACRO(PIN_D3, p1, 1<<4, ARG); \
		DOMACRO(PIN_D2, p1, 1<<5, ARG); \
		DOMACRO(PIN_D1, p1, 1<<6, ARG); \
		DOMACRO(PIN_D0, p1, 1<<7, ARG); \
	} while (0)

#define PORT2(DOMACRO, ARG) \
	do { \
		DOMACRO(PIN_D15, p2, 1<<0, ARG); \
		DOMACRO(PIN_D14, p2, 1<<1, ARG); \
		DOMACRO(PIN_D13, p2, 1<<2, ARG); \
		DOMACRO(PIN_D12, p2, 1<<3, ARG); \
		DOMACRO(PIN_D11, p2, 1<<4, ARG); \
		DOMACRO(PIN_D10, p2, 1<<5, ARG); \
		DOMACRO(PIN_D9,  p2, 1<<6, ARG); \
		DOMACRO(PIN_D8,  p2, 1<<7, ARG); \
	} while (0)

#define PORT3(DOMACRO, ARG) \
	do { \
		DOMACRO(PIN_TXD,     p3, 1<<1, ARG); \
		DOMACRO(PIN_INT0not, p3, 1<<2, ARG); \
		DOMACRO(PIN_INT1not, p3, 1<<3, ARG); \
		DOMACRO(PIN_RDnot,   p3, 1<<7, ARG); \
	} while (0)

#define READPORT(pin, port, bit, arg) \
	do { \
		if (!IS_L(pin)) state->diag_ctrl->port##val |= (bit); \
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
			    << " rst " << PIN_RST? \
			    << " cyc " << state->cycle \
			    << " int0 " << PIN_INT0not \
			    << " int1 " << PIN_INT1not \
			    << " t0 " << PIN_T0 \
			    << " C " << BUS_C_TRACE() \
			    << " D " << BUS_D_TRACE() \
			); \
		} \
	} while(0)

	uint8_t p0val, p0mask;

	state->ctx.activations++;

	if (PIN_XTAL2.posedge()) {
		if (state->diag_ctrl->do_movx && state->cycle == 2) {
			p0val = state->diag_ctrl->movx_data;
			p0mask = 0xff;
			TRACE(<< "P0 data " << std::hex << (unsigned)p0val);
			BUS_A_WRITE(p0val);
		}
		if (state->diag_ctrl->do_movx && state->cycle == 3) {
			TRACE(<< "WR low");
			PIN_WRnot = false;
		}
		if (state->diag_ctrl->do_movx && state->cycle == 9) {
			TRACE(<< "WR high");
			PIN_WRnot = true;
		}
		if (state->diag_ctrl->do_movx && state->cycle == 11) {
			TRACE(<< "P0 back " << std::hex << (unsigned)state->diag_ctrl->p0val);
			BUS_A_WRITE(state->diag_ctrl->p0val);
		}
		if (++state->cycle < 12) {
			if (state->diag_ctrl->do_movx) {
				// DBG();
			}
			return;
		}
		PIN_WRnot = true;
		state->cycle = 0;
		state->diag_ctrl->do_movx = 0;
		state->diag_ctrl->pin9_reset = PIN_RST;
		if (state->diag_ctrl->next_needs_p1) {
			state->diag_ctrl->p1val = 0;
			BUS_B_READ(state->diag_ctrl->p1val);
			// TRACE(<< "Need P1 " << std::hex << state->diag_ctrl->p1val);
		}
		if (state->diag_ctrl->next_needs_p2) {
			state->diag_ctrl->p2val = 0;
			BUS_C_READ(state->diag_ctrl->p2val);
			// TRACE(<< "Need P2 " << std::hex << state->diag_ctrl->p2val);
		}
		if (state->diag_ctrl->next_needs_p3) {
			state->diag_ctrl->p3val = 0;
			PORT3(READPORT, 0);
			// TRACE(<< "Need P3 " << std::hex << state->diag_ctrl->p3val);
		}
		DiagProcStep(state->diag_ctrl, &state->dctx);
		if (state->diag_ctrl->p1mask) {
			TRACE(
			    << "Set P1 "
			    << std::hex << state->diag_ctrl->p1mask
			    << "::"
			    << std::hex << state->diag_ctrl->p1val
			);
			if (state->diag_ctrl->p1val == 0xff)
				BUS_B_Z();
			else
				BUS_B_WRITE(state->diag_ctrl->p1val);
				
			state->diag_ctrl->p1mask = 0;
		}
		if (state->diag_ctrl->p2mask) {
			TRACE(
			    << "Set P2 "
			    << std::hex << state->diag_ctrl->p2mask
			    << "::"
			    << std::hex << state->diag_ctrl->p2val
			);
			if (state->diag_ctrl->p2val == 0xff)
				BUS_C_Z();
			else
				BUS_C_WRITE(state->diag_ctrl->p2val);
			state->diag_ctrl->p2mask = 0;
		}
		if (state->diag_ctrl->p3mask) {
			TRACE(
			    << "Set P3 "
			    << std::hex << state->diag_ctrl->p3mask
			    << "::"
			    << std::hex << state->diag_ctrl->p3val
			);
			TRACE(<< "Set P3");
			PORT3(SETPORT, state->diag_ctrl->p3);
			state->diag_ctrl->p3mask = 0;
		}
		if (state->diag_ctrl->do_movx) {
			TRACE(
				<< "DO MOVX "
				<< std::hex << state->diag_ctrl->movx_data
				<< " => "
				<< std::hex << state->diag_ctrl->movx_adr
			);
			/*
			 * As far as I can tell, only the data byte
			 * is used. IOCp63 as the ALE signal but I
			 * cannot see it being used any where.
			 */
			p0val = state->diag_ctrl->movx_adr;
			p0mask = 0xff;
			TRACE(<< "P0 adr " << std::hex << (unsigned)p0val);
			BUS_A_WRITE(p0val);
		}
	}
	// DBG();
