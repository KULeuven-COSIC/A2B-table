# ALGORITHM = {debraize, debraize-fix, dual-lookup}
ALGORITHM=debraize-fix

# TYPE = {profile, ttest}
TYPE=profile

# CFLAGS += {-DK4, -DK8}
CFLAGS += -DK4

PROJECT=$(ALGORITHM)-$(TYPE)
BUILD_DIR=bin
SHARED_DIR=src/common

CFILES += src/$(TYPE).c src/$(ALGORITHM)/$(ALGORITHM).c $(wildcard $(SHARED_DIR)/*.c) 
AFILES += $(SHARED_DIR)/aes-encrypt.S src/$(ALGORITHM)/$(ALGORITHM)-convert.S

ifeq ($(TYPE),ttest)
    DEVICE=stm32f303vc6t
    CFLAGS += -DTTEST -DSTMF303VC6T
else # profile
    DEVICE=stm32f407vg6t
    CFLAGS += -DPROFILE -DSTM32F407VG6T
endif

OPT = -O3 -g

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR) $(ALGORITHM)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include mk/rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk



