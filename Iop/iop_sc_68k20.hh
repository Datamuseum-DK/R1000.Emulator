#ifndef IOC_SC_68K20_HH
#define IOC_SC_68K20_HH
// This is included in the IOC's 68k20 SystemC class

#ifdef __cplusplus
extern "C" {
#endif

struct ioc_sc_bus_xact {
	uint32_t	address;
	uint32_t	data;
	int		width;
	int		is_write;
	unsigned	sc_state;
};

struct ioc_sc_bus_xact * ioc_sc_bus_get_xact(void);
void ioc_sc_bus_done(struct ioc_sc_bus_xact **);

uint32_t ioc_bus_xact_schedule(uint32_t adr, uint32_t data, int width,
    int is_write, int is_sync);

#ifdef __cplusplus
}
#endif

#endif /* IOC_SC_68K20_HH */
