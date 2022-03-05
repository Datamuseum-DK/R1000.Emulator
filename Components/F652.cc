#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F652.hh"

struct scm_F652_state {
	struct ctx ctx;
	unsigned areg;
	unsigned breg;
};

SCM_F652 :: SCM_F652(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1.pos() << pin23.pos() << pin2 << pin22 << pin3 << pin21
	    << pin4 << pin5 << pin6 << pin7 << pin8 << pin9 << pin10 << pin11
	    << pin20 << pin19 << pin18 << pin17 << pin16 << pin15 << pin14 << pin13;

	state = (struct scm_F652_state *)CTX_Get("F652", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

// pin1 CPAB
// pin2 SAB
// pin3 OEAB
// pin4…11 A
// pin23 CPBA
// pin22 SBA
// pin21 OEBA~
// pin20…13 B

#define CPAB pin1
#define SAB pin2
#define OEAB pin3
#define CPBA pin23
#define SBA pin22
#define OEBA_ pin21

void
SCM_F652 :: doit(void)
{
	state->ctx.activations++;
	unsigned a = 0;
	unsigned b = 0;
	char what[12];

	what[0] = '\0';
	if (IS_H(OEBA_)) {
		pin4 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin6 = sc_logic_Z;
		pin7 = sc_logic_Z;
		pin8 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin10 = sc_logic_Z;
		pin11 = sc_logic_Z;
		if (IS_H(pin4)) a |= (1 << 7);
		if (IS_H(pin5)) a |= (1 << 6);
		if (IS_H(pin6)) a |= (1 << 5);
		if (IS_H(pin7)) a |= (1 << 4);
		if (IS_H(pin8)) a |= (1 << 3);
		if (IS_H(pin9)) a |= (1 << 2);
		if (IS_H(pin10)) a |= (1 << 1);
		if (IS_H(pin11)) a |= (1 << 0);
		strcat(what, "Ai");
	}
	if (IS_L(OEAB)) {
		pin20 = sc_logic_Z;
		pin19 = sc_logic_Z;
		pin18 = sc_logic_Z;
		pin17 = sc_logic_Z;
		pin16 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin13 = sc_logic_Z;
		if (IS_H(pin20)) b |= (1 << 7);
		if (IS_H(pin19)) b |= (1 << 6);
		if (IS_H(pin18)) b |= (1 << 5);
		if (IS_H(pin17)) b |= (1 << 4);
		if (IS_H(pin16)) b |= (1 << 3);
		if (IS_H(pin15)) b |= (1 << 2);
		if (IS_H(pin14)) b |= (1 << 1);
		if (IS_H(pin13)) b |= (1 << 0);
		strcat(what, "Bi");
	}
	if (IS_H(OEAB)) {
		if (IS_L(SAB)) {
			b = a;
			strcat(what, "Ba");
		} else {
			b = state->areg;
			strcat(what, "Br");
		}
	}
	if (IS_L(OEBA_)) {
		if (IS_L(SBA)) {
			a = b;
			strcat(what, "Ab");
		} else {
			a = state->breg;
			strcat(what, "Ar");
		}
	}

	if (CPAB.posedge()) {
		state->areg = a;
		strcat(what, "A<");
	}
	if (CPBA.posedge()) {
		state->breg = b;
		strcat(what, "B<");
	}

	if (IS_L(OEBA_)) {
		// A is output
		pin4 = AS(a & (1 << 7));
		pin5 = AS(a & (1 << 6));
		pin6 = AS(a & (1 << 5));
		pin7 = AS(a & (1 << 4));
		pin8 = AS(a & (1 << 3));
		pin9 = AS(a & (1 << 2));
		pin10 = AS(a & (1 << 1));
		pin11 = AS(a & (1 << 0));
		strcat(what, "A>");
	}
	if (IS_H(OEAB)) {
		// B is output
		pin20 = AS(b & (1 << 7));
		pin19 = AS(b & (1 << 6));
		pin18 = AS(b & (1 << 5));
		pin17 = AS(b & (1 << 4));
		pin16 = AS(b & (1 << 3));
		pin15 = AS(b & (1 << 2));
		pin14 = AS(b & (1 << 1));
		pin13 = AS(b & (1 << 0));
		strcat(what, "B>");
	}
	TRACE(
	    << what << " a "
	    << pin4 << pin5 << pin6 << pin7 << pin8 << pin9 << pin10 << pin11
	    << " b "
	    << pin20 << pin19 << pin18 << pin17 << pin16 << pin15 << pin14 << pin13
	    << " | a "
	    << std::hex << a
	    << " ra "
	    << std::hex << state->areg
	    << " b "
	    << std::hex << b
	    << " rb "
	    << std::hex << state->breg
	);
}
