// Machine-generated, see R1000.HwDoc/ImageProcessing/Chipdesc/chip.py
#define XREG16_PIN_CLK pin1
#define XREG16_PIN_INV pin2
#define XREG16_PIN_OE pin3
#define XREG16_PIN_I0 pin6
#define XREG16_PIN_Y0 pin7
#define XREG16_PIN_I1 pin8
#define XREG16_PIN_Y1 pin9
#define XREG16_PIN_I2 pin10
#define XREG16_PIN_Y2 pin11
#define XREG16_PIN_I3 pin12
#define XREG16_PIN_Y3 pin13
#define XREG16_PIN_I4 pin14
#define XREG16_PIN_Y4 pin15
#define XREG16_PIN_I5 pin16
#define XREG16_PIN_Y5 pin17
#define XREG16_PIN_I6 pin18
#define XREG16_PIN_Y6 pin19
#define XREG16_PIN_I7 pin20
#define XREG16_PIN_Y7 pin21
#define XREG16_PIN_I8 pin22
#define XREG16_PIN_Y8 pin23
#define XREG16_PIN_I9 pin24
#define XREG16_PIN_Y9 pin25
#define XREG16_PIN_I10 pin26
#define XREG16_PIN_Y10 pin27
#define XREG16_PIN_I11 pin28
#define XREG16_PIN_Y11 pin29
#define XREG16_PIN_I12 pin30
#define XREG16_PIN_Y12 pin31
#define XREG16_PIN_I13 pin32
#define XREG16_PIN_Y13 pin33
#define XREG16_PIN_I14 pin34
#define XREG16_PIN_Y14 pin35
#define XREG16_PIN_I15 pin36
#define XREG16_PIN_Y15 pin37
#ifdef ANON_PINS
    #define PIN_CLK XREG16_PIN_CLK
    #define PIN_INV XREG16_PIN_INV
    #define PIN_OE XREG16_PIN_OE
    #define PIN_I0 XREG16_PIN_I0
    #define PIN_Y0 XREG16_PIN_Y0
    #define PIN_I1 XREG16_PIN_I1
    #define PIN_Y1 XREG16_PIN_Y1
    #define PIN_I2 XREG16_PIN_I2
    #define PIN_Y2 XREG16_PIN_Y2
    #define PIN_I3 XREG16_PIN_I3
    #define PIN_Y3 XREG16_PIN_Y3
    #define PIN_I4 XREG16_PIN_I4
    #define PIN_Y4 XREG16_PIN_Y4
    #define PIN_I5 XREG16_PIN_I5
    #define PIN_Y5 XREG16_PIN_Y5
    #define PIN_I6 XREG16_PIN_I6
    #define PIN_Y6 XREG16_PIN_Y6
    #define PIN_I7 XREG16_PIN_I7
    #define PIN_Y7 XREG16_PIN_Y7
    #define PIN_I8 XREG16_PIN_I8
    #define PIN_Y8 XREG16_PIN_Y8
    #define PIN_I9 XREG16_PIN_I9
    #define PIN_Y9 XREG16_PIN_Y9
    #define PIN_I10 XREG16_PIN_I10
    #define PIN_Y10 XREG16_PIN_Y10
    #define PIN_I11 XREG16_PIN_I11
    #define PIN_Y11 XREG16_PIN_Y11
    #define PIN_I12 XREG16_PIN_I12
    #define PIN_Y12 XREG16_PIN_Y12
    #define PIN_I13 XREG16_PIN_I13
    #define PIN_Y13 XREG16_PIN_Y13
    #define PIN_I14 XREG16_PIN_I14
    #define PIN_Y14 XREG16_PIN_Y14
    #define PIN_I15 XREG16_PIN_I15
    #define PIN_Y15 XREG16_PIN_Y15
#endif
#define XREG16_PINLIST \
	sc_in <sc_logic> pin1; \
	sc_in <sc_logic> pin2; \
	sc_in <sc_logic> pin3; \
	sc_in <sc_logic> pin6; \
	sc_out <sc_logic> pin7; \
	sc_in <sc_logic> pin8; \
	sc_out <sc_logic> pin9; \
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
	sc_out <sc_logic> pin21; \
	sc_in <sc_logic> pin22; \
	sc_out <sc_logic> pin23; \
	sc_in <sc_logic> pin24; \
	sc_out <sc_logic> pin25; \
	sc_in <sc_logic> pin26; \
	sc_out <sc_logic> pin27; \
	sc_in <sc_logic> pin28; \
	sc_out <sc_logic> pin29; \
	sc_in <sc_logic> pin30; \
	sc_out <sc_logic> pin31; \
	sc_in <sc_logic> pin32; \
	sc_out <sc_logic> pin33; \
	sc_in <sc_logic> pin34; \
	sc_out <sc_logic> pin35; \
	sc_in <sc_logic> pin36; \
	sc_out <sc_logic> pin37;

#ifdef ANON_PINS
#define PIN_PAIRS(macro) \
	macro(15, PIN_I0, PIN_Y0) \
	macro(14, PIN_I1, PIN_Y1) \
	macro(13, PIN_I2, PIN_Y2) \
	macro(12, PIN_I3, PIN_Y3) \
	macro(11, PIN_I4, PIN_Y4) \
	macro(10, PIN_I5, PIN_Y5) \
	macro(9, PIN_I6, PIN_Y6) \
	macro(8, PIN_I7, PIN_Y7) \
	macro(7, PIN_I8, PIN_Y8) \
	macro(6, PIN_I9, PIN_Y9) \
	macro(5, PIN_I10, PIN_Y10) \
	macro(4, PIN_I11, PIN_Y11) \
	macro(3, PIN_I12, PIN_Y12) \
	macro(2, PIN_I13, PIN_Y13) \
	macro(1, PIN_I14, PIN_Y14) \
	macro(0, PIN_I15, PIN_Y15)
#endif
