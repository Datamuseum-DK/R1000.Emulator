// This source file is included in the IOC's 68K20's doit() SystemC function


	unsigned ipl, dsack, data;

	if (!state->xact)
		state->xact = ioc_sc_bus_get_xact();

	if (!state->xact) {
		BUS_IPL_READ(ipl);
		if (ipl != state->last_ipl) {
			TRACE(<< "IPL-CHANGE " << BUS_IPL_TRACE() << " " << std::hex << ipl);
			state->last_ipl = ipl;
			return;
		}
		if (ipl != 7)
			ioc_sc_bus_start_iack(ipl);
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
	BUS_D_READ(data);
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
		<< " D " << std::hex << data
	);
	switch (state->xact->sc_state) {
	case 100:
		PIN_ECS = 0;
		BUS_FC_WRITE(state->xact->fc);
		BUS_A_WRITE(state->xact->address);
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

	case 200:
		PIN_ECS = 0;
		BUS_FC_WRITE(state->xact->fc);
		BUS_A_WRITE(state->xact->address);
		BUS_SIZ_WRITE(state->xact->width);
		PIN_WR = state->xact->is_write == 0;
		next_trigger(PIN_CLK.negedge_event());
		state->xact->sc_state++;
		break;
        case 201:
		PIN_ECS = 1;
		PIN_DBEN = 0;
		PIN_AS = 0;
		next_trigger(10, SC_NS);
		state->xact->sc_state++;
		break;
        case 202:
		PIN_DS = 0;
		next_trigger(PIN_CLK.posedge_event());
		state->xact->sc_state++;
		break;
        case 203:
		next_trigger(PIN_CLK.posedge_event());
		state->xact->sc_state++;
		break;
        case 204:
		BUS_DSACK_READ(dsack);
		if (dsack != 3) {
			state->xact->sc_state++;
			BUS_D_READ(state->xact->data);
		}
		next_trigger(PIN_CLK.posedge_event());
		break;
	default:
		if (state->xact->sc_state < 1000) {
			TRACE(
			    << "SC_68K A "
			    << std::hex << state->xact->address
			    << " D "
			    << std::hex <<  state->xact->data
			    << " W "
			    << state->xact->is_write
			);
			state->xact->sc_state = 1000;
		}
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
