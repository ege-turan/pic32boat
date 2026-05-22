#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/boatFramework.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/boatFramework.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=FrameworkSource/ES_CheckEvents.c FrameworkSource/ES_DeferRecall.c FrameworkSource/ES_Framework.c FrameworkSource/ES_LookupTables.c FrameworkSource/ES_Port.c FrameworkSource/ES_PostList.c FrameworkSource/ES_Queue.c FrameworkSource/ES_Timers.c FrameworkSource/terminal.c FrameworkSource/circular_buffer_no_modulo_threadsafe.c FrameworkSource/dbprintf.c ProjectSource/EventCheckers.c ProjectSource/main.c ProjectSource/PIC32_IC_Lib.c ProjectSource/PIC32_PWM_Lib.c ProjectSource/DrivingService.c ProjectSource/CommunicationService.c ProjectSource/PairedServoService.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o ${OBJECTDIR}/FrameworkSource/ES_Framework.o ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o ${OBJECTDIR}/FrameworkSource/ES_Port.o ${OBJECTDIR}/FrameworkSource/ES_PostList.o ${OBJECTDIR}/FrameworkSource/ES_Queue.o ${OBJECTDIR}/FrameworkSource/ES_Timers.o ${OBJECTDIR}/FrameworkSource/terminal.o ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o ${OBJECTDIR}/FrameworkSource/dbprintf.o ${OBJECTDIR}/ProjectSource/EventCheckers.o ${OBJECTDIR}/ProjectSource/main.o ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o ${OBJECTDIR}/ProjectSource/DrivingService.o ${OBJECTDIR}/ProjectSource/CommunicationService.o ${OBJECTDIR}/ProjectSource/PairedServoService.o
POSSIBLE_DEPFILES=${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o.d ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o.d ${OBJECTDIR}/FrameworkSource/ES_Framework.o.d ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o.d ${OBJECTDIR}/FrameworkSource/ES_Port.o.d ${OBJECTDIR}/FrameworkSource/ES_PostList.o.d ${OBJECTDIR}/FrameworkSource/ES_Queue.o.d ${OBJECTDIR}/FrameworkSource/ES_Timers.o.d ${OBJECTDIR}/FrameworkSource/terminal.o.d ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o.d ${OBJECTDIR}/FrameworkSource/dbprintf.o.d ${OBJECTDIR}/ProjectSource/EventCheckers.o.d ${OBJECTDIR}/ProjectSource/main.o.d ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o.d ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o.d ${OBJECTDIR}/ProjectSource/DrivingService.o.d ${OBJECTDIR}/ProjectSource/CommunicationService.o.d ${OBJECTDIR}/ProjectSource/PairedServoService.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o ${OBJECTDIR}/FrameworkSource/ES_Framework.o ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o ${OBJECTDIR}/FrameworkSource/ES_Port.o ${OBJECTDIR}/FrameworkSource/ES_PostList.o ${OBJECTDIR}/FrameworkSource/ES_Queue.o ${OBJECTDIR}/FrameworkSource/ES_Timers.o ${OBJECTDIR}/FrameworkSource/terminal.o ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o ${OBJECTDIR}/FrameworkSource/dbprintf.o ${OBJECTDIR}/ProjectSource/EventCheckers.o ${OBJECTDIR}/ProjectSource/main.o ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o ${OBJECTDIR}/ProjectSource/DrivingService.o ${OBJECTDIR}/ProjectSource/CommunicationService.o ${OBJECTDIR}/ProjectSource/PairedServoService.o

# Source Files
SOURCEFILES=FrameworkSource/ES_CheckEvents.c FrameworkSource/ES_DeferRecall.c FrameworkSource/ES_Framework.c FrameworkSource/ES_LookupTables.c FrameworkSource/ES_Port.c FrameworkSource/ES_PostList.c FrameworkSource/ES_Queue.c FrameworkSource/ES_Timers.c FrameworkSource/terminal.c FrameworkSource/circular_buffer_no_modulo_threadsafe.c FrameworkSource/dbprintf.c ProjectSource/EventCheckers.c ProjectSource/main.c ProjectSource/PIC32_IC_Lib.c ProjectSource/PIC32_PWM_Lib.c ProjectSource/DrivingService.c ProjectSource/CommunicationService.c ProjectSource/PairedServoService.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/boatFramework.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX170F256B
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o: FrameworkSource/ES_CheckEvents.c  .generated_files/flags/default/67f957079cb1dac315ef434f31d6267b0b7032a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o FrameworkSource/ES_CheckEvents.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o: FrameworkSource/ES_DeferRecall.c  .generated_files/flags/default/438057ded7ba66bf4c0dc40d0dbef7ec5cc707e0 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o FrameworkSource/ES_DeferRecall.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Framework.o: FrameworkSource/ES_Framework.c  .generated_files/flags/default/149f2bb7fa9aa6f9689ffe238d5285abd93a2aa8 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Framework.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Framework.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Framework.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Framework.o FrameworkSource/ES_Framework.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_LookupTables.o: FrameworkSource/ES_LookupTables.c  .generated_files/flags/default/467c122d1fd73d29a0d3aa85125d8e677cc72a68 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_LookupTables.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o FrameworkSource/ES_LookupTables.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Port.o: FrameworkSource/ES_Port.c  .generated_files/flags/default/d917c0e1785674ee5f24c64dd3372c057d1e5fe4 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Port.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Port.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Port.o FrameworkSource/ES_Port.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_PostList.o: FrameworkSource/ES_PostList.c  .generated_files/flags/default/2eccca45bffc353f49218cd34e18b4d5c709b564 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_PostList.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_PostList.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_PostList.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_PostList.o FrameworkSource/ES_PostList.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Queue.o: FrameworkSource/ES_Queue.c  .generated_files/flags/default/34291e4f16a27cafa0493a726225a374bf629470 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Queue.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Queue.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Queue.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Queue.o FrameworkSource/ES_Queue.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Timers.o: FrameworkSource/ES_Timers.c  .generated_files/flags/default/6141319a368c7125cab1e0a21b3fbadf48fd2d43 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Timers.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Timers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Timers.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Timers.o FrameworkSource/ES_Timers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/terminal.o: FrameworkSource/terminal.c  .generated_files/flags/default/b87b3e2ec076f8103895b303e1456e65239eed34 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/terminal.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/terminal.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/terminal.o.d" -o ${OBJECTDIR}/FrameworkSource/terminal.o FrameworkSource/terminal.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o: FrameworkSource/circular_buffer_no_modulo_threadsafe.c  .generated_files/flags/default/74566707a1658f92b0c29b8b5f084a1339bb2566 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o.d" -o ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o FrameworkSource/circular_buffer_no_modulo_threadsafe.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/dbprintf.o: FrameworkSource/dbprintf.c  .generated_files/flags/default/4733bc369a88f28f43379d289e1888ab92261f32 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/dbprintf.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/dbprintf.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/dbprintf.o.d" -o ${OBJECTDIR}/FrameworkSource/dbprintf.o FrameworkSource/dbprintf.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/EventCheckers.o: ProjectSource/EventCheckers.c  .generated_files/flags/default/43ebf79f629ca8d97210fd11e4083fbfdd0da83d .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/EventCheckers.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/EventCheckers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/EventCheckers.o.d" -o ${OBJECTDIR}/ProjectSource/EventCheckers.o ProjectSource/EventCheckers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/main.o: ProjectSource/main.c  .generated_files/flags/default/91f644575c6a5e81656414723f4f41f8e3d1fbb3 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/main.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/main.o.d" -o ${OBJECTDIR}/ProjectSource/main.o ProjectSource/main.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o: ProjectSource/PIC32_IC_Lib.c  .generated_files/flags/default/35f28705b32aa3f67b0759f4c938b14b1ec0c099 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o.d" -o ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o ProjectSource/PIC32_IC_Lib.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o: ProjectSource/PIC32_PWM_Lib.c  .generated_files/flags/default/1ba436e1e509110751b577292356f699ac56c4b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o.d" -o ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o ProjectSource/PIC32_PWM_Lib.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/DrivingService.o: ProjectSource/DrivingService.c  .generated_files/flags/default/a1b20d91ae0741c5b049627a86f72ede783e413 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/DrivingService.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/DrivingService.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/DrivingService.o.d" -o ${OBJECTDIR}/ProjectSource/DrivingService.o ProjectSource/DrivingService.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/CommunicationService.o: ProjectSource/CommunicationService.c  .generated_files/flags/default/555f87950be573cd46346c453a8454a812a5fd1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/CommunicationService.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/CommunicationService.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/CommunicationService.o.d" -o ${OBJECTDIR}/ProjectSource/CommunicationService.o ProjectSource/CommunicationService.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PairedServoService.o: ProjectSource/PairedServoService.c  .generated_files/flags/default/144abf07b89396ac1d61cd05375a4c19670e2fc1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PairedServoService.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PairedServoService.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PairedServoService.o.d" -o ${OBJECTDIR}/ProjectSource/PairedServoService.o ProjectSource/PairedServoService.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
else
${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o: FrameworkSource/ES_CheckEvents.c  .generated_files/flags/default/ce39aa6fc368f2a42dbebe31b282de3fb2c96793 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o FrameworkSource/ES_CheckEvents.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o: FrameworkSource/ES_DeferRecall.c  .generated_files/flags/default/6674a0baca046721771a4af5c3ea5128ac0748f4 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o FrameworkSource/ES_DeferRecall.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Framework.o: FrameworkSource/ES_Framework.c  .generated_files/flags/default/7604d5903b440fdb7b1d4eef65113fc5859b2a0a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Framework.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Framework.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Framework.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Framework.o FrameworkSource/ES_Framework.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_LookupTables.o: FrameworkSource/ES_LookupTables.c  .generated_files/flags/default/8b6b56fcd701ec8e3a09776b26105b2a3ebae5f6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_LookupTables.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o FrameworkSource/ES_LookupTables.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Port.o: FrameworkSource/ES_Port.c  .generated_files/flags/default/d0a39d777224d16237684ae9562acfd7f1903dca .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Port.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Port.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Port.o FrameworkSource/ES_Port.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_PostList.o: FrameworkSource/ES_PostList.c  .generated_files/flags/default/fc33fd6cbed6588db5207ce129d54fe88c33cdbd .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_PostList.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_PostList.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_PostList.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_PostList.o FrameworkSource/ES_PostList.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Queue.o: FrameworkSource/ES_Queue.c  .generated_files/flags/default/6fe639f46616bd876a87bd1298c432f09c1f88aa .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Queue.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Queue.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Queue.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Queue.o FrameworkSource/ES_Queue.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Timers.o: FrameworkSource/ES_Timers.c  .generated_files/flags/default/fcb5677bcd4d89c6941ab632ab72b3275d826c29 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Timers.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Timers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Timers.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Timers.o FrameworkSource/ES_Timers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/terminal.o: FrameworkSource/terminal.c  .generated_files/flags/default/3a541cfa01585a6f9e4da716b9d10ad4b91c2362 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/terminal.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/terminal.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/terminal.o.d" -o ${OBJECTDIR}/FrameworkSource/terminal.o FrameworkSource/terminal.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o: FrameworkSource/circular_buffer_no_modulo_threadsafe.c  .generated_files/flags/default/23b2156081bf1f544bcc291b39c127550a88a4f1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o.d" -o ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o FrameworkSource/circular_buffer_no_modulo_threadsafe.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/dbprintf.o: FrameworkSource/dbprintf.c  .generated_files/flags/default/783855fd47c49155a68e2aa21f9edc21dc50c2f .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/dbprintf.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/dbprintf.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/dbprintf.o.d" -o ${OBJECTDIR}/FrameworkSource/dbprintf.o FrameworkSource/dbprintf.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/EventCheckers.o: ProjectSource/EventCheckers.c  .generated_files/flags/default/e4c5381dec014ab7d2cfdceaa47c65efb891de9d .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/EventCheckers.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/EventCheckers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/EventCheckers.o.d" -o ${OBJECTDIR}/ProjectSource/EventCheckers.o ProjectSource/EventCheckers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/main.o: ProjectSource/main.c  .generated_files/flags/default/90b725ade96a1e2ef9ca9248e4003f2d92c6a515 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/main.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/main.o.d" -o ${OBJECTDIR}/ProjectSource/main.o ProjectSource/main.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o: ProjectSource/PIC32_IC_Lib.c  .generated_files/flags/default/2f769a9c562686a1253c243a7d93739827cd6f10 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o.d" -o ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o ProjectSource/PIC32_IC_Lib.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o: ProjectSource/PIC32_PWM_Lib.c  .generated_files/flags/default/8e6b3cfaed940921cf69f3f8fedefa61779837d7 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o.d" -o ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o ProjectSource/PIC32_PWM_Lib.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/DrivingService.o: ProjectSource/DrivingService.c  .generated_files/flags/default/1968bf736a91977700ee7f6317aa64b007641303 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/DrivingService.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/DrivingService.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/DrivingService.o.d" -o ${OBJECTDIR}/ProjectSource/DrivingService.o ProjectSource/DrivingService.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/CommunicationService.o: ProjectSource/CommunicationService.c  .generated_files/flags/default/c382d07861fdf3f37eace222004c145931217e31 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/CommunicationService.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/CommunicationService.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/CommunicationService.o.d" -o ${OBJECTDIR}/ProjectSource/CommunicationService.o ProjectSource/CommunicationService.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PairedServoService.o: ProjectSource/PairedServoService.c  .generated_files/flags/default/88a48cf4c5a482bdf8b97d263b98c0164026451a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PairedServoService.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PairedServoService.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PairedServoService.o.d" -o ${OBJECTDIR}/ProjectSource/PairedServoService.o ProjectSource/PairedServoService.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/boatFramework.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g   -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/boatFramework.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)   -mreserve=data@0x0:0x1FC -mreserve=boot@0x1FC00490:0x1FC00BEF  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=_min_heap_size=256,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}"
	
else
${DISTDIR}/boatFramework.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/boatFramework.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=256,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}"
	${MP_CC_DIR}\\xc32-bin2hex ${DISTDIR}/boatFramework.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
