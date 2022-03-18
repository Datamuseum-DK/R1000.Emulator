// Machine-generated, see R1000.HwDoc/ImageProcessing/Chipdesc/chip.py
#define XBUF8_PIN_INV_ pin1
#define XBUF8_PIN_OE_ pin2
#define XBUF8_PIN_I0 pin3
#define XBUF8_PIN_Y0 pin4
#define XBUF8_PIN_I1 pin5
#define XBUF8_PIN_Y1 pin6
#define XBUF8_PIN_I2 pin7
#define XBUF8_PIN_Y2 pin8
#define XBUF8_PIN_I3 pin9
#define XBUF8_PIN_Y3 pin10
#define XBUF8_PIN_I4 pin11
#define XBUF8_PIN_Y4 pin12
#define XBUF8_PIN_I5 pin13
#define XBUF8_PIN_Y5 pin14
#define XBUF8_PIN_I6 pin15
#define XBUF8_PIN_Y6 pin16
#define XBUF8_PIN_I7 pin17
#define XBUF8_PIN_Y7 pin18
#ifdef ANON_PINS
    #define PIN_INV_ XBUF8_PIN_INV_
    #define PIN_OE_ XBUF8_PIN_OE_
    #define PIN_I0 XBUF8_PIN_I0
    #define PIN_Y0 XBUF8_PIN_Y0
    #define PIN_I1 XBUF8_PIN_I1
    #define PIN_Y1 XBUF8_PIN_Y1
    #define PIN_I2 XBUF8_PIN_I2
    #define PIN_Y2 XBUF8_PIN_Y2
    #define PIN_I3 XBUF8_PIN_I3
    #define PIN_Y3 XBUF8_PIN_Y3
    #define PIN_I4 XBUF8_PIN_I4
    #define PIN_Y4 XBUF8_PIN_Y4
    #define PIN_I5 XBUF8_PIN_I5
    #define PIN_Y5 XBUF8_PIN_Y5
    #define PIN_I6 XBUF8_PIN_I6
    #define PIN_Y6 XBUF8_PIN_Y6
    #define PIN_I7 XBUF8_PIN_I7
    #define PIN_Y7 XBUF8_PIN_Y7
#endif
#define XBUF8_PINLIST \
	sc_in <sc_logic> pin1; \
	sc_in <sc_logic> pin2; \
	sc_in <sc_logic> pin3; \
	sc_out <sc_logic> pin4; \
	sc_in <sc_logic> pin5; \
	sc_out <sc_logic> pin6; \
	sc_in <sc_logic> pin7; \
	sc_out <sc_logic> pin8; \
	sc_in <sc_logic> pin9; \
	sc_out <sc_logic> pin10; \
	sc_in <sc_logic> pin11; \
	sc_out <sc_logic> pin12; \
	sc_in <sc_logic> pin13; \
	sc_out <sc_logic> pin14; \
	sc_in <sc_logic> pin15; \
	sc_out <sc_logic> pin16; \
	sc_in <sc_logic> pin17; \
	sc_out <sc_logic> pin18;

#ifdef ANON_PINS
#define PIN_PAIRS(macro) \
	macro(7, PIN_I0, PIN_Y0) \
	macro(6, PIN_I1, PIN_Y1) \
	macro(5, PIN_I2, PIN_Y2) \
	macro(4, PIN_I3, PIN_Y3) \
	macro(3, PIN_I4, PIN_Y4) \
	macro(2, PIN_I5, PIN_Y5) \
	macro(1, PIN_I6, PIN_Y6) \
	macro(0, PIN_I7, PIN_Y7)
#endif
