PROJECT = inner_led_toggle
BUILD_DIR = bin

CFILES = my-project.c

DEVICE=stm32f103c6
OOCD_FILE = board/stm32f4discovery.cfg

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

astyle: 
	astyle --style=attach --break-blocks --pad-oper --pad-header --align-pointer=name --align-reference=name my-project.c
