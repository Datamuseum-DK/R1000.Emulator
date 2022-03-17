
#define XBUF4_PIN_INV pin1
#define XBUF4_PIN_OE pin2
#define XBUF4_PIN_I0 pin3
#define XBUF4_PIN_Y0 pin4
#define XBUF4_PIN_I1 pin5
#define XBUF4_PIN_Y1 pin6
#define XBUF4_PIN_I2 pin7
#define XBUF4_PIN_Y2 pin8
#define XBUF4_PIN_I3 pin9
#define XBUF4_PIN_Y3 pin10
#ifdef ANON_PINS
    #define PIN_INV XBUF4_PIN_INV
    #define PIN_OE XBUF4_PIN_OE
    #define PIN_I0 XBUF4_PIN_I0
    #define PIN_Y0 XBUF4_PIN_Y0
    #define PIN_I1 XBUF4_PIN_I1
    #define PIN_Y1 XBUF4_PIN_Y1
    #define PIN_I2 XBUF4_PIN_I2
    #define PIN_Y2 XBUF4_PIN_Y2
    #define PIN_I3 XBUF4_PIN_I3
    #define PIN_Y3 XBUF4_PIN_Y3
#endif
#define XBUF4_PINLIST \
	sc_in <sc_logic> pin1; \
	sc_in <sc_logic> pin2; \
	sc_in <sc_logic> pin3; \
	sc_out <sc_logic> pin4; \
	sc_in <sc_logic> pin5; \
	sc_out <sc_logic> pin6; \
	sc_in <sc_logic> pin7; \
	sc_out <sc_logic> pin8; \
	sc_in <sc_logic> pin9; \
	sc_out <sc_logic> pin10;

#ifdef ANON_PINS
#define PIN_PAIRS(macro) \
	macro(3, PIN_I0, PIN_Y0) \
	macro(2, PIN_I1, PIN_Y1) \
	macro(1, PIN_I2, PIN_Y2) \
	macro(0, PIN_I3, PIN_Y3)
#endif
