/*
 * Declarations for BCM2838 mailbox test.
 *
 * Copyright (c) 2023 Auriga LLC
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#define MBOX0_BASE 0xFE00B880
#define MBOX1_BASE 0xFE00B8A0

#define MBOX_REG_READ   0x00
#define MBOX_REG_WRITE  0x00
#define MBOX_REG_PEEK   0x10
#define MBOX_REG_SENDER 0x14
#define MBOX_REG_STATUS 0x18
#define MBOX_REG_CONFIG 0x1C

#define MBOX_READ_EMPTY 0x40000000

#define MBOX_CHANNEL_ID_PROPERTY 8

#define MBOX_PROCESS_REQUEST      0x00000000
#define MBOX_SUCCESS              0x80000000
#define MBOX_ERROR_PARSING_BUFFER 0x80000001

#define TAG_END                    0x00000000
#define TAG_GET_FIRMWARE_REVISION  0x00000001
#define TAG_GET_FIRMWARE_VARIANT   0x00000002
#define TAG_GET_BOARD_REVISION     0x00010002
#define TAG_GET_ARM_MEMORY         0x00010005
#define TAG_GET_VC_MEMORY          0x00010006
#define TAG_SET_DEVICE_POWER_STATE 0x00028001
#define TAG_GET_CLOCK_STATE        0x00030001
#define TAG_GET_CLOCK_RATE         0x00030002
#define TAG_GET_MAX_CLOCK_RATE     0x00030004
#define TAG_GET_MIN_CLOCK_RATE     0x00030007
#define TAG_GET_CLOCKS             0x00010007
#define TAG_GET_TEMPERATURE        0x00030006
#define TAG_GET_MAX_TEMPERATURE    0x0003000a
#define TAG_ALLOCATE_BUFFER        0x00040001
#define TAG_RELEASE_FRAMEBUFFER    0x00048001
#define TAG_BLANK_FRAMEBUFFER      0x00040002
#define TAG_GET_FB_PHYS_WIDTH      0x00040003
#define TAG_TEST_FB_PHYS_WIDTH     0x00044003
#define TAG_SET_FB_PHYS_WIDTH      0x00048003
#define TAG_GET_FB_VIRT_WIDTH      0x00040004
#define TAG_TEST_FB_VIRT_WIDTH     0x00044004
#define TAG_SET_FB_VIRT_WIDTH      0x00048004
#define TAG_GET_FB_DEPTH           0x00040005
#define TAG_TEST_FB_DEPTH          0x00044005
#define TAG_SET_FB_DEPTH           0x00048005
#define TAG_GET_PIXEL_ORDER        0x00040006
#define TAG_TEST_PIXEL_ORDER       0x00044006
#define TAG_SET_PIXEL_ORDER        0x00048006
#define TAG_GET_FB_ALPHA_MODE      0x00040007
#define TAG_TEST_FB_ALPHA_MODE     0x00044007
#define TAG_SET_FB_ALPHA_MODE      0x00048007
#define TAG_GET_PITCH              0x00040008
#define TAG_SET_PITCH              0x00048008
#define TAG_GET_VIRTUAL_OFFSET     0x00040009
#define TAG_TEST_VIRTUAL_OFFSET    0x00044009
#define TAG_SET_VIRTUAL_OFFSET     0x00048009
#define TAG_GET_OVERSCAN           0x0004000a
#define TAG_TEST_OVERSCAN          0x0004400a
#define TAG_SET_OVERSCAN           0x0004800a
#define TAG_SET_PALETTE            0x0004800b
#define TAG_GET_COMMANDLINE        0x00050001
#define TAG_GET_DMA_CHANNELS       0x00060001
#define TAG_GET_THROTTLED          0x00030046
#define TAG_GET_NUM_DISPLAYS       0x00040013
#define TAG_GET_DISPLAY_SETTINGS   0x00040014
#define TAG_GET_GPIO_CONFIG        0x00030043
#define TAG_SET_GPIO_CONFIG        0x00038043
#define TAG_GET_GPIO_STATE         0x00030041
#define TAG_SET_GPIO_STATE         0x00038041
#define TAG_INITIALIZE_VCHIQ       0x00048010

#define BOARD_REVISION    0xB03115
#define FIRMWARE_REVISION 346337
#define FIRMWARE_VARIANT  0x77777777 /* TODO: Find the real value */

#define ARM_MEMORY_BASE 0x00000000
#define ARM_MEMORY_SIZE 0x3c000000
#define VC_MEMORY_BASE  0x3c000000
#define VC_MEMORY_SIZE  0x04000000
#define VC_FB_BASE      0x3c100000
#define VC_FB_SIZE      0x00096000

#define CLOCK_ID_ROOT      0x00000000
#define CLOCK_ID_EMMC      0x00000001
#define CLOCK_ID_UART      0x00000002
#define CLOCK_ID_ARM       0x00000003
#define CLOCK_ID_CORE      0x00000004
#define CLOCK_ID_UNDEFINED 0x12345678

#define CLOCK_RATE_EMMC 50000000
#define CLOCK_RATE_UART 3000000
#define CLOCK_RATE_CORE 350000000
#define CLOCK_RATE_ANY  700000000

#define DEVICE_ID_SD_CARD   0x00000000
#define DEVICE_ID_UART0     0x00000001
#define DEVICE_ID_UART1     0x00000002
#define DEVICE_ID_USB HCD   0x00000003
#define DEVICE_ID_I2C0      0x00000004
#define DEVICE_ID_I2C1      0x00000005
#define DEVICE_ID_I2C2      0x00000006
#define DEVICE_ID_SPI       0x00000007
#define DEVICE_ID_CCP2TX    0x00000008
#define DEVICE_ID_UNKNOWN_0 0x00000009
#define DEVICE_ID_UNKNOWN_1 0x0000000a

#define TEMPERATURE_ID_SOC 0x00000000

#define TEMPERATURE_SOC     25000
#define TEMPERATURE_SOC_MAX 99000

#define ALIGN_4K 4096

#define PIXEL_ORDER_BGR 0
#define PIXEL_ORDER_RGB 1

#define ALPHA_MODE_ENABLED  0
#define ALPHA_MODE_REVERSED 1
#define ALPHA_MODE_IGNORED  2

#define GPIO_MASK 0x003c

#define GPIO_0 0x00000080

#define GPIO_DIRECTION_IN  0
#define GPIO_DIRECTION_OUT 1

#define GPIO_TERMINATION_DISABLED 0
#define GPIO_TERMINATION_ENABLED  1

#define GPIO_TERMINATION_PULLUP_DISABLED 0
#define GPIO_TERMINATION_PULLUP_ENABLED  1

#define GPIO_POLARITY_LOW  0
#define GPIO_POLARITY_HIGH 1

#define GPIO_STATE_DOWN 0

/* Used to test stubs that don't perform actual work */
#define DUMMY_VALUE 0x12345678

typedef struct {
    uint32_t size;
    uint32_t req_resp_code;
} MboxBufHeader;

#define DECLARE_TAG_TYPE(TypeName, RequestValueType, ResponseValueType) \
typedef struct {                                                        \
    uint32_t id;                                                        \
    uint32_t value_buffer_size;                                         \
    union {                                                             \
        struct {                                                        \
            uint32_t zero;                                              \
            RequestValueType value;                                     \
        } request;                                                      \
        struct {                                                        \
            uint32_t size_stat;                                         \
            ResponseValueType value;                                    \
        } response;                                                     \
    };                                                                  \
} TypeName

DECLARE_TAG_TYPE(TAG_GET_FIRMWARE_REVISION_t,
    struct {},
    struct {
        uint32_t revision;
    });

DECLARE_TAG_TYPE(TAG_GET_FIRMWARE_VARIANT_t,
    struct {},
    struct {
        uint32_t variant;
    });

DECLARE_TAG_TYPE(TAG_GET_BOARD_REVISION_t,
    struct {},
    struct {
        uint32_t revision;
    });

DECLARE_TAG_TYPE(TAG_GET_ARM_MEMORY_t,
    struct {},
    struct {
        uint32_t base;
        uint32_t size;
    });

DECLARE_TAG_TYPE(TAG_GET_VC_MEMORY_t,
    struct {},
    struct {
        uint32_t base;
        uint32_t size;
    });

DECLARE_TAG_TYPE(TAG_SET_DEVICE_POWER_STATE_t,
    struct {
        uint32_t device_id;
        uint32_t cmd;
    },
    struct {
        uint32_t device_id;
        uint32_t cmd;
    });

DECLARE_TAG_TYPE(TAG_GET_CLOCK_STATE_t,
    struct {
        uint32_t clock_id;
    },
    struct {
        uint32_t clock_id;
        uint32_t cmd;
    });

DECLARE_TAG_TYPE(TAG_GET_CLOCK_RATE_t,
    struct {
        uint32_t clock_id;
    },
    struct {
        uint32_t clock_id;
        uint32_t rate;
    });

DECLARE_TAG_TYPE(TAG_GET_MAX_CLOCK_RATE_t,
    struct {
        uint32_t clock_id;
    },
    struct {
        uint32_t clock_id;
        uint32_t rate;
    });

DECLARE_TAG_TYPE(TAG_GET_MIN_CLOCK_RATE_t,
    struct {
        uint32_t clock_id;
    },
    struct {
        uint32_t clock_id;
        uint32_t rate;
    });

DECLARE_TAG_TYPE(TAG_GET_CLOCKS_t,
    struct {},
    struct {
        uint32_t root_clock;
        uint32_t arm_clock;
    });

DECLARE_TAG_TYPE(TAG_GET_TEMPERATURE_t,
    struct {
        uint32_t temperature_id;
    },
    struct {
        uint32_t temperature_id;
        uint32_t temperature;
    });

DECLARE_TAG_TYPE(TAG_GET_MAX_TEMPERATURE_t,
    struct {
        uint32_t temperature_id;
    },
    struct {
        uint32_t temperature_id;
        uint32_t temperature;
    });

DECLARE_TAG_TYPE(TAG_ALLOCATE_BUFFER_t,
    struct {
        uint32_t alignment;
    },
    struct {
        uint32_t base;
        uint32_t size;
    });

DECLARE_TAG_TYPE(TAG_RELEASE_FRAMEBUFFER_t,
    struct {},
    struct {});

DECLARE_TAG_TYPE(TAG_BLANK_FRAMEBUFFER_t,
    struct {
        uint32_t on;
    },
    struct {
        uint32_t on;
    });

DECLARE_TAG_TYPE(TAG_GET_FB_PHYS_WIDTH_t,
    struct {},
    struct {
        uint32_t width;
        uint32_t height;
    });

DECLARE_TAG_TYPE(TAG_TEST_FB_PHYS_WIDTH_t,
    struct {
        uint32_t width;
        uint32_t height;
    },
    struct {
        uint32_t width;
        uint32_t height;
    });

DECLARE_TAG_TYPE(TAG_SET_FB_PHYS_WIDTH_t,
    struct {
        uint32_t width;
        uint32_t height;
    },
    struct {
        uint32_t width;
        uint32_t height;
    });

DECLARE_TAG_TYPE(TAG_GET_FB_VIRT_WIDTH_t,
    struct {},
    struct {
        uint32_t width;
        uint32_t height;
    });

DECLARE_TAG_TYPE(TAG_TEST_FB_VIRT_WIDTH_t,
    struct {
        uint32_t width;
        uint32_t height;
    },
    struct {
        uint32_t width;
        uint32_t height;
    });

DECLARE_TAG_TYPE(TAG_SET_FB_VIRT_WIDTH_t,
    struct {
        uint32_t width;
        uint32_t height;
    },
    struct {
        uint32_t width;
        uint32_t height;
    });

DECLARE_TAG_TYPE(TAG_GET_FB_DEPTH_t,
    struct {},
    struct {
        uint32_t bpp;
    });

DECLARE_TAG_TYPE(TAG_TEST_FB_DEPTH_t,
    struct {
        uint32_t bpp;
    },
    struct {
        uint32_t bpp;
    });

DECLARE_TAG_TYPE(TAG_SET_FB_DEPTH_t,
    struct {
        uint32_t bpp;
    },
    struct {
        uint32_t bpp;
    });

DECLARE_TAG_TYPE(TAG_GET_PIXEL_ORDER_t,
    struct {},
    struct {
        uint32_t pixel_order;
    });

DECLARE_TAG_TYPE(TAG_TEST_PIXEL_ORDER_t,
    struct {
        uint32_t pixel_order;
    },
    struct {
        uint32_t pixel_order;
    });

DECLARE_TAG_TYPE(TAG_SET_PIXEL_ORDER_t,
    struct {
        uint32_t pixel_order;
    },
    struct {
        uint32_t pixel_order;
    });

DECLARE_TAG_TYPE(TAG_GET_FB_ALPHA_MODE_t,
    struct {},
    struct {
        uint32_t alpha_mode;
    });

DECLARE_TAG_TYPE(TAG_TEST_FB_ALPHA_MODE_t,
    struct {
        uint32_t alpha_mode;
    },
    struct {
        uint32_t alpha_mode;
    });

DECLARE_TAG_TYPE(TAG_SET_FB_ALPHA_MODE_t,
    struct {
        uint32_t alpha_mode;
    },
    struct {
        uint32_t alpha_mode;
    });

DECLARE_TAG_TYPE(TAG_GET_PITCH_t,
    struct {},
    struct {
        uint32_t pitch;
    });

DECLARE_TAG_TYPE(TAG_SET_PITCH_t,
    struct {
        uint32_t pitch;
    },
    struct {});

DECLARE_TAG_TYPE(TAG_GET_VIRTUAL_OFFSET_t,
    struct {},
    struct {
        uint32_t x;
        uint32_t y;
    });

DECLARE_TAG_TYPE(TAG_TEST_VIRTUAL_OFFSET_t,
    struct {
        uint32_t x;
        uint32_t y;
    },
    struct {
        uint32_t x;
        uint32_t y;
    });

DECLARE_TAG_TYPE(TAG_SET_VIRTUAL_OFFSET_t,
    struct {
        uint32_t x;
        uint32_t y;
    },
    struct {
        uint32_t x;
        uint32_t y;
    });

DECLARE_TAG_TYPE(TAG_GET_OVERSCAN_t,
    struct {},
    struct {
        uint32_t top;
        uint32_t bottom;
        uint32_t left;
        uint32_t right;
    });

DECLARE_TAG_TYPE(TAG_TEST_OVERSCAN_t,
    struct {
        uint32_t top;
        uint32_t bottom;
        uint32_t left;
        uint32_t right;
    },
    struct {
        uint32_t top;
        uint32_t bottom;
        uint32_t left;
        uint32_t right;
    });

DECLARE_TAG_TYPE(TAG_SET_OVERSCAN_t,
    struct {
        uint32_t top;
        uint32_t bottom;
        uint32_t left;
        uint32_t right;
    },
    struct {
        uint32_t top;
        uint32_t bottom;
        uint32_t left;
        uint32_t right;
    });

DECLARE_TAG_TYPE(TAG_GET_COMMANDLINE_t,
    struct {},
    struct {});

DECLARE_TAG_TYPE(TAG_GET_DMA_CHANNELS_t,
    struct {},
    struct {
        uint32_t mask;
    });

DECLARE_TAG_TYPE(TAG_GET_THROTTLED_t,
    struct {},
    struct {
        uint32_t throttled;
    });

DECLARE_TAG_TYPE(TAG_GET_NUM_DISPLAYS_t,
    struct {},
    struct {
        uint32_t num_displays;
    });

DECLARE_TAG_TYPE(TAG_GET_DISPLAY_SETTINGS_t,
    struct {},
    struct {
        uint32_t display_num;
        uint32_t phys_width;
        uint32_t phys_height;
        uint32_t bpp;
        uint16_t pitch;
        uint32_t virt_width;
        uint32_t virt_height;
        uint16_t virt_width_offset;
        uint32_t virt_height_offset;
        uint32_t fb_bus_address_lo;
        uint32_t fb_bus_address_hi;
    });

DECLARE_TAG_TYPE(TAG_GET_GPIO_CONFIG_t,
    struct {
        uint32_t gpio_num;
    },
    struct {
        uint32_t zero;
        uint32_t direction;
        uint32_t polarity;
        uint32_t term_en;
        uint32_t term_pull_up;
    });


DECLARE_TAG_TYPE(TAG_SET_GPIO_CONFIG_t,
    struct {
        uint32_t gpio_num;
        uint32_t direction;
        uint32_t polarity;
        uint32_t term_en;
        uint32_t term_pull_up;
        uint32_t state;
    },
    struct {
        uint32_t zero;
    });

DECLARE_TAG_TYPE(TAG_GET_GPIO_STATE_t,
    struct {
        uint32_t gpio_num;
    },
    struct {
        uint32_t zero;
        uint32_t state;
    });

DECLARE_TAG_TYPE(TAG_SET_GPIO_STATE_t,
    struct {
        uint32_t gpio_num;
        uint32_t state;
    },
    struct {
        uint32_t zero;
    });

DECLARE_TAG_TYPE(TAG_INITIALIZE_VCHIQ_t,
    struct {},
    struct {
        uint32_t zero;
    });

int mbox0_has_data(void);
void mbox0_read_message(uint8_t channel, void *msgbuf, size_t msgbuf_size);
void mbox1_write_message(uint8_t channel, uint32_t msg_addr);
int qtest_mbox0_has_data(QTestState *s);
void qtest_mbox0_read_message(QTestState *s, uint8_t channel, void *msgbuf, size_t msgbuf_size);
void qtest_mbox1_write_message(QTestState *s, uint8_t channel, uint32_t msg_addr);
