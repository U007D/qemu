/*
 * Tests set for BCM2838 mailbox property interface.
 *
 * Copyright (c) 2022 Auriga
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "hw/registerfields.h"
#include "libqtest-single.h"
#include "bcm2838-mailbox.h"

REG32(MBOX_SIZE_STAT,          0)
FIELD(MBOX_SIZE_STAT, SIZE,    0, 31)
FIELD(MBOX_SIZE_STAT, SUCCESS, 31, 1)

REG32(SET_DEVICE_POWER_STATE_CMD,        0)
FIELD(SET_DEVICE_POWER_STATE_CMD, EN,    0, 1)
FIELD(SET_DEVICE_POWER_STATE_CMD, WAIT,  1, 1)

REG32(GET_CLOCK_STATE_CMD,        0)
FIELD(GET_CLOCK_STATE_CMD, EN,    0, 1)
FIELD(GET_CLOCK_STATE_CMD, NPRES, 1, 1)

#define MBOX_TEST_MESSAGE_ADDRESS 0x10000000

#define TEST_TAG(x) TAG_ ## x
#define TEST_TAG_TYPE(x) TAG_##x##_t

#define TEST_FN_NAME(test, subtest) \
    test ## _ ## subtest ## _test

#define SETUP_FN_NAME(test, subtest) \
    test ## _ ## subtest ## _setup

#define CHECK_FN_NAME(test, subtest) \
    test ## _ ## subtest ## _spec_check

#define DECLARE_TEST_CASE_SETUP(testname, ...)              \
    void SETUP_FN_NAME(testname, __VA_ARGS__)               \
                             (TEST_TAG_TYPE(testname) * tag)

/*----------------------------------------------------------------------------*/
#define DECLARE_TEST_CASE(testname, ...)                                       \
    __attribute__((weak))                                                      \
    void SETUP_FN_NAME(testname, __VA_ARGS__)                                  \
                      (TEST_TAG_TYPE(testname) * tag);                         \
    static void CHECK_FN_NAME(testname, __VA_ARGS__)                           \
                             (TEST_TAG_TYPE(testname) *tag);                   \
    static void TEST_FN_NAME(testname, __VA_ARGS__)(void)                      \
    {                                                                          \
        struct {                                                               \
            MboxBufHeader header;                                              \
            TEST_TAG_TYPE(testname) tag;                                       \
            uint32_t end_tag;                                                  \
        } mailbox_buffer = { 0 };                                              \
                                                                               \
        QTestState *qts = qtest_init("-machine raspi4b");                   \
                                                                               \
        mailbox_buffer.header.size = sizeof(mailbox_buffer);                   \
        mailbox_buffer.header.req_resp_code = MBOX_PROCESS_REQUEST;            \
                                                                               \
        mailbox_buffer.tag.id = TEST_TAG(testname);                            \
        mailbox_buffer.tag.value_buffer_size = MAX(                            \
            sizeof(mailbox_buffer.tag.request.value),                          \
            sizeof(mailbox_buffer.tag.response.value));                        \
        mailbox_buffer.tag.request.zero = 0;                                   \
                                                                               \
        mailbox_buffer.end_tag = TAG_END;                                      \
                                                                               \
        if (SETUP_FN_NAME(testname, __VA_ARGS__)) {                            \
            SETUP_FN_NAME(testname, __VA_ARGS__)(&mailbox_buffer.tag);         \
        }                                                                      \
                                                                               \
        qtest_memwrite(qts, MBOX_TEST_MESSAGE_ADDRESS,                         \
                    &mailbox_buffer, sizeof(mailbox_buffer));                  \
        qtest_mbox1_write_message(qts, MBOX_CHANNEL_ID_PROPERTY,               \
                            MBOX_TEST_MESSAGE_ADDRESS);                        \
                                                                               \
        qtest_mbox0_read_message(qts, MBOX_CHANNEL_ID_PROPERTY,                \
                            &mailbox_buffer, sizeof(mailbox_buffer));          \
                                                                               \
        g_assert_cmphex(mailbox_buffer.header.req_resp_code, ==, MBOX_SUCCESS);\
                                                                               \
        g_assert_cmphex(mailbox_buffer.tag.id, ==, TEST_TAG(testname));        \
                                                                               \
        uint32_t size = FIELD_EX32(mailbox_buffer.tag.response.size_stat,      \
                                   MBOX_SIZE_STAT, SIZE);                      \
        uint32_t success = FIELD_EX32(mailbox_buffer.tag.response.size_stat,   \
                                      MBOX_SIZE_STAT, SUCCESS);                \
        g_assert_cmpint(size, ==, sizeof(mailbox_buffer.tag.response.value));  \
        g_assert_cmpint(success, ==, 1);                                       \
                                                                               \
        CHECK_FN_NAME(testname, __VA_ARGS__)(&mailbox_buffer.tag);             \
                                                                               \
        qtest_quit(qts);                                                       \
    }                                                                          \
    static void CHECK_FN_NAME(testname, __VA_ARGS__)                           \
                             (TEST_TAG_TYPE(testname) * tag)

/*----------------------------------------------------------------------------*/

#define QTEST_ADD_TEST_CASE(testname, ...)                                     \
    qtest_add_func(stringify(/bcm2838/mbox/property/                           \
                   TEST_FN_NAME(testname, __VA_ARGS__)-test),                  \
                   TEST_FN_NAME(testname, __VA_ARGS__))

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_FIRMWARE_REVISION) {
    g_assert_cmpint(tag->response.value.revision, ==, FIRMWARE_REVISION);
}

// /*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_BOARD_REVISION) {
    g_assert_cmpint(tag->response.value.revision, ==, BOARD_REVISION);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_ARM_MEMORY) {
    g_assert_cmphex(tag->response.value.base, ==, ARM_MEMORY_BASE);
    g_assert_cmphex(tag->response.value.size, ==, ARM_MEMORY_SIZE);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_VC_MEMORY) {
    g_assert_cmphex(tag->response.value.base, ==, VC_MEMORY_BASE);
    g_assert_cmphex(tag->response.value.size, ==, VC_MEMORY_SIZE);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_DEVICE_POWER_STATE) {
    uint32_t enabled =
        FIELD_EX32(tag->response.value.cmd, SET_DEVICE_POWER_STATE_CMD, EN);
    uint32_t wait =
        FIELD_EX32(tag->response.value.cmd, SET_DEVICE_POWER_STATE_CMD, WAIT);
    g_assert_cmphex(tag->response.value.device_id, ==, DEVICE_ID_UART0);
    g_assert_cmpint(enabled, ==, 1);
    g_assert_cmpint(wait, ==, 0);
}
DECLARE_TEST_CASE_SETUP(SET_DEVICE_POWER_STATE) {
    tag->request.value.device_id = DEVICE_ID_UART0;
    tag->response.value.cmd =
        FIELD_DP32(tag->response.value.cmd, SET_DEVICE_POWER_STATE_CMD, EN, 1);
    tag->response.value.cmd =
        FIELD_DP32(tag->response.value.cmd, SET_DEVICE_POWER_STATE_CMD, WAIT, 1);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_CLOCK_STATE) {
    uint32_t enabled =
        FIELD_EX32(tag->response.value.cmd, GET_CLOCK_STATE_CMD, EN);
    uint32_t not_present =
        FIELD_EX32(tag->response.value.cmd, GET_CLOCK_STATE_CMD, NPRES);
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_CORE);
    g_assert_cmphex(enabled, ==, 1);
    g_assert_cmphex(not_present, ==, 0);
}
DECLARE_TEST_CASE_SETUP(GET_CLOCK_STATE) {
   tag->request.value.clock_id = CLOCK_ID_CORE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_CLOCK_RATE, EMMC) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_EMMC);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_EMMC);
}
DECLARE_TEST_CASE_SETUP(GET_CLOCK_RATE, EMMC) {
    tag->request.value.clock_id = CLOCK_ID_EMMC;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_MAX_CLOCK_RATE, EMMC) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_EMMC);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_EMMC);
}
DECLARE_TEST_CASE_SETUP(GET_MAX_CLOCK_RATE, EMMC) {
    tag->request.value.clock_id = CLOCK_ID_EMMC;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_MIN_CLOCK_RATE, EMMC) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_EMMC);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_EMMC);
}
DECLARE_TEST_CASE_SETUP(GET_MIN_CLOCK_RATE, EMMC) {
    tag->request.value.clock_id = CLOCK_ID_EMMC;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_CLOCK_RATE, UART) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_UART);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_UART);
}
DECLARE_TEST_CASE_SETUP(GET_CLOCK_RATE, UART) {
    tag->request.value.clock_id = CLOCK_ID_UART;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_MAX_CLOCK_RATE, UART) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_UART);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_UART);
}
DECLARE_TEST_CASE_SETUP(GET_MAX_CLOCK_RATE, UART) {
    tag->request.value.clock_id = CLOCK_ID_UART;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_MIN_CLOCK_RATE, UART) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_UART);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_UART);
}
DECLARE_TEST_CASE_SETUP(GET_MIN_CLOCK_RATE, UART) {
    tag->request.value.clock_id = CLOCK_ID_UART;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_CLOCK_RATE, CORE) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_CORE);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_CORE);
}
DECLARE_TEST_CASE_SETUP(GET_CLOCK_RATE, CORE) {
    tag->request.value.clock_id = CLOCK_ID_CORE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_MAX_CLOCK_RATE, CORE) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_CORE);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_CORE);
}
DECLARE_TEST_CASE_SETUP(GET_MAX_CLOCK_RATE, CORE) {
    tag->request.value.clock_id = CLOCK_ID_CORE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_MIN_CLOCK_RATE, CORE) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_CORE);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_CORE);
}
DECLARE_TEST_CASE_SETUP(GET_MIN_CLOCK_RATE, CORE) {
    tag->request.value.clock_id = CLOCK_ID_CORE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_CLOCK_RATE, ANY) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_UNDEFINED);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_ANY);
}
DECLARE_TEST_CASE_SETUP(GET_CLOCK_RATE, ANY) {
    tag->request.value.clock_id = CLOCK_ID_UNDEFINED;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_MAX_CLOCK_RATE, ANY) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_UNDEFINED);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_ANY);
}
DECLARE_TEST_CASE_SETUP(GET_MAX_CLOCK_RATE, ANY) {
    tag->request.value.clock_id = CLOCK_ID_UNDEFINED;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_MIN_CLOCK_RATE, ANY) {
    g_assert_cmphex(tag->response.value.clock_id, ==, CLOCK_ID_UNDEFINED);
    g_assert_cmphex(tag->response.value.rate, ==, CLOCK_RATE_ANY);
}
DECLARE_TEST_CASE_SETUP(GET_MIN_CLOCK_RATE, ANY) {
    tag->request.value.clock_id = CLOCK_ID_UNDEFINED;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_CLOCKS) {
    g_assert_cmphex(tag->response.value.root_clock, ==, CLOCK_ID_ROOT);
    g_assert_cmphex(tag->response.value.arm_clock, ==, CLOCK_ID_ARM);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_TEMPERATURE) {
    g_assert_cmphex(tag->response.value.temperature_id, ==, TEMPERATURE_ID_SOC);
    g_assert_cmpint(tag->response.value.temperature, ==, TEMPERATURE_SOC);
}
DECLARE_TEST_CASE_SETUP(GET_TEMPERATURE) {
    tag->request.value.temperature_id = TEMPERATURE_ID_SOC;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_MAX_TEMPERATURE) {
    g_assert_cmphex(tag->response.value.temperature_id, ==, TEMPERATURE_ID_SOC);
    g_assert_cmpint(tag->response.value.temperature, ==, TEMPERATURE_SOC_MAX);
}
DECLARE_TEST_CASE_SETUP(GET_MAX_TEMPERATURE) {
    tag->request.value.temperature_id = TEMPERATURE_ID_SOC;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(ALLOCATE_BUFFER) {
    g_assert_cmphex(tag->response.value.base, ==, VC_FB_BASE);
    g_assert_cmphex(tag->response.value.size, ==, VC_FB_SIZE);
}
DECLARE_TEST_CASE_SETUP(ALLOCATE_BUFFER) {
    tag->request.value.alignment = ALIGN_4K;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(RELEASE_FRAMEBUFFER) {
    /* No special checks are needed for this test */
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(BLANK_FRAMEBUFFER) {
    g_assert_cmphex(tag->response.value.on, ==, 0);
}
DECLARE_TEST_CASE_SETUP(BLANK_FRAMEBUFFER) {
    tag->request.value.on = 0;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(TEST_FB_PHYS_WIDTH) {
    g_assert_cmpint(tag->response.value.width, ==, DUMMY_VALUE);
    g_assert_cmpint(tag->response.value.height, ==, DUMMY_VALUE);
}
DECLARE_TEST_CASE_SETUP(TEST_FB_PHYS_WIDTH) {
    tag->request.value.width = DUMMY_VALUE;
    tag->request.value.height = DUMMY_VALUE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_FB_PHYS_WIDTH, INITIAL) {
    g_assert_cmpint(tag->response.value.width, ==, 640);
    g_assert_cmpint(tag->response.value.height, ==, 480);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_FB_PHYS_WIDTH) {
    g_assert_cmpint(tag->response.value.width, ==, 800);
    g_assert_cmpint(tag->response.value.height, ==, 600);
}
DECLARE_TEST_CASE_SETUP(SET_FB_PHYS_WIDTH) {
    tag->request.value.width = 800;
    tag->request.value.height = 600;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(TEST_FB_VIRT_WIDTH) {
    g_assert_cmpint(tag->response.value.width, ==, DUMMY_VALUE);
    g_assert_cmpint(tag->response.value.height, ==, DUMMY_VALUE);
}
DECLARE_TEST_CASE_SETUP(TEST_FB_VIRT_WIDTH) {
    tag->request.value.width = DUMMY_VALUE;
    tag->request.value.height = DUMMY_VALUE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_FB_VIRT_WIDTH, INITIAL) {
    g_assert_cmpint(tag->response.value.width, ==, 640);
    g_assert_cmpint(tag->response.value.height, ==, 480);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_FB_VIRT_WIDTH) {
    g_assert_cmpint(tag->response.value.width, ==, 800);
    g_assert_cmpint(tag->response.value.height, ==, 600);
}
DECLARE_TEST_CASE_SETUP(SET_FB_VIRT_WIDTH) {
    tag->request.value.width = 800;
    tag->request.value.height = 600;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(TEST_FB_DEPTH) {
    g_assert_cmpint(tag->response.value.bpp, ==, DUMMY_VALUE);
}
DECLARE_TEST_CASE_SETUP(TEST_FB_DEPTH) {
    tag->request.value.bpp = DUMMY_VALUE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_FB_DEPTH) {
    g_assert_cmpint(tag->response.value.bpp, ==, 16);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_FB_DEPTH) {
    g_assert_cmpint(tag->response.value.bpp, ==, 24);
}
DECLARE_TEST_CASE_SETUP(SET_FB_DEPTH) {
    tag->request.value.bpp = 24;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(TEST_PIXEL_ORDER) {
    g_assert_cmphex(tag->response.value.pixel_order, ==, DUMMY_VALUE);
}
DECLARE_TEST_CASE_SETUP(TEST_PIXEL_ORDER) {
    tag->request.value.pixel_order = DUMMY_VALUE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_PIXEL_ORDER) {
    g_assert_cmphex(tag->response.value.pixel_order, ==, PIXEL_ORDER_RGB);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_PIXEL_ORDER, BGR) {
    g_assert_cmphex(tag->response.value.pixel_order, ==, PIXEL_ORDER_BGR);
}
DECLARE_TEST_CASE_SETUP(SET_PIXEL_ORDER, BGR) {
    tag->request.value.pixel_order = PIXEL_ORDER_BGR;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_PIXEL_ORDER, RGB) {
    g_assert_cmphex(tag->response.value.pixel_order, ==, PIXEL_ORDER_BGR);
}
DECLARE_TEST_CASE_SETUP(SET_PIXEL_ORDER, RGB) {
    tag->request.value.pixel_order = PIXEL_ORDER_BGR;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(TEST_FB_ALPHA_MODE) {
    g_assert_cmphex(tag->response.value.alpha_mode, ==, DUMMY_VALUE);
}
DECLARE_TEST_CASE_SETUP(TEST_FB_ALPHA_MODE) {
    tag->request.value.alpha_mode = DUMMY_VALUE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_FB_ALPHA_MODE) {
    g_assert_cmphex(tag->response.value.alpha_mode, ==, ALPHA_MODE_IGNORED);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_FB_ALPHA_MODE, ENABLED) {
    g_assert_cmphex(tag->response.value.alpha_mode, ==, ALPHA_MODE_ENABLED);
}
DECLARE_TEST_CASE_SETUP(SET_FB_ALPHA_MODE, ENABLED) {
    tag->request.value.alpha_mode = ALPHA_MODE_ENABLED;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_FB_ALPHA_MODE, REVERSED) {
    g_assert_cmphex(tag->response.value.alpha_mode, ==, ALPHA_MODE_REVERSED);
}
DECLARE_TEST_CASE_SETUP(SET_FB_ALPHA_MODE, REVERSED) {
    tag->request.value.alpha_mode = ALPHA_MODE_REVERSED;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_FB_ALPHA_MODE, IGNORED) {
    g_assert_cmphex(tag->response.value.alpha_mode, ==, ALPHA_MODE_IGNORED);
}
DECLARE_TEST_CASE_SETUP(SET_FB_ALPHA_MODE, IGNORED) {
    tag->request.value.alpha_mode = ALPHA_MODE_IGNORED;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_PITCH) {
    g_assert_cmpint(tag->response.value.pitch, ==, 1280);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(TEST_VIRTUAL_OFFSET) {
    g_assert_cmpint(tag->response.value.x, ==, DUMMY_VALUE);
    g_assert_cmpint(tag->response.value.y, ==, DUMMY_VALUE);
}
DECLARE_TEST_CASE_SETUP(TEST_VIRTUAL_OFFSET) {
    tag->request.value.x = DUMMY_VALUE;
    tag->request.value.y = DUMMY_VALUE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_VIRTUAL_OFFSET) {
    g_assert_cmpint(tag->response.value.x, ==, 0);
    g_assert_cmpint(tag->response.value.y, ==, 0);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_VIRTUAL_OFFSET, _0_) {
    g_assert_cmpint(tag->response.value.x, ==, 0);
    g_assert_cmpint(tag->response.value.y, ==, 0);
}
DECLARE_TEST_CASE_SETUP(SET_VIRTUAL_OFFSET, _0_) {
    tag->request.value.x = 0;
    tag->request.value.y = 0;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_VIRTUAL_OFFSET, _42_) {
    g_assert_cmpint(tag->response.value.x, ==, 42);
    g_assert_cmpint(tag->response.value.y, ==, 42);
}
DECLARE_TEST_CASE_SETUP(SET_VIRTUAL_OFFSET, _42_) {
    tag->request.value.x = 42;
    tag->request.value.y = 42;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_OVERSCAN) {
    g_assert_cmpint(tag->response.value.top, ==, 0);
    g_assert_cmpint(tag->response.value.bottom, ==, 0);
    g_assert_cmpint(tag->response.value.left, ==, 0);
    g_assert_cmpint(tag->response.value.right, ==, 0);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(TEST_OVERSCAN) {
    g_assert_cmpint(tag->response.value.top, ==, 0);
    g_assert_cmpint(tag->response.value.bottom, ==, 0);
    g_assert_cmpint(tag->response.value.left, ==, 0);
    g_assert_cmpint(tag->response.value.right, ==, 0);
}
DECLARE_TEST_CASE_SETUP(TEST_OVERSCAN) {
    tag->request.value.top = DUMMY_VALUE;
    tag->request.value.bottom = DUMMY_VALUE;
    tag->request.value.left = DUMMY_VALUE;
    tag->request.value.right = DUMMY_VALUE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(SET_OVERSCAN) {
    g_assert_cmpint(tag->response.value.top, ==, 0);
    g_assert_cmpint(tag->response.value.bottom, ==, 0);
    g_assert_cmpint(tag->response.value.left, ==, 0);
    g_assert_cmpint(tag->response.value.right, ==, 0);
}
DECLARE_TEST_CASE_SETUP(SET_OVERSCAN) {
    tag->request.value.top = DUMMY_VALUE;
    tag->request.value.bottom = DUMMY_VALUE;
    tag->request.value.left = DUMMY_VALUE;
    tag->request.value.right = DUMMY_VALUE;
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_DMA_CHANNELS) {
    g_assert_cmphex(tag->response.value.mask, ==, GPIO_MASK);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_COMMANDLINE) {
    /* No special checks are needed for this test case */
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_THROTTLED) {
    g_assert_cmpint(tag->response.value.throttled, ==, 0);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(GET_NUM_DISPLAYS) {
    g_assert_cmpint(tag->response.value.num_displays, ==, 1);
}

/*----------------------------------------------------------------------------*/
DECLARE_TEST_CASE(INITIALIZE_VCHIQ) {
    g_assert_cmpint(tag->response.value.zero, ==, 0);
}

/*----------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    g_test_init(&argc, &argv, NULL);

    QTEST_ADD_TEST_CASE(GET_FIRMWARE_REVISION);
    QTEST_ADD_TEST_CASE(GET_BOARD_REVISION);
    QTEST_ADD_TEST_CASE(GET_ARM_MEMORY);
    QTEST_ADD_TEST_CASE(GET_VC_MEMORY);
    QTEST_ADD_TEST_CASE(SET_DEVICE_POWER_STATE);
    QTEST_ADD_TEST_CASE(GET_CLOCK_STATE);
    QTEST_ADD_TEST_CASE(GET_CLOCK_RATE, EMMC);
    QTEST_ADD_TEST_CASE(GET_MAX_CLOCK_RATE, EMMC);
    QTEST_ADD_TEST_CASE(GET_MIN_CLOCK_RATE, EMMC);
    QTEST_ADD_TEST_CASE(GET_CLOCK_RATE, UART);
    QTEST_ADD_TEST_CASE(GET_MAX_CLOCK_RATE, UART);
    QTEST_ADD_TEST_CASE(GET_MIN_CLOCK_RATE, UART);
    QTEST_ADD_TEST_CASE(GET_CLOCK_RATE, CORE);
    QTEST_ADD_TEST_CASE(GET_MAX_CLOCK_RATE, CORE);
    QTEST_ADD_TEST_CASE(GET_MIN_CLOCK_RATE, CORE);
    QTEST_ADD_TEST_CASE(GET_CLOCK_RATE, ANY);
    QTEST_ADD_TEST_CASE(GET_MAX_CLOCK_RATE, ANY);
    QTEST_ADD_TEST_CASE(GET_MIN_CLOCK_RATE, ANY);
    QTEST_ADD_TEST_CASE(GET_CLOCKS);
    QTEST_ADD_TEST_CASE(GET_TEMPERATURE);
    QTEST_ADD_TEST_CASE(GET_MAX_TEMPERATURE);
    QTEST_ADD_TEST_CASE(ALLOCATE_BUFFER);
    QTEST_ADD_TEST_CASE(RELEASE_FRAMEBUFFER);
    QTEST_ADD_TEST_CASE(BLANK_FRAMEBUFFER);
    QTEST_ADD_TEST_CASE(TEST_FB_PHYS_WIDTH);
    QTEST_ADD_TEST_CASE(GET_FB_PHYS_WIDTH, INITIAL);
    QTEST_ADD_TEST_CASE(SET_FB_PHYS_WIDTH);
    QTEST_ADD_TEST_CASE(TEST_FB_VIRT_WIDTH);
    QTEST_ADD_TEST_CASE(GET_FB_VIRT_WIDTH, INITIAL);
    QTEST_ADD_TEST_CASE(SET_FB_VIRT_WIDTH);
    QTEST_ADD_TEST_CASE(TEST_FB_DEPTH);
    QTEST_ADD_TEST_CASE(GET_FB_DEPTH);
    QTEST_ADD_TEST_CASE(SET_FB_DEPTH);
    QTEST_ADD_TEST_CASE(TEST_PIXEL_ORDER);
    QTEST_ADD_TEST_CASE(GET_PIXEL_ORDER);
    QTEST_ADD_TEST_CASE(SET_PIXEL_ORDER, BGR);
    QTEST_ADD_TEST_CASE(SET_PIXEL_ORDER, RGB);
    QTEST_ADD_TEST_CASE(TEST_FB_ALPHA_MODE);
    QTEST_ADD_TEST_CASE(GET_FB_ALPHA_MODE);
    QTEST_ADD_TEST_CASE(SET_FB_ALPHA_MODE, ENABLED);
    QTEST_ADD_TEST_CASE(SET_FB_ALPHA_MODE, REVERSED);
    QTEST_ADD_TEST_CASE(SET_FB_ALPHA_MODE, IGNORED);
    QTEST_ADD_TEST_CASE(GET_PITCH);
    QTEST_ADD_TEST_CASE(TEST_VIRTUAL_OFFSET);
    QTEST_ADD_TEST_CASE(GET_VIRTUAL_OFFSET);
    QTEST_ADD_TEST_CASE(SET_VIRTUAL_OFFSET, _0_);
    QTEST_ADD_TEST_CASE(SET_VIRTUAL_OFFSET, _42_);
    QTEST_ADD_TEST_CASE(GET_OVERSCAN);
    QTEST_ADD_TEST_CASE(TEST_OVERSCAN);
    QTEST_ADD_TEST_CASE(SET_OVERSCAN);
    QTEST_ADD_TEST_CASE(GET_DMA_CHANNELS);
    QTEST_ADD_TEST_CASE(GET_COMMANDLINE);
    QTEST_ADD_TEST_CASE(GET_THROTTLED);
    QTEST_ADD_TEST_CASE(GET_NUM_DISPLAYS);
    QTEST_ADD_TEST_CASE(INITIALIZE_VCHIQ);

    return g_test_run();
}
