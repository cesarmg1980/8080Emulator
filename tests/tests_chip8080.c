#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include "../src/chip8080.h"

static void test_lxi_b_d16(void **state) {
    /* Test that:
     * LXI B,D16; B <- byte 3, C <- byte 2;
     * Instruction Size: 3 BYTES  */

    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0;
    chip->reg_c = 0;
    chip->reg_pc = 0;

    size_t instruction_size = 3 * sizeof(u_int8_t);
    u_int8_t *program_data = (u_int8_t *) malloc(instruction_size);
    program_data[0] = 0x01;
    program_data[1] = 0x02;
    program_data[2] = 0x03;


    lxi_b_d16(chip, program_data);

    assert_int_equal(0x03, chip->reg_b);
    assert_int_equal(0x02, chip->reg_c);
    assert_int_equal(3, chip->reg_pc);

    free(program_data);
    destroy_chip8080(chip);
}

static void test_stax_b(void **state) {
    /* Test That
     * STAX B; (BC) <- A;
     * Instruction Size: 1 BYTE */

    Chip8080 *chip = make_chip8080();
    chip->reg_a = 0x0a;
    chip->reg_b = 0x00;
    chip->reg_c = 0x01;
    chip->reg_pc = 0;

    stax_b(chip);

    assert_int_equal(0x0a, chip->memory[0x0001]);
    assert_int_equal(1, chip->reg_pc);

    // We check that only the specified memory offset has the data,
    // and the adjacents places are empty (0)
    assert_int_equal(0x00, chip->memory[0x0001 - 1]);
    assert_int_equal(0x00, chip->memory[0x0001 + 1]);

    destroy_chip8080(chip);
}

static void test_inx_b(void **state) {
    /* Test that:
     * INX B: BC <- BC + 1;
     * Instruction Size: 1 BYTE
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0x00;
    chip->reg_c = 0xff;
    chip->reg_pc = 0;

    inx_b(chip);

    assert_int_equal(0x01, chip->reg_b);
    assert_int_equal(0x00, chip->reg_c);
    assert_int_equal(1, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_inr_b(void **state) {
    /* Test that:
     * INR B: B <- B+1;
     * Flags: Z, S, P, AC;
     * Instruction Size: 1 BYTE
     */

    /* Scenario A: reg_b = -1
     * Expected Result: reg_b = 0
     * Flags: Z=1, S=0, P=1, AC=0
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0xff;
    chip->reg_pc = 0x00ff;

    inr_b(chip);

    assert_int_equal(0x00, chip->reg_b);
    assert_int_equal(0x01, chip->flags.z);
    assert_int_equal(0x00, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);


     /* Scenario B Edge Case: reg_b = -17
     * Expected Result: reg_b = -16
     * Flags: Z=0, S=1, P=1, AC=1
     */

    Chip8080 *chip_2 = make_chip8080();
    chip_2->reg_b = 0xef;
    chip_2->reg_pc = 0x00ff;

    inr_b(chip_2);

    assert_int_equal(0xf0, chip_2->reg_b);
    assert_int_equal(0x00, chip_2->flags.z);
    assert_int_equal(0x01, chip_2->flags.s);
    assert_int_equal(0x01, chip_2->flags.p);
    assert_int_equal(0x01, chip_2->flags.ac);
    assert_int_equal(0x0100, chip_2->reg_pc);

    destroy_chip8080(chip_2);
}

static void test_dcr_b(void **state) {
    /* Test that:
     * DCR B: B <- B - 1;
     * Flags: Z, S, P, AC
     * Instruction Size: 1 BYTE
     *
     * Scenario: reg_b = 1
     * Expected Result: reg_b = 0
     * Flags: Z=1, S=0, P=1, AC=0
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0x01;
    chip->reg_pc = 0x00ff;

    dcr_b(chip);

    assert_int_equal(0x00, chip->reg_b);
    assert_int_equal(0x1, chip->flags.z);
    assert_int_equal(0x0, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);

    //TODO: Implemente Edge Cases
}

static void test_mvi_b_d8(void **state) {
    /* MVI B, data 8 bits: B <- Byte 2
     * Flags: None
     * Instruction Size: 2 Bytes
     *
     * Scenario: Byte 2 = 3
     * Expected Result: reg_b = 3
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0x0;
    chip->reg_pc = 0x00ff;

    size_t instruction_size = 2 * sizeof(u_int8_t);
    u_int8_t *program_data = (u_int8_t*) malloc(instruction_size);
    program_data[0] = 0x06; // The Instruction Opcode
    program_data[1] = 0x03;

    mvi_b_d8(chip, program_data);

    assert_int_equal(0x03, chip->reg_b);
    assert_int_equal(0x0101, chip->reg_pc);

    free(program_data);
    destroy_chip8080(chip);
}

static void test_rlc(void **state) {
    // TODO: Implement Test
}

static void test_dad_b(void **state) {
    /* Test that:
     * DAD B; HL = HL + BC
     */
    Chip8080 *chip = make_chip8080();
    chip->reg_h = 0x01;
    chip->reg_l = 0xff;
    chip->reg_b = 0x00;
    chip->reg_c = 0xff;
    chip->reg_pc = 0x0100;

    dad_b(chip);

    assert_int_equal(0x02, chip->reg_h);
    assert_int_equal(0xfe, chip->reg_l);
    assert_int_equal(0x00, chip->flags.cy);
    assert_int_equal(0x0101, chip->reg_pc);

    destroy_chip8080(chip);
}


static void test_ldax_b(void **state) {
    /* LDAX B: A <- (BC)
     * Flags: None
     * Instruction Size: 1 BYTE
     *
     * Scenario Address[BC] = 0x0a
     * Expected Result: reg_a = 0xa
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0x00;
    chip->reg_c = 0x0a;
    chip->memory[0x000a] = 0x0f;
    chip->reg_pc = 0x00ff;

    ldax_b(chip);

    assert_int_equal(0x0f, chip->reg_a);
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_dcx_b(void **state) {
    /* DCX B: BC = BC - 1
     */
    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0x01;
    chip->reg_c = 0x00;
    chip->reg_pc = 0x00ff;

    dcx_b(chip);

    assert_int_equal(0x00ff, make_register_pair_from(chip->reg_b, chip->reg_c));
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_inr_c(void **state) {
    /* INR C: C = C + 1
     * Flags: Z, S, P, AC
     * Instruction Size: 1 BYTE
     *
     * Scenario C = 0xff
     * Expected Result: 0x00
     * Flags: Z=1, S=0, P=1, AC=0
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_c = 0xff;
    chip->reg_pc = 0x00ff;

    inr_c(chip);

    assert_int_equal(0x00, chip->reg_c);
    assert_int_equal(0x01, chip->flags.z);
    assert_int_equal(0x00, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);

}

static void test_dcr_c(void **state) {
    /* INR C: C = C - 1
     * Flags: Z, S, P, AC
     * Instruction Size: 1 BYTE
     *
     * Scenario C = 0x00
     * Expected Result: 0xff
     * Flags: Z=0, S=1, P=1, AC=0
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_c = 0x00;
    chip->reg_pc = 0x00ff;

    dcr_c(chip);

    assert_int_equal(0xff, chip->reg_c);
    assert_int_equal(0x00, chip->flags.z);
    assert_int_equal(0x01, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_mvi_c_d8(void **state) {
    /* MVI C, data 8 bits: B <- Byte 2
     * Flags: None
     * Instruction Size: 2 Bytes
     *
     * Scenario: Byte 2 = 5
     * Expected Result: reg_c = 5
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_c = 0x0;
    chip->reg_pc = 0x00ff;

    size_t instruction_size = 2 * sizeof(u_int8_t);
    u_int8_t *program_data = (u_int8_t*) malloc(instruction_size);

    program_data[0] = 0x0e; // The Instruction Opcode
    program_data[1] = 0x05;

    mvi_c_d8(chip, program_data);

    assert_int_equal(0x05, chip->reg_c);
    assert_int_equal(0x0101, chip->reg_pc);

    free(program_data);
    destroy_chip8080(chip);
}

static void tests_rrc(void **state) {
    /* Unimplemented  */ 
    // TODO: Implement
}

static void test_lxi_d_d16(void **state) {
    /* Test that: 
     * LXI D,D16; D <- byte 3, E <- byte 2;
     * Instruction Size: 3 BYTES  */

    Chip8080 *chip = make_chip8080();
    chip->reg_d = 0;
    chip->reg_e = 0;
    chip->reg_pc = 0;

    size_t instruction_size = 3 * sizeof(u_int8_t);
    u_int8_t *program_data = (u_int8_t *) malloc(instruction_size);
    program_data[0] = 0x01; // The Instruction's Opcode
    program_data[1] = 0x02;
    program_data[2] = 0x03;


    lxi_d_d16(chip, program_data);

    assert_int_equal(0x03, chip->reg_d);
    assert_int_equal(0x02, chip->reg_e);
    assert_int_equal(3, chip->reg_pc);

    free(program_data);
    destroy_chip8080(chip);
}

static void test_stax_d(void **state) {
    /* Test That
     * STAX D; (DE) <- A;
     * Instruction Size: 1 BYTE */

    Chip8080 *chip = make_chip8080();
    chip->reg_a = 0x0a;
    chip->reg_d = 0x00;
    chip->reg_e = 0x01;
    chip->reg_pc = 0x00ff;

    stax_d(chip);

    assert_int_equal(0x0a, chip->memory[0x0001]);
    assert_int_equal(0x0100, chip->reg_pc);

    // We check that only the specified memory offset has the data,
    // and the adjacents places are empty (0)
    assert_int_equal(0x00, chip->memory[0x0001 - 1]);
    assert_int_equal(0x00, chip->memory[0x0001 + 1]);

    destroy_chip8080(chip);
}

static void test_inx_d(void **state) {
    /* Tests that:
     * INX DE; DE <- DE+1
     */

    Chip8080 * chip = make_chip8080();
    chip->reg_d = 0x00;
    chip->reg_e = 0xff;
    chip->reg_pc = 0x0000;

    inx_d(chip);

    assert_int_equal(0x01, chip->reg_d);
    assert_int_equal(0x00, chip->reg_e);
    assert_int_equal(0x0001, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_inr_d(void **state) {
    /* Tests that:
     * INR D; D <- D+1
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_d = 0xff;
    chip->reg_pc = 0x00ff;

    inr_d(chip);

    assert_int_equal(0x00, chip->reg_d);
    assert_int_equal(0x01, chip->flags.z);
    assert_int_equal(0x00, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_dcr_d(void **state) {
    /* Tests that:
     * DCR D; D <- D - 1
     */
    Chip8080 *chip = make_chip8080();
    chip->reg_d = 0x01;
    chip->reg_pc = 0x00fe;

    dcr_d(chip);

    assert_int_equal(0x00, chip->reg_d);
    assert_int_equal(0x01, chip->flags.z);
    assert_int_equal(0x00, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);
    assert_int_equal(0x00ff, chip->reg_pc);

    destroy_chip8080(chip);
}

static void tests_mvi_d_d8(void **state) {
    /* Tests that:
     * MVI D, D8; D <- D8
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_d = 0x0;
    chip->reg_pc = 0x00ff;

    size_t instruction_size = 2 * sizeof(u_int8_t);
    u_int8_t *program_data = (u_int8_t*) malloc(instruction_size);
    program_data[0] = 0x16; // The Instruction Opcode
    program_data[1] = 0x03;

    mvi_d_d8(chip, program_data);

    assert_int_equal(0x03, chip->reg_d);
    assert_int_equal(0x0101, chip->reg_pc);

    free(program_data);

    destroy_chip8080(chip);
}

static void test_ral(void **state) {
    /* Unimplemented
     */

    // TODO: Implement
}

static void test_dad_d(void **state) {
    /* Test that:
     * DAD D; HL = HL + DE
     */
    Chip8080 *chip = make_chip8080();
    chip->reg_h = 0x01;
    chip->reg_l = 0xff;
    chip->reg_d = 0x00;
    chip->reg_e = 0xff;
    chip->reg_pc = 0x00ff;

    dad_d(chip);

    assert_int_equal(0x02, chip->reg_h);
    assert_int_equal(0xfe, chip->reg_l);
    assert_int_equal(0x00, chip->flags.cy);
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_ldax_d(void **state) {
    /* Tests that:
     * LDAX B: A <- (DE)
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_d = 0x00;
    chip->reg_e = 0x0a;
    chip->memory[0x000a] = 0x0f;
    chip->reg_pc = 0x00ff;

    ldax_d(chip);

    assert_int_equal(0x0f, chip->reg_a);
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_dcx_d(void **state) {
    /* DCX D: DE = DE - 1
     */
    Chip8080 *chip = make_chip8080();
    chip->reg_d = 0x01;
    chip->reg_e = 0x00;
    chip->reg_pc = 0x00ff;

    dcx_d(chip);

    assert_int_equal(0x00ff, make_register_pair_from(chip->reg_d, chip->reg_e));
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_inr_e(void **state) {
    /* Test that:
     * INR B: B <- B+1;
     * Flags: Z, S, P, AC;
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_e = 0xff;
    chip->reg_pc = 0x00ff;

    inr_e(chip);

    assert_int_equal(0x00, chip->reg_e);
    assert_int_equal(0x01, chip->flags.z);
    assert_int_equal(0x00, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_dcr_e(void **state) {
    /* Test that:
     * DCR B: B <- B - 1;
     * Flags: Z, S, P, AC
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_e = 0x01;
    chip->reg_pc = 0x00ff;

    dcr_e(chip);

    assert_int_equal(0x00, chip->reg_e);
    assert_int_equal(0x1, chip->flags.z);
    assert_int_equal(0x0, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);
    assert_int_equal(0x0100, chip->reg_pc);

    destroy_chip8080(chip);

}

static void test_mvi_e_d8(void **state) {
    /* MVI E, data 8 bits: B <- Byte 2
     * Flags: None
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_e = 0x0;
    chip->reg_pc = 0x00ff;

    size_t instruction_size = 2 * sizeof(u_int8_t);
    u_int8_t *program_data = (u_int8_t*) malloc(instruction_size);
    program_data[0] = 0x06; // The Instruction Opcode
    program_data[1] = 0x03;

    mvi_e_d8(chip, program_data);

    assert_int_equal(0x03, chip->reg_e);
    assert_int_equal(0x0101, chip->reg_pc);

    free(program_data);
    destroy_chip8080(chip);
}

static void test_rar(void **state) {
    /* Unimplemented
     */

    // TODO: Implement
}

static void test_lxi_h_16(void **state) {
    /* Test that:
     * LXI H,D16; H <- byte 3, L <- byte 2;
     * Instruction Size: 3 BYTES  */

    Chip8080 *chip = make_chip8080();
    chip->reg_h = 0;
    chip->reg_l = 0;
    chip->reg_pc = 0;

    size_t instruction_size = 3 * sizeof(u_int8_t);
    u_int8_t *program_data = (u_int8_t *) malloc(instruction_size);
    program_data[0] = 0x21;
    program_data[1] = 0x02;
    program_data[2] = 0x03;


    lxi_h_d16(chip, program_data);

    assert_int_equal(0x03, chip->reg_h);
    assert_int_equal(0x02, chip->reg_l);
    assert_int_equal(3, chip->reg_pc);

    free(program_data);
    destroy_chip8080(chip);
}

static void test_shld_addr(void **state) {
    /* Test that:
     * SHDL addr; (addr) <- L; (addr+1) <- H
     * Instruction Size: 3 BYTES  */

    Chip8080 *chip = make_chip8080();
    chip->reg_l = 0x0a;
    chip->reg_h = 0x10;
    chip->reg_pc = 0;

    size_t instruction_size = 3 * sizeof(u_int8_t);
    u_int8_t *program_data = (u_int8_t *) malloc(instruction_size);
    program_data[0] = 0x22;
    program_data[1] = 0x0a;
    program_data[2] = 0x01;

    shld_addr(chip, program_data);

    assert_int_equal(0x0a, chip->memory[0x010a]);
    assert_int_equal(0x10, chip->memory[0x010a+1]);
    assert_int_equal(3, chip->reg_pc);

    free(program_data);
    destroy_chip8080(chip);
}

static void test_inx_h(void **state) {
    /* Tests that:
     * INX H: HL <- HL+1
     */
    Chip8080 *chip = make_chip8080();
    chip->reg_h = 0x00;
    chip->reg_l = 0xff;

    inx_h(chip);

    assert_int_equal(0x01, chip->reg_h);
    assert_int_equal(0x00, chip->reg_l);
    assert_int_equal(1, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_inr_h(void **state) {
    /* Tests that:
     * INX H: H <- H+1
     * and Flags: Z, S, P, AC are set appropriately
     */
    Chip8080 *chip = make_chip8080();
    chip->reg_h = 0x0a;
    chip->reg_pc = 0x001a;

    inr_h(chip);

    assert_int_equal(0x0b, chip->reg_h);
    assert_int_equal(0x00, chip->flags.z);
    assert_int_equal(0x00, chip->flags.s);
    assert_int_equal(0x00, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);
    assert_int_equal(0x001b, chip->reg_pc);

    destroy_chip8080(chip);
}

static void test_dcr_h(void **state) {
    /* Test that:
     * DCR H: H <- H-1 
     * and Flags: Z, S, P, AC are set/unset appropriately 
     */
    Chip8080 *chip = make_chip8080();
    chip->reg_h = 0x01;
    chip->reg_pc = 0x10ff;

    dcr_h(chip);

    assert_int_equal(0x00, chip->reg_h);
    assert_int_equal(0x01, chip->flags.z);
    assert_int_equal(0x00, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);
    assert_int_equal(0x1100, chip->reg_pc);

    destroy_chip8080(chip);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_lxi_b_d16),
        cmocka_unit_test(test_stax_b),
        cmocka_unit_test(test_inx_b),
        cmocka_unit_test(test_inr_b),
        cmocka_unit_test(test_dcr_b),
        cmocka_unit_test(test_mvi_b_d8),
        //cmocka_unit_test(test_rlc),
        cmocka_unit_test(test_dad_b),
        cmocka_unit_test(test_ldax_b),
        cmocka_unit_test(test_dcx_b),
        cmocka_unit_test(test_inr_c),
        cmocka_unit_test(test_dcr_c),
        cmocka_unit_test(test_mvi_c_d8),
        //cmocka_unit_test(tests_rrc),
        cmocka_unit_test(test_lxi_d_d16),
        cmocka_unit_test(test_stax_d),
        cmocka_unit_test(test_inx_d),
        cmocka_unit_test(test_inr_d),
        cmocka_unit_test(test_dcr_d),
        cmocka_unit_test(tests_mvi_d_d8),
        // cmocka_unit_test(test_ral),
        cmocka_unit_test(test_dad_d),
        cmocka_unit_test(test_ldax_d),
        cmocka_unit_test(test_dcx_d),
        cmocka_unit_test(test_inr_e),
        cmocka_unit_test(test_dcr_e),
        cmocka_unit_test(test_mvi_e_d8),
        // cmocka_unit_test(test_rar),
        cmocka_unit_test(test_lxi_h_16),
        cmocka_unit_test(test_shld_addr),
        cmocka_unit_test(test_inx_h),
        cmocka_unit_test(test_inr_h),
        cmocka_unit_test(test_dcr_h),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
