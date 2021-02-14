
struct memdesc {
	const char		*name;
	uint32_t		lo;
	uint64_t		hi;
	uint32_t		mask;
	uint8_t			*rd_space;
	uint8_t			*wr_space;
	uint8_t			*pegs;
	size_t			space_length;
	size_t			pegs_length;
};

void mem_peg_check(const char *, struct memdesc *, unsigned, unsigned);

void mem_fail(const char *, unsigned, unsigned, unsigned);
extern const char *mem_error_cause;

typedef void mem_pre_read(int, uint8_t *, unsigned, unsigned);
typedef void mem_post_write(int, uint8_t *, unsigned, unsigned);

//void m68k_write_memory_8(unsigned, unsigned);
//unsigned m68k_read_memory_8(unsigned);
void m68k_debug_write_memory_8(unsigned, unsigned);
unsigned m68k_debug_read_memory_8(unsigned);
//void m68k_write_memory_16(unsigned, unsigned);
//unsigned m68k_read_memory_16(unsigned);
void m68k_debug_write_memory_16(unsigned, unsigned);
unsigned m68k_debug_read_memory_16(unsigned);
//void m68k_write_memory_32(unsigned, unsigned);
//unsigned m68k_read_memory_32(unsigned);
void m68k_debug_write_memory_32(unsigned, unsigned);
unsigned m68k_debug_read_memory_32(unsigned);

#include "_memcfg.h"
