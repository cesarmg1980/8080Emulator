#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "chip8080.h"
#include "tools.h"

int run8080(Chip8080 *chip) {
    unsigned char *program_data = &chip->memory[chip->reg_pc];

    switch(*program_data) {
        case 0x00: nop(chip); break;
        case 0x01: lxi_b_d16(chip, program_data); break;
        case 0x02: stax_b(chip); break;
        case 0x03: inx_b(chip); break;
        case 0x04: inr_b(chip); break;
        case 0x05: dcr_b(chip); break;
        case 0x06: mvi_b_d8(chip, program_data); break;
        case 0x07: unimplementedInstruction(chip); // rlc(chip)
        case 0x08: nop(chip); break;
        case 0x09: dad_b(chip); break;
        case 0x0a: ldax_b(chip); break;
        case 0x0b: dcx_b(chip); break;
        case 0x0c: inr_c(chip); break;
        case 0x0d: dcr_c(chip); break;
        case 0x0e: mvi_c_d8(chip, program_data); break;
        case 0x0f: unimplementedInstruction(chip); break; // rrc(chip)
        case 0x10: nop(chip); break;
        case 0x11: lxi_d_d16(chip, program_data); break;
        case 0x12: stax_d(chip); break;
        case 0x13: inx_d(chip); break;
        case 0x14: inr_d(chip); break;
        case 0x15: dcr_d(chip); break;
        case 0x16: mvi_d_d8(chip, program_data); break;
        case 0x17: unimplementedInstruction(chip); break; // ral(chip)
        case 0x18: nop(chip); break;
        case 0x19: dad_d(chip); break;
        case 0x1a: ldax_d(chip); break;
        case 0x1b: dcx_d(chip); break;
        case 0x1c: inr_e(chip); break;
        case 0x1d: dcr_e(chip); break;
        case 0x1f: unimplementedInstruction(chip); break; // rar(chip)
        case 0x20: nop(chip); break;
        case 0x21: lxi_h_d16(chip, program_data); break;
        case 0x22: shld_addr(chip, program_data); break;
        case 0x23: inx_h(chip); break; 
        case 0x24: inr_h(chip); break;
        case 0x25: dcr_h(chip); break;
        case 0x26: mvi_h_d8(chip, program_data); break;
        case 0x27: unimplementedInstruction(chip); break; // daa(chip)
        case 0x28: nop(chip); break;
        case 0x29: dad_h(chip); break;
        case 0x2a: lhld_adr(chip, program_data);
        case 0x2b: dcx_h(chip); break;
        case 0x2c: inr_l(chip); break;
        case 0x2d: dcr_l(chip); break;
        case 0x2e: mvi_l_d8(chip, program_data); break;
        case 0x2f: cma(chip); break;
    }
    return 0;
}

Chip8080* make_chip8080() {
    Chip8080 *chip8080 = malloc(sizeof(Chip8080));
    chip8080->memory = _make_memory_bank();
    reset_chip_state(chip8080);
    return chip8080;
}

void reset_chip_state(Chip8080 *chip) {
    chip->reg_a = 0;
    chip->reg_b = 0;
    chip->reg_c = 0;
    chip->reg_d = 0;
    chip->reg_e = 0;
    chip->reg_h = 0;
    chip->reg_l = 0;
    chip->reg_pc = 0;
    chip->reg_sp = 0;
    chip->flags.z = 0;
    chip->flags.s = 0;
    chip->flags.p = 0;
    chip->flags.cy = 0;
    chip->flags.ac = 0;
    chip->flags.pad = 0;
}

u_int8_t* _make_memory_bank() {
    u_int8_t *memory_ptr = (u_int8_t*) malloc(MAX_MEMORY);
    _clean_memory_bank(memory_ptr);
    return memory_ptr;
}

void _clean_memory_bank(u_int8_t *memory_ptr) {
    for(int i=0; i<MAX_MEMORY; i++)
        memory_ptr[i] = 0;
}

u_int16_t make_register_pair_from(u_int8_t reg_x, u_int8_t reg_y) {
    return (reg_x << 8) | reg_y;
}

u_int8_t get_register_pair_h(u_int16_t reg_pair) {
    return (reg_pair & 0xff00) >> 8;
}

u_int8_t get_register_pair_l(u_int16_t reg_pair) {
    return reg_pair & 0xff;
}

int is_zero(u_int8_t reg) {
    return reg == 0;
}

int has_sign(u_int8_t reg) {
    return (reg & 0x80) != 0;
}

int has_parity(int x, int size) {
	int i;
	int p = 0;
	x = (x & ((1<<size)-1));
	for (i=0; i<size; i++) {
		if (x & 0x1) p++;
		x = x >> 1;
	}
	return (0 == (p & 0x1));
}

int is_multiple_of_16(u_int8_t reg) {
    return reg % 16 == 0;
}

int has_ac(u_int8_t reg) {
    return !is_zero(reg) && is_multiple_of_16(reg);
}

void destroy_chip8080(Chip8080 *chip) {
    free(chip->memory);
    free(chip);
}


/*
 *  Instrucions
 *
 */

void unimplementedInstruction(Chip8080 *chip) {
    printf("Error: Unimplemented Instruction!\n");
    chip->reg_pc--;
    disassemble_machine_code(chip->memory, chip->reg_pc);
    printf("\n");
    exit(1);
}

void nop(Chip8080 *chip) {
    /* [0x00] NOP; No operation,
     * PC += 1,
     * Instrucion Size: 1 BYTE
     */
    chip->reg_pc++;
}

void lxi_b_d16(Chip8080 *chip, unsigned char *program_data) {
    /* [0x01] LXI B,D16; B <- byte 3, C <- byte 2;
     * Flags: None,
     * Instruction Size: 3 BYTES
     */
    chip->reg_b = program_data[2];
    chip->reg_c = program_data[1];
    chip->reg_pc += 3;
}

void stax_b(Chip8080 *chip) {
    /* [0x02] STAX B; (BC) <- A;
     * Flags: None,
     * Instruction Size: 1 BYTE
     */
    u_int16_t reg_bc = make_register_pair_from(chip->reg_b, chip->reg_c);
    chip->memory[reg_bc] = chip->reg_a;
    chip->reg_pc++;
}

void inx_b(Chip8080 *chip) {
    /* [0x03] INX B; BC <- BC + 1;
     * Flags: None,
     * Instruction Size: 1 BYTE
     */
    u_int16_t reg_bc = make_register_pair_from(chip->reg_b, chip->reg_c);
    reg_bc++;
    chip->reg_b = get_register_pair_h(reg_bc);
    chip->reg_c = get_register_pair_l(reg_bc);
    chip->reg_pc++;
}

void inr_b(Chip8080 *chip) {
    /* [0x04] INR B; B <- B+1
     * Flags: Z, S, P, AC
     * BYTES: 1
    */
    chip->reg_b++;
    chip->flags.z = is_zero(chip->reg_b);
    chip->flags.s = has_sign(chip->reg_b);
    chip->flags.p = has_parity(chip->reg_b, 8);
    chip->flags.ac = has_ac(chip->reg_b);
    chip->reg_pc++;
}

void dcr_b(Chip8080 *chip) {
    /* [0x05] DCR B; B <- B-1; 1 BYTE
     * Flags: Z, S, P, AC
     * BYTES: 1
     */
    chip->reg_b--;
    chip->flags.z = is_zero(chip->reg_b);
    chip->flags.s = has_sign(chip->reg_b);
    chip->flags.p = has_parity(chip->reg_b, 8);
    chip->flags.ac = has_ac(chip->reg_b);
    chip->reg_pc++;
}

void mvi_b_d8(Chip8080 *chip, unsigned char *program_data) {
    /* [0x06] MVI B,D8; B <- byte 2
     * Flags: None
     * BYTES 2 */
    chip->reg_b = program_data[1];
    chip->reg_pc += 2;
}

void rlc(Chip8080 *chip) {
    /* [0x07] A = A << 1; bit 0 = prev bit 7; CY = prev bit 7
     * Flags: CY
     * BYTES: 1
     *
     * TODO: Implement
     */
}

void dad_b(Chip8080 *chip) {
    /* [0x09] DAD B; HL = HL+BC
     * Flags: CY
     * BYTES = 1
     */
    u_int32_t hl = make_register_pair_from(chip->reg_h, chip->reg_l);
    u_int32_t bc = make_register_pair_from(chip->reg_b, chip->reg_c);
    u_int32_t res = hl + bc;
    chip->reg_h = get_register_pair_h(res);
    chip->reg_l = get_register_pair_l(res);
    chip->flags.cy = ((res & 0xffff0000) > 0);
    chip->reg_pc++;
}

void ldax_b(Chip8080 *chip) {
    /* [0x0a] A <- (BC)
     * Flags: None
     * BYTES: 1
     */
    u_int16_t reg_bc = make_register_pair_from(chip->reg_b, chip->reg_c);
    chip->reg_a = chip->memory[reg_bc];
    chip->reg_pc++;
}

void dcx_b(Chip8080 *chip) {
    /* [0x0b] BC = BC - 1
     * Flags: None
     * BYTES: 1
     */
    u_int16_t reg_bc = make_register_pair_from(chip->reg_b, chip->reg_c);
    reg_bc--;
    chip->reg_b = get_register_pair_h(reg_bc);
    chip->reg_c = get_register_pair_l(reg_bc);
    chip->reg_pc++;
}

void inr_c(Chip8080 *chip) {
    /* [0x0c] C = C + 1
     * Flags: Z, S, P, AC
     * BYTES: 1
     */
    chip->reg_c++;
    chip->flags.z = is_zero(chip->reg_c);
    chip->flags.s = has_sign(chip->reg_c);
    chip->flags.p = has_parity(chip->reg_c, 8);
    chip->flags.ac = has_ac(chip->reg_c);
    chip->reg_pc++;
}

void dcr_c(Chip8080 *chip) {
    /* [0x0d] C = C - 1
     * Flags: Z, S, P, AC
     * Bytes: 1
     */
    chip->reg_c--;
    chip->flags.z = is_zero(chip->reg_c);
    chip->flags.s = has_sign(chip->reg_c);
    chip->flags.p = has_parity(chip->reg_c, 8);
    chip->flags.ac = has_ac(chip->reg_c);
    chip->reg_pc++;
}

void mvi_c_d8(Chip8080 *chip, unsigned char *program_data) {
    /* [0x0e] C = Byte 2
     * Flags: None
     * Bytes: 2
     */
    chip->reg_c = program_data[1];
    chip->reg_pc += 2;
}

void rrc(Chip8080 *chip) {
    /* [0x0f] A = A>>1; bit 7 = prev bit 0; CY = prev bit 0
     * Flags: CY
     * Instruction Size: 1 Byte
     */
     // TODO: Implement
}

void lxi_d_d16(Chip8080 *chip, unsigned char *program_data) {
    /* [0x11] LXI D,D16; D <- byte 3, E <- byte 2;
     * Flags: None,
     * Instruction Size: 3 BYTES
     */
    chip->reg_d = program_data[2];
    chip->reg_e = program_data[1];
    chip->reg_pc += 3;
}

void stax_d(Chip8080 *chip) {
    /* [0x12] STAX D; (DE) <- A;
     * Flags: None,
     * Instruction Size: 1 BYTE
     */
    u_int16_t reg_de = make_register_pair_from(chip->reg_d, chip->reg_e);
    chip->memory[reg_de] = chip->reg_a;
    chip->reg_pc++;
}

void inx_d(Chip8080 *chip) {
    /* [0x13] INX DE; (DE) <- DE+1
     * Flags: None,
     * Instruction Size: 1 Byte
     */
    u_int16_t reg_de = make_register_pair_from(chip->reg_d, chip->reg_e);
    reg_de++;
    chip->reg_d = get_register_pair_h(reg_de);
    chip->reg_e = get_register_pair_l(reg_de);
    chip->reg_pc++;
}

void inr_d(Chip8080 *chip) {
    /* [0x14] INR D; D <- D+1
     * Flags: Z, S, P, AC
     * Instruction Size: 1 Byte
     */
    chip->reg_d++;
    chip->flags.z = is_zero(chip->reg_d);
    chip->flags.s = has_sign(chip->reg_d);
    chip->flags.p = has_parity(chip->reg_d, 8);
    chip->flags.ac = has_ac(chip->reg_d);
    chip->reg_pc++;
}

void dcr_d(Chip8080 *chip) {
    /* [0x15] DCR D; D <- D-1
     * Flags: Z, S, P, AC
     * Instruction Size: 1 Byte
     */
    chip->reg_d--;
    chip->flags.z = is_zero(chip->reg_d);
    chip->flags.s = has_sign(chip->reg_d);
    chip->flags.p = has_parity(chip->reg_d, 8);
    chip->flags.ac = has_ac(chip->reg_d);
    chip->reg_pc++;
}

void mvi_d_d8(Chip8080 *chip, unsigned char *program_data) {
    /* [0x16] MVI D, D8; D <- Byte 2
     * Flags: None
     * Instruction Size: 2 Bytes
     */
    chip->reg_d = program_data[1];
    chip->reg_pc += 2;
}

void ral(Chip8080 *chip) {
    /* [0x17] RAL; A = A << 1; bit 0 = prev CY; CY = prev bit 7
     * Flags: CY
     * Instruction Size: 1 Byte
     */

    //TODO: Implement
}

void dad_d(Chip8080 *chip) {
    /* [0x19] DAD D; HL = HL+DE
     * Flags: CY
     * BYTES = 1
     */
    u_int32_t hl = make_register_pair_from(chip->reg_h, chip->reg_l);
    u_int32_t de = make_register_pair_from(chip->reg_d, chip->reg_e);
    u_int32_t res = hl + de;
    chip->reg_h = get_register_pair_h(res);
    chip->reg_l = get_register_pair_l(res);
    chip->flags.cy = ((res & 0xffff0000) > 0);
    chip->reg_pc++;
}

void ldax_d(Chip8080 *chip) {
    /* [0x1a] LDAX D; A <- (DE)
     * Flags: None
     * Instruction Size: 1 Byte
     */
    u_int16_t reg_de = make_register_pair_from(chip->reg_d, chip->reg_e);
    chip->reg_a = chip->memory[reg_de];
    chip->reg_pc++;
}

void dcx_d(Chip8080 * chip) {
    /* [0x0b] DE = DE - 1
     * Flags: None
     * BYTES: 1
     */
    u_int16_t reg_de = make_register_pair_from(chip->reg_d, chip->reg_e);
    reg_de--;
    chip->reg_d = get_register_pair_h(reg_de);
    chip->reg_e = get_register_pair_l(reg_de);
    chip->reg_pc++;
}

void inr_e(Chip8080 *chip) {
    /* [0x1c] INR E; E <- E+1
     * Flags: Z, S, P, AC
     * BYTES: 1
    */

    chip->reg_e++;
    chip->flags.z = is_zero(chip->reg_e);
    chip->flags.s = has_sign(chip->reg_e);
    chip->flags.p = has_parity(chip->reg_e, 8);
    chip->flags.ac = has_ac(chip->reg_e);
    chip->reg_pc++;
}

void dcr_e(Chip8080 *chip) {
    /* [0x1d] DCR E; E <- E-1
     * Flags: Z, S, P, AC
     * BYTES: 1
     */
    chip->reg_e--;
    chip->flags.z = is_zero(chip->reg_e);
    chip->flags.s = has_sign(chip->reg_e);
    chip->flags.p = has_parity(chip->reg_e, 8);
    chip->flags.ac = has_ac(chip->reg_e);
    chip->reg_pc++;
}

void mvi_e_d8(Chip8080 *chip, unsigned char *program_data) {
    /* [0x1e] MVI E,D8; E <- byte 2
     * Flags: None
     * BYTES 2 */
    chip->reg_e = program_data[1];
    chip->reg_pc += 2;
}

void rar(Chip8080 *chip) {
    /* [0x1f] RAR; A = A >> 1; bit 7 = prev bit 7; CY = prev bit 0
     * Flags: CY
     * Instruction Size: 1 Byte
     */

    //TODO: Implement
}

void lxi_h_d16(Chip8080 *chip, unsigned char *program_data) {
    /* [0x01] LXI H,D16; h <- byte 3, L <- byte 2;
     * Flags: None,
     * Instruction Size: 3 BYTES
     */
    chip->reg_h = program_data[2];
    chip->reg_l = program_data[1];
    chip->reg_pc += 3;
}

void shld_addr(Chip8080 *chip, unsigned char *program_data) {
    /* [0x22] SHLD addr; (addr) <- L; (addr + 1) <- H
     * Flags: None
     * Instruction Size: 3 Bytes
     */
    chip->memory[make_register_pair_from(program_data[2], program_data[1])] = chip->reg_l;
    chip->memory[make_register_pair_from(program_data[2], program_data[1])+1] = chip->reg_h;
    chip->reg_pc += 3;
}

void inx_h(Chip8080 *chip) {
    /* [0x23] INX H; HL <- HL + 1
     * Flags: None
     * Instruction Size: 1 Byte
     */
    u_int16_t reg_hl = make_register_pair_from(chip->reg_h, chip->reg_l);
    reg_hl++;
    chip->reg_h = get_register_pair_h(reg_hl);
    chip->reg_l = get_register_pair_l(reg_hl);
    chip->reg_pc++;
}

void inr_h(Chip8080 *chip) {
    /* [0x24] INR H: H <- H+1
     * Flags: Z, S, P, AC
     * Instruction Size: 1 Byte
     */
    chip->reg_h++;
    chip->flags.z = is_zero(chip->reg_h);
    chip->flags.s = has_sign(chip->reg_h);
    chip->flags.p = has_parity(chip->reg_h, 8);
    chip->flags.ac = has_ac(chip->reg_h);
    chip->reg_pc++;
}

void dcr_h(Chip8080 *chip) {
    /* [0x25] DCR H: H <- H-1  
     * Flags: Z, S, P, AC 
     * Instruction Size: 1 Byte
     */
    chip->reg_h--;
    chip->flags.z = is_zero(chip->reg_h);
    chip->flags.s = has_sign(chip->reg_h);
    chip->flags.p = has_parity(chip->reg_h, 8);
    chip->flags.ac = has_ac(chip->reg_h);
    chip->reg_pc++;
}

void mvi_h_d8(Chip8080 *chip, unsigned char *program_data) {
    /* [0x26] MVI H,D8; B <- byte 2
     * Flags: None
     * BYTES 2 */
    chip->reg_h = program_data[1];
    chip->reg_pc += 2;
}

void daa(Chip8080 *chip) {
    /* [0x27] 1 Byte */
    // TODO: Implement
}

void dad_h(Chip8080 *chip) {
    /* [0x29] DAD H; HL = HL + HL
     * Flags: CY
     * BYTES = 1
     */
    u_int32_t hl = make_register_pair_from(chip->reg_h, chip->reg_l);
    u_int32_t res = 2 * hl;
    chip->reg_h = get_register_pair_h(res);
    chip->reg_l = get_register_pair_l(res);
    chip->flags.cy = ((res & 0xffff0000) > 0);
    chip->reg_pc++;
}

void lhld_adr(Chip8080 *chip, unsigned char *program_data) {
    /* [0x2a] LHLD addr; L <- (addr), H <- (addr+1)
     * Flags: None
     * Bytes: 3
     */
    chip->reg_l = chip->memory[make_register_pair_from(program_data[2], program_data[1])];
    chip->reg_h = chip->memory[make_register_pair_from(program_data[2], program_data[1]) + 1];
    chip->reg_pc += 3;
}

void dcx_h(Chip8080 *chip) {
    /* [0x2b] DCX H: HL <- HL - 1
     * Flags: None
     * Bytes: 1*/
    u_int16_t hl = make_register_pair_from(chip->reg_h, chip->reg_l);
    hl--;
    chip->reg_h = get_register_pair_h(hl);
    chip->reg_l = get_register_pair_l(hl);
    chip->reg_pc++;
}

void inr_l(Chip8080 *chip) {
    /* [0x2c] INR L: L <- L + 1
     * Flags: Z, S, P, AC
     * Bytes: 1
     */
    chip->reg_l++;
    chip->flags.z = is_zero(chip->reg_l);
    chip->flags.s =  has_sign(chip->reg_l);
    chip->flags.p = has_parity(chip->reg_l, 8);
    chip->flags.ac = has_ac(chip->reg_l);
    chip->reg_pc++;
}

void dcr_l(Chip8080 *chip) {
    /* [0x2d] DCR L: L <- L-1
     * Flags: Z, S, P, AC
     * Bytes: 1
     */
    chip->reg_l--;
    chip->flags.z = is_zero(chip->reg_l);
    chip->flags.s = has_sign(chip->reg_l);
    chip->flags.p = has_parity(chip->reg_l, 8);
    chip->flags.ac = has_ac(chip->reg_l);
    chip->reg_pc++;
}

void mvi_l_d8(Chip8080 *chip, unsigned char *program_data) {
    /* [0x2e] MVI L, D8: L <- Byte 2 of program_data 
     * Flags: None 
     * Byte: 2
     */
    chip->reg_l = program_data[1];
    chip->reg_pc += 2;
}

void cma(Chip8080 *chip) {
    /* [0x2f] CMA: A <- !A,
     * Flags: None,
     * Bytes: 1
     */
    chip->reg_a = ~chip->reg_a; 
    chip->reg_pc++;
}
