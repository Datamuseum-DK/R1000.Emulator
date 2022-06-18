// This source file is included in the IOC's 68K20's doit() SystemC function

	BUS_SIZ_WRITE(0);

	struct ioc_sc_bus_xact *bxpa;

	bxpa = ioc_sc_bus_get_xact();
	if (bxpa) {
		TRACE(
			<< "GOT BXPA adr= "
			<< std::hex << bxpa->address
			<< " data= "
			<< std::hex << bxpa->data
			<< " write= "
			<< bxpa->is_write
		);
		ioc_sc_bus_done(bxpa);
	}
