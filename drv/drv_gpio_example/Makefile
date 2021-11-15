################################################################################
# file    : Makefile
# date    : 15/11/2021
# Author  : sgravois
# version : v1.0.0
################################################################################
# Modification History :
#	version	| 	Date	| 	Author	|	Description
#	v1.0.0	|15/11/2021	|sgravois	| File Creation		
################################################################################

################################################################################
#
# ENVIRONNEMENT VARIABLES to build a module
#
################################################################################
#
# ARCH : Defines the target Architecture
#
# CROSS_COMPILE : Defines the prefix of the cross compiler
#
# KERNEL : Defines the directory of the kernel sources. The kernel sources used
#			to compile the module MUST be the same as the running kernel on
#			the target. Otherwise, the module must be compiled again with the
#			correct configuration
#
################################################################################

ARCH=arm

#CROSS_COMPILE=arm-buildroot-linux-gnueabihf-
CROSS_COMPILE=arm-linux-gnueabihf-

# KERNEL=/home/ndomblides/Documents/distri_AE_2016.2/src/linux-xlnx-xilinx-v2016.2/
# KERNEL=/home/stef/buildroot-2018.02/output/build/linux-xilinx-v2017.3
# KERNEL=$(KERNEL_PATH_HOME_STEF)
# KERNEL=$(KERNEL_PATH)
# KERNEL=$(KERNEL_PATH_NEW)
# KERNEL=/home/stef/linux-xilinx-v2017.3-src
# KERNEL=/home/nico/eclipse-workspace/AE3666_LO01_ramdisk/buildroot-2018.02/output/build/linux-xlnx

KERNEL=/media/stef/ssd/yocto-astrokit-dunfell/rpi3/tmp/work/raspberrypi3-poky-linux-gnueabi/linux-raspberrypi/1_5.4.72+gitAUTOINC+5d52d9eea9_154de7bbd5-r0/linux-raspberrypi3-standard-build

OUTPUT=astrogpio
SRC_DIR=../src

# SRC_DIR=../src
obj-m += $(OUTPUT).o

################################################################################
#
# Object file list to build to generate output file
#
################################################################################

# $(OUTPUT)-objs :=\
# $(SRC_DIR)/demodHF_dev.o\
# $(SRC_DIR)/demodHF_plat.o\
# $(SRC_DIR)/demodHF_of.o\
# $(SRC_DIR)/demodHF_hw.o\
# $(SRC_DIR)/demodHF_sys.o\
# $(SRC_DIR)/demodHF_ocm.o\
# $(SRC_DIR)/demodHF_dma_buf.o\
# $(SRC_DIR)/demodHF_dma_job.o\
# $(SRC_DIR)/demodHF_dma_jobs.o\
# $(SRC_DIR)/demodHF_dma.o\
# $(SRC_DIR)/demodHF_intr.o\
# $(SRC_DIR)/demodHF_test.o

################################################################################
#
# Additive compilation flags
#
################################################################################

# ccflags-y :=  -Wno-unused-function -Wno-unused-variable -Wall -DDEMODHF_VER=\"$(MOD_VER)\"
ccflags-y :=  -Wno-unused-function -Wno-unused-variable -Wall

################################################################################
#
# Instructions to build the module with make command
#
# all is the default make instruction
#
################################################################################
allnover: 
	$(MAKE) -j4 ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL) M=$(PWD) modules
all: 

# Build Module
	$(MAKE) -j4 ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL) M=$(PWD) modules
	
# Calculate md5 sum of output file
	echo $(OUTPUT).ko > $(OUTPUT).md5
	echo Version : v$(MOD_VER)>> $(OUTPUT).md5
	date >> $(OUTPUT).md5
	echo -n MD5 sum : >> $(OUTPUT).md5
	md5sum $(OUTPUT).ko >> $(OUTPUT).md5
	
# Copy output files into version directory
	mkdir $(MOD_VER)
	cp $(OUTPUT).ko $(MOD_VER)
	cp $(OUTPUT).md5 $(MOD_VER)
	
# update module build version 
	echo $(shell echo $$(($(BLD)+1))) > $(BLD_FILE)

################################################################################
#
# Instructions to clean the project
#
################################################################################

# Nom du binaire
EXEC:=$(OUTPUT).ko
APP_REF:=$(EXEC)


stef_date := $(shell date +%Y%m%d%H)

path:
	echo $(PATH)
	
clean:
	rm $($(OUTPUT)-objs)
	rm *.o
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL) M=$(PWD) clean 

################################################################################

REP=/etc/mirza
IP1=192.168.1.79
IP2=192.168.1.81
IP3=192.168.1.77

install_ip1:$(EXEC)
	sshpass -p root ssh root@$(IP1) "mv $(REP)/$(EXEC) $(REP)/$(EXEC).save.$(stef_date) 2>/dev/null"
	sshpass -p root scp -p $(EXEC) root@$(IP1):$(REP)
	sshpass -p root ssh root@$(IP1) "ls -lrt $(REP)/$(OUTPUT)*"

install_ip2:$(EXEC)
	sshpass -p root ssh root@$(IP2) "mv $(REP)/$(EXEC) $(REP)/$(EXEC).save.$(stef_date) 2>/dev/null"
	sshpass -p root scp -p $(EXEC) root@$(IP2):$(REP)
	sshpass -p root ssh root@$(IP2) "ls -lrt $(REP)/$(OUTPUT)*"

install_ip3:$(EXEC)
	sshpass -p root ssh root@$(IP3) "mv $(REP)/$(EXEC) $(REP)/$(EXEC).save.$(stef_date) 2>/dev/null"
	sshpass -p root scp -p $(EXEC) root@$(IP3):$(REP)
	sshpass -p root ssh root@$(IP3) "ls -lrt $(REP)/$(OUTPUT)*"

################################################################################
#
# END Makefile - END Makefile - END Makefile - END Makefile - END Makefile - 
#
################################################################################





