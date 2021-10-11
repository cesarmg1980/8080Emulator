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

    destroy_chip8080(chip);
}

static void test_dcx_b(void **state) {
    /* DCX B: BC = BC - 1
     * Flags: None
     * Instruction Size: 1 BYTE
     *
     * Scenario BC = 0x0100
     * Expected Result: 0x00ff
     */
    Chip8080 *chip = make_chip8080();
    chip->reg_b = 0x01;
    chip->reg_c = 0x00;
    chip->reg_pc = 0x00ff;

    dcx_b(chip);

    assert_int_equal(0x00ff, make_register_pair_from(chip->reg_b, chip->reg_c));

    destroy_chip8080(chip);

    /* Scenario BC = 0x0000 
     * Expected Result: 0xffff 
     */
    Chip8080 *chip_2 = make_chip8080();
    chip_2->reg_b = 0x00;
    chip_2->reg_c = 0x00;
    chip_2->reg_pc = 0xffff;

    dcx_b(chip_2);

    assert_int_equal(0xffff, make_register_pair_from(chip_2->reg_b, chip_2->reg_c));

    destroy_chip8080(chip_2);
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
    chip->reg_pc = 0xffff;

    inr_c(chip);

    assert_int_equal(0x00, chip->reg_c);
    assert_int_equal(0x01, chip->flags.z);
    assert_int_equal(0x00, chip->flags.s);
    assert_int_equal(0x01, chip->flags.p);
    assert_int_equal(0x00, chip->flags.ac);

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
        cmocka_unit_test(test_rlc),
        cmocka_unit_test(test_dad_b),
        cmocka_unit_test(test_ldax_b),
        cmocka_unit_test(test_dcx_b),
        cmocka_unit_test(test_inr_c),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
