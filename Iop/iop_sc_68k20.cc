// This source file is included in the IOC's 68K20's doit() SystemC function


	if (!state->xact)
		state->xact = ioc_sc_bus_get_xact();
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
	case 0:
		PIN_ECS = 0;
		BUS_A_WRITE(state->xact->address);
		BUS_FC_WRITE(5);
		BUS_SIZ_WRITE(state->xact->width);
		PIN_WR = state->xact->is_write == 0;
		next_trigger(PIN_CLK.negedge_event());
		state->xact->sc_state = 1;
		break;
	case 1:
		PIN_ECS = 1;
		PIN_DBEN = 0;
		PIN_AS = 0;
		next_trigger(PIN_CLK.posedge_event());
		state->xact->sc_state = 2;
		break;
	case 2:
		BUS_D_WRITE(state->xact->data);
		next_trigger(PIN_CLK.negedge_event());
		state->xact->sc_state = 3;
		break;
	case 3:
		PIN_DS = 0;
		next_trigger(PIN_CLK.negedge_event());
		state->xact->sc_state = 4;
		break;
	case 4:
		PIN_DS = 1;
		next_trigger(PIN_CLK.negedge_event());
		state->xact->sc_state = 5;
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
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
