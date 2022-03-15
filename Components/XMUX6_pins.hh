// Machine-generated, see R1000.HwDoc/ImageProcessing/Chipdesc/chip.py
#define XMUX6_PIN_S pin1
#define XMUX6_PIN_E pin2
#define XMUX6_PIN_A0 pin3
#define XMUX6_PIN_INV pin4
#define XMUX6_PIN_A1 pin5
#define XMUX6_PIN_A2 pin6
#define XMUX6_PIN_A3 pin7
#define XMUX6_PIN_A4 pin8
#define XMUX6_PIN_A5 pin9
#define XMUX6_PIN_B0 pin10
#define XMUX6_PIN_Y0 pin11
#define XMUX6_PIN_B1 pin12
#define XMUX6_PIN_Y1 pin13
#define XMUX6_PIN_B2 pin14
#define XMUX6_PIN_Y2 pin15
#define XMUX6_PIN_B3 pin16
#define XMUX6_PIN_Y3 pin17
#define XMUX6_PIN_B4 pin18
#define XMUX6_PIN_Y4 pin19
#define XMUX6_PIN_B5 pin20
#define XMUX6_PIN_Y5 pin21
#ifdef ANON_PINS
    #define PIN_S XMUX6_PIN_S
    #define PIN_E XMUX6_PIN_E
    #define PIN_A0 XMUX6_PIN_A0
    #define PIN_INV XMUX6_PIN_INV
    #define PIN_A1 XMUX6_PIN_A1
    #define PIN_A2 XMUX6_PIN_A2
    #define PIN_A3 XMUX6_PIN_A3
    #define PIN_A4 XMUX6_PIN_A4
    #define PIN_A5 XMUX6_PIN_A5
    #define PIN_B0 XMUX6_PIN_B0
    #define PIN_Y0 XMUX6_PIN_Y0
    #define PIN_B1 XMUX6_PIN_B1
    #define PIN_Y1 XMUX6_PIN_Y1
    #define PIN_B2 XMUX6_PIN_B2
    #define PIN_Y2 XMUX6_PIN_Y2
    #define PIN_B3 XMUX6_PIN_B3
    #define PIN_Y3 XMUX6_PIN_Y3
    #define PIN_B4 XMUX6_PIN_B4
    #define PIN_Y4 XMUX6_PIN_Y4
    #define PIN_B5 XMUX6_PIN_B5
    #define PIN_Y5 XMUX6_PIN_Y5
#endif
#define XMUX6_PINLIST \
	sc_in <sc_logic> pin1; \
	sc_in <sc_logic> pin2; \
	sc_in <sc_logic> pin3; \
	sc_in <sc_logic> pin4; \
	sc_in <sc_logic> pin5; \
	sc_in <sc_logic> pin6; \
	sc_in <sc_logic> pin7; \
	sc_in <sc_logic> pin8; \
	sc_in <sc_logic> pin9; \
	sc_in <sc_logic> pin10; \
	sc_out <sc_logic> pin11; \
	sc_in <sc_logic> pin12; \
	sc_out <sc_logic> pin13; \
	sc_in <sc_logic> pin14; \
	sc_out <sc_logic> pin15; \
	sc_in <sc_logic> pin16; \
	sc_out <sc_logic> pin17; \
	sc_in <sc_logic> pin18; \
	sc_out <sc_logic> pin19; \
	sc_in <sc_logic> pin20; \
	sc_out <sc_logic> pin21;

#ifdef ANON_PINS
#define PIN_SETS(macro) \
	macro(PIN_A0, PIN_B0, PIN_Y0) \
	macro(PIN_A1, PIN_B1, PIN_Y1) \
	macro(PIN_A2, PIN_B2, PIN_Y2) \
	macro(PIN_A3, PIN_B3, PIN_Y3) \
	macro(PIN_A4, PIN_B4, PIN_Y4) \
	macro(PIN_A5, PIN_B5, PIN_Y5)
#endif
