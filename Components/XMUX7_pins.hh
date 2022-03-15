// Machine-generated, see R1000.HwDoc/ImageProcessing/Chipdesc/chip.py
#define XMUX7_PIN_S pin1
#define XMUX7_PIN_E pin2
#define XMUX7_PIN_A0 pin3
#define XMUX7_PIN_INV pin4
#define XMUX7_PIN_A1 pin5
#define XMUX7_PIN_A2 pin6
#define XMUX7_PIN_A3 pin7
#define XMUX7_PIN_A4 pin8
#define XMUX7_PIN_A5 pin9
#define XMUX7_PIN_A6 pin10
#define XMUX7_PIN_B0 pin11
#define XMUX7_PIN_Y0 pin12
#define XMUX7_PIN_B1 pin13
#define XMUX7_PIN_Y1 pin14
#define XMUX7_PIN_B2 pin15
#define XMUX7_PIN_Y2 pin16
#define XMUX7_PIN_B3 pin17
#define XMUX7_PIN_Y3 pin18
#define XMUX7_PIN_B4 pin19
#define XMUX7_PIN_Y4 pin20
#define XMUX7_PIN_B5 pin21
#define XMUX7_PIN_Y5 pin22
#define XMUX7_PIN_B6 pin23
#define XMUX7_PIN_Y6 pin24
#ifdef ANON_PINS
    #define PIN_S XMUX7_PIN_S
    #define PIN_E XMUX7_PIN_E
    #define PIN_A0 XMUX7_PIN_A0
    #define PIN_INV XMUX7_PIN_INV
    #define PIN_A1 XMUX7_PIN_A1
    #define PIN_A2 XMUX7_PIN_A2
    #define PIN_A3 XMUX7_PIN_A3
    #define PIN_A4 XMUX7_PIN_A4
    #define PIN_A5 XMUX7_PIN_A5
    #define PIN_A6 XMUX7_PIN_A6
    #define PIN_B0 XMUX7_PIN_B0
    #define PIN_Y0 XMUX7_PIN_Y0
    #define PIN_B1 XMUX7_PIN_B1
    #define PIN_Y1 XMUX7_PIN_Y1
    #define PIN_B2 XMUX7_PIN_B2
    #define PIN_Y2 XMUX7_PIN_Y2
    #define PIN_B3 XMUX7_PIN_B3
    #define PIN_Y3 XMUX7_PIN_Y3
    #define PIN_B4 XMUX7_PIN_B4
    #define PIN_Y4 XMUX7_PIN_Y4
    #define PIN_B5 XMUX7_PIN_B5
    #define PIN_Y5 XMUX7_PIN_Y5
    #define PIN_B6 XMUX7_PIN_B6
    #define PIN_Y6 XMUX7_PIN_Y6
#endif
#define XMUX7_PINLIST \
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
	sc_in <sc_logic> pin11; \
	sc_out <sc_logic> pin12; \
	sc_in <sc_logic> pin13; \
	sc_out <sc_logic> pin14; \
	sc_in <sc_logic> pin15; \
	sc_out <sc_logic> pin16; \
	sc_in <sc_logic> pin17; \
	sc_out <sc_logic> pin18; \
	sc_in <sc_logic> pin19; \
	sc_out <sc_logic> pin20; \
	sc_in <sc_logic> pin21; \
	sc_out <sc_logic> pin22; \
	sc_in <sc_logic> pin23; \
	sc_out <sc_logic> pin24;

#ifdef ANON_PINS
#define PIN_SETS(macro) \
	macro(PIN_A0, PIN_B0, PIN_Y0) \
	macro(PIN_A1, PIN_B1, PIN_Y1) \
	macro(PIN_A2, PIN_B2, PIN_Y2) \
	macro(PIN_A3, PIN_B3, PIN_Y3) \
	macro(PIN_A4, PIN_B4, PIN_Y4) \
	macro(PIN_A5, PIN_B5, PIN_Y5) \
	macro(PIN_A6, PIN_B6, PIN_Y6)
#endif
