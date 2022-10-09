#include <stdlib.h>

#define MAX_MEMORY 0xffff

typedef struct Flags {
    u_int8_t z:1;
    u_int8_t s:1;
    u_int8_t p:1;
    u_int8_t cy:1;
    u_int8_t ac:1;
    u_int8_t pad:3;
} Flags;

typedef struct Chip8080 {
    u_int8_t reg_a;
    u_int8_t reg_b;
    u_int8_t reg_c;
    u_int8_t reg_d;
    u_int8_t reg_e;
    u_int8_t reg_h;
    u_int8_t reg_l;
    u_int16_t reg_sp;
    u_int16_t reg_pc;
    u_int8_t *memory;
    struct Flags flags;
    u_int8_t irq_enable;
} Chip8080;

Chip8080* make_chip8080();
void reset_chip_state(Chip8080*);
u_int8_t* _make_memory_bank();
void _clean_memory_bank(u_int8_t*);
u_int16_t make_register_pair_from(u_int8_t, u_int8_t);
u_int8_t get_register_pair_h(u_int16_t);
u_int8_t get_register_pair_l(u_int16_t);
int is_zero(u_int8_t);
int has_sign_(u_int8_t);
int has_parity(int, int);
int is_multiple_of_8(u_int8_t);
int has_ac(u_int8_t);
void destroy_chip8080(Chip8080*);
int run8080(Chip8080*);
void nop(Chip8080*); // 0x00
void lxi_b_d16(Chip8080*, unsigned char*); // 0x01
void stax_b(Chip8080*); // 0x02
void inx_b(Chip8080*); // 0x03
void inr_b(Chip8080*); // 0x04
void dcr_b(Chip8080*); // 0x05
void mvi_b_d8(Chip8080*, unsigned char*); // 0x06
void rlc(Chip8080*); // 0x07
void dad_b(Chip8080*); // 0x09
void ldax_b(Chip8080 *); // 0x0a
void dcx_b(Chip8080*); // 0x0b
void inr_c(Chip8080*); // 0x0c
void dcr_c(Chip8080*); // 0x0d
void mvi_c_d8(Chip8080*, unsigned char*); // 0x0e
void rrc(Chip8080*); // 0x0f
void lxi_d_d16(Chip8080*, unsigned char*); // 0x11
void stax_d(Chip8080*); // 0x12
void inx_d(Chip8080*); // 0x13
void inr_d(Chip8080*); // 0x14
void dcr_d(Chip8080*); // 0x15
void mvi_d_d8(Chip8080*, unsigned char*); // 0x16
void ral(Chip8080*); // 0x17
void dad_d(Chip8080*); // 0x19
void ldax_d(Chip8080*); // 0x1a
void dcx_d(Chip8080*); // 0x1b
void inr_e(Chip8080*); // 0x1c
void dcr_e(Chip8080*); // 0x1d
void mvi_e_d8(Chip8080*, unsigned char*); // 0x1e
void rar(Chip8080*); // 0x1f
void unimplementedInstruction(Chip8080*);
