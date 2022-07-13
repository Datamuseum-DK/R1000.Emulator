// This source file is included in the IOC's 68K20's doit() SystemC function


	unsigned ipl;

	if (!state->xact)
		state->xact = ioc_sc_bus_get_xact();

	if (!state->xact) {
		BUS_IPL_READ(ipl);
		if (ipl != state->last_ipl) {
			TRACE(<< "IPL-CHANGE " << BUS_IPL_TRACE() << " " << std::hex << ipl);
			state->last_ipl = ipl;
			return;
		}
		if (ipl != 7) {
			ioc_sc_bus_start_iack(ipl);
		}
	}

	if (!state->xact) {
		PIN_DS = 1;
		PIN_AS = 1;
		PIN_DBEN = 1;
		PIN_ECS = 1;
		PIN_WR = 1;
#ifdef BUS_A_Z
		BUS_A_Z();
#endif
		BUS_D_Z();
		BUS_FC_WRITE(0);
		BUS_SIZ_WRITE(0);
		return;
	}
	TRACE(
		<< "BXPA state= "
		<< state->xact->sc_state
		<< " adr= "
		<< std::hex << state->xact->address
		<< " data= "
		<< std::hex << state->xact->data
		<< " width = "
		<< state->xact->width
		<< " write= "
		<< state->xact->is_write
		<< " DSACK "
		<< PIN_DSACK0
		<< PIN_DSACK1
		<< " IPL "
		<< BUS_IPL_TRACE()
	);
	switch (state->xact->sc_state) {
	case 100:
		PIN_ECS = 0;
		BUS_FC_WRITE(state->xact->fc);
		BUS_A_WRITE(state->xact->address);
		BUS_FC_WRITE(5);
		BUS_SIZ_WRITE(state->xact->width);
		PIN_WR = state->xact->is_write == 0;
		next_trigger(PIN_CLK.negedge_event());
		state->xact->sc_state++;
		break;
	case 101:
		PIN_ECS = 1;
		PIN_DBEN = 0;
		PIN_AS = 0;
		next_trigger(PIN_CLK.posedge_event());
		state->xact->sc_state++;
		break;
	case 102:
		BUS_D_WRITE(state->xact->data);
		next_trigger(PIN_CLK.negedge_event());
		state->xact->sc_state++;
		break;
	case 103:
		PIN_DS = 0;
		next_trigger(PIN_CLK.negedge_event());
		state->xact->sc_state++;
		break;
	case 104:
		PIN_DS = 1;
		next_trigger(PIN_CLK.negedge_event());
		state->xact->sc_state++;
		break;
	case 105:
	case 106:
	case 107:
	case 108:
	case 109:
	case 110:
	case 111:
		next_trigger(PIN_CLK.negedge_event());
		state->xact->sc_state++;
		break;
	default:
		PIN_DS = 1;
		PIN_AS = 1;
		PIN_DBEN = 1;
		PIN_ECS = 1;
		PIN_WR = 1;
#ifdef BUS_A_Z
		BUS_A_Z();
#endif
		BUS_D_Z();
		BUS_FC_WRITE(0);
		BUS_SIZ_WRITE(0);
		ioc_sc_bus_done(&state->xact);
		break;
	}
