#include <stdio.h>
#include <stdlib.h>
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
        case 0x07: rlc(chip); break;
        case 0x08: nop(chip); break;
        case 0x09: dad_b(chip); break;
        case 0x0a: ldax_b(chip); break;
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

int is_multiple_of_8(u_int8_t reg) {
    return reg % 8 == 0;
}

// TODO: This implementation of AC is incorrect
int has_ac(u_int8_t reg) {
    return !is_zero(reg) && is_multiple_of_8(reg);
}

void destroy_chip8080(Chip8080 *chip) {
    free(chip->memory);
    free(chip);
}

/*  Instrucions */ 


void nop(Chip8080 *chip) {
    /* [0x00] NOP; No operation, advances the PC by 1 */
    chip->reg_pc++;
}

void lxi_b_d16(Chip8080 *chip, unsigned char *program_data) {
    /* [0x01] LXI B,D16; B <- byte 3, C <- byte 2; 3 BYTES - */
    chip->reg_b = program_data[2]; 
    chip->reg_c = program_data[1]; 
    chip->reg_pc += 3; 
}

void stax_b(Chip8080 *chip) {
    /* [0x02] STAX B; (BC) <- A;  1 BYTE */
    u_int16_t reg_bc = make_register_pair_from(chip->reg_b, chip->reg_c);
    chip->memory[reg_bc] = chip->reg_a;
    chip->reg_pc++;
}

void inx_b(Chip8080 *chip) {
    /* [0x03] INX B; BC <- BC + 1; 1 BYTE*/
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
    // TODO: implement AC Flag
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
    // TODO: Implement AC flag
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

}

void unimplementedInstruction(Chip8080 *chip) {
    printf("Error: Unimplemented Instruction!\n");
    chip->reg_pc--;
    disassemble_machine_code(chip->memory, chip->reg_pc);
    printf("\n");
    exit(1);
}
