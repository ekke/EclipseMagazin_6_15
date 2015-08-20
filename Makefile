QMAKE_TARGET  = eclipse_magazin_6_15
PROJECT_DIR	  := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
I18N_DIR	  := $(PROJECT_DIR)/translations

include mk/cs-base.mk

