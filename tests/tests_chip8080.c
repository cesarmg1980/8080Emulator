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

    // We check that only the specified memory offset has the data, and the adjacents places are empty (0)
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

static void test_inr_b_simple_case(void **state) {
    /* Test that:
     * INR B: B <- B+1;
     * Flags: Z, S, P, AC;
     * Instruction Size: 1 BYTE
     *
     * Scenario: reg_b = 1
     * Expected Result: reg_b = 2
     * Flags: Z=0, S=0, P=1, AC=0
     */
    
    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0x01;
    chip->reg_pc = 0x00ff;

    inr_b(chip);

    assert_int_equal(0x02, chip->reg_b);
    assert_int_equal(0x00, chip->flags.z);
    assert_int_equal(0x00, chip->flags.s);
    assert_int_equal(0x00, chip->flags.p);
    //TODO: Implement AC flag
    assert_int_equal(0x0100, chip->reg_pc);
}

static void test_inr_b_minus_1_to_0(void **state) {
    /* Test that:
     * INR B: B <- B+1;
     * Flags: Z, S, P, AC;
     * Instruction Size: 1 BYTE
     *
     * Scenario: reg_b = -1
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
    // TODO Implement AC Flag
    assert_int_equal(0x0100, chip->reg_pc);
}

static void test_inr_b_minus_2_to_minus_1(void **state) {
    /* Test that:
     * INR B: B <- B+1;
     * Flags: Z, S, P, AC;
     * Instruction Size: 1 BYTE
     *
     * Scenario: reg_b = -2
     * Expected Result: reg_b = -1
     * Flags: Z=0, S=1, P=1, AC=0
     */
    
    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0xfe;
    chip->reg_pc = 0x00ff;

    inr_b(chip);

    assert_int_equal(0xff, chip->reg_b);
    assert_int_equal(0x00, chip->flags.z);
    assert_int_equal(0x01, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    // TODO Implement AC flag
    assert_int_equal(0x0100, chip->reg_pc);
}

static void test_inr_b_ac(void **state) {
    /* Test that:
     * INR B: B <- B+1;
     * Flags: Z, S, P, AC;
     * Instruction Size: 1 BYTE
     *
     * Scenario: reg_b = 7
     * Expected Result: reg_b = 8
     * Flags: Z=0, S=0, P=0, AC=0
     */
    
    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0x07;
    chip->reg_pc = 0x00ff;

    inr_b(chip);

    assert_int_equal(0x08, chip->reg_b);
    assert_int_equal(0x00, chip->flags.z);
    assert_int_equal(0x00, chip->flags.s);
    assert_int_equal(0x00, chip->flags.p);
    // TODO Implement AC Flag
    assert_int_equal(0x0100, chip->reg_pc);
}

static void test_dcr_b_simple_case(void **state) {
    /* Test that:
     * DCR B: B <- B - 1;
     * Flags: Z, S, P, AC
     * Instruction Size: 1 BYTE
     *
     * Scenario: reg_b = 5
     * Expected Result: reg_b = 4
     * Flags: Z=0, S=0, P=0, AC=0 
     */

    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0x05;
    chip->reg_pc = 0x00ff;

    dcr_b(chip);
    assert_int_equal(0x04, chip->reg_b);
    assert_int_equal(0x0, chip->flags.z);
    assert_int_equal(0x0, chip->flags.s);
    // TODO Implement AC Flag
    assert_int_equal(0x0100, chip->reg_pc);
}

// TODO: Edge Tests Cases for DCR B
//

static void test_mvi_b_d8(void **state) {
    // TODO: Implement Test

}

static void test_rlc(void **state) {
    // TODO: Implement Test
}

static void test_dad_b(void **state) {
    // TODO: Implement Test
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
}

static void test_dcx_b(void **state) {

}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_lxi_b_d16),
        cmocka_unit_test(test_stax_b),
        cmocka_unit_test(test_inx_b),
        cmocka_unit_test(test_inr_b_simple_case),
        cmocka_unit_test(test_inr_b_minus_1_to_0),
        cmocka_unit_test(test_inr_b_minus_2_to_minus_1),
        cmocka_unit_test(test_inr_b_ac),
        cmocka_unit_test(test_dcr_b_simple_case),
        cmocka_unit_test(test_mvi_b_d8),
        cmocka_unit_test(test_rlc),
        cmocka_unit_test(test_dad_b),
        cmocka_unit_test(test_ldax_b),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
