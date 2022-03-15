// Machine-generated, see R1000.HwDoc/ImageProcessing/Chipdesc/chip.py
#define XADD8_PIN_CI pin1
#define XADD8_PIN_CO pin2
#define XADD8_PIN_A0 pin3
#define XADD8_PIN_A1 pin4
#define XADD8_PIN_A2 pin5
#define XADD8_PIN_A3 pin6
#define XADD8_PIN_A4 pin7
#define XADD8_PIN_A5 pin8
#define XADD8_PIN_A6 pin9
#define XADD8_PIN_A7 pin10
#define XADD8_PIN_B0 pin11
#define XADD8_PIN_Y0 pin12
#define XADD8_PIN_B1 pin13
#define XADD8_PIN_Y1 pin14
#define XADD8_PIN_B2 pin15
#define XADD8_PIN_Y2 pin16
#define XADD8_PIN_B3 pin17
#define XADD8_PIN_Y3 pin18
#define XADD8_PIN_B4 pin19
#define XADD8_PIN_Y4 pin20
#define XADD8_PIN_B5 pin21
#define XADD8_PIN_Y5 pin22
#define XADD8_PIN_B6 pin23
#define XADD8_PIN_Y6 pin24
#define XADD8_PIN_B7 pin25
#define XADD8_PIN_Y7 pin26
#ifdef ANON_PINS
    #define PIN_CI XADD8_PIN_CI
    #define PIN_CO XADD8_PIN_CO
    #define PIN_A0 XADD8_PIN_A0
    #define PIN_A1 XADD8_PIN_A1
    #define PIN_A2 XADD8_PIN_A2
    #define PIN_A3 XADD8_PIN_A3
    #define PIN_A4 XADD8_PIN_A4
    #define PIN_A5 XADD8_PIN_A5
    #define PIN_A6 XADD8_PIN_A6
    #define PIN_A7 XADD8_PIN_A7
    #define PIN_B0 XADD8_PIN_B0
    #define PIN_Y0 XADD8_PIN_Y0
    #define PIN_B1 XADD8_PIN_B1
    #define PIN_Y1 XADD8_PIN_Y1
    #define PIN_B2 XADD8_PIN_B2
    #define PIN_Y2 XADD8_PIN_Y2
    #define PIN_B3 XADD8_PIN_B3
    #define PIN_Y3 XADD8_PIN_Y3
    #define PIN_B4 XADD8_PIN_B4
    #define PIN_Y4 XADD8_PIN_Y4
    #define PIN_B5 XADD8_PIN_B5
    #define PIN_Y5 XADD8_PIN_Y5
    #define PIN_B6 XADD8_PIN_B6
    #define PIN_Y6 XADD8_PIN_Y6
    #define PIN_B7 XADD8_PIN_B7
    #define PIN_Y7 XADD8_PIN_Y7
#endif
#define XADD8_PINLIST \
	sc_in <sc_logic> pin1; \
	sc_out <sc_logic> pin2; \
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
	sc_out <sc_logic> pin24; \
	sc_in <sc_logic> pin25; \
	sc_out <sc_logic> pin26;

#ifdef ANON_PINS
#define PIN_SETS_IN(macro) \
	macro(7, PIN_A0) \
	macro(7, PIN_B0) \
	macro(6, PIN_A1) \
	macro(6, PIN_B1) \
	macro(5, PIN_A2) \
	macro(5, PIN_B2) \
	macro(4, PIN_A3) \
	macro(4, PIN_B3) \
	macro(3, PIN_A4) \
	macro(3, PIN_B4) \
	macro(2, PIN_A5) \
	macro(2, PIN_B5) \
	macro(1, PIN_A6) \
	macro(1, PIN_B6) \
	macro(0, PIN_A7) \
	macro(0, PIN_B7) \
	macro(0, PIN_CI)

#define PIN_SETS_OUT(macro) \
	macro(8, PIN_CO) \
	macro(7, PIN_Y0) \
	macro(6, PIN_Y1) \
	macro(5, PIN_Y2) \
	macro(4, PIN_Y3) \
	macro(3, PIN_Y4) \
	macro(2, PIN_Y5) \
	macro(1, PIN_Y6) \
	macro(0, PIN_Y7)
#endif
