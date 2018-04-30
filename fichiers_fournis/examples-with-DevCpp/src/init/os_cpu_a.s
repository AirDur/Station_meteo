#------------------------------------------------------------------------------
#- File: os_cpu_a.s
#------------------------------------------------------------------------------
#-            (c) Copyright ARM Limited 1999.  All rights reserved. 
#-
#-                              ARM Specific code
#------------------------------------------------------------------------------
#-
#- 
#-	Functions defined in this module:
#-
#-	void ARMDisableInt(void)	disable interrupts when in SVC
#-	void ARMEnableInt(void)		enable interrupts when in SVC


#	AREA	|subr|, CODE, READONLY

#	void DisableInt(void)
#	void EnableInt(void)
#
#	Disable and enable IRQ and FIQ preserving current CPU mode.
#
	.GLOBAL	ARMDisableInt
ARMDisableInt:
	STMDB	sp!, {r0}
	MRS		r0, CPSR
	ORR		r0, r0, #NoInt
	MSR		CPSR_cxsf, r0
	LDMIA	sp!, {r0}
	MOV	pc, lr


	.GLOBAL	ARMEnableInt
ARMEnableInt:
	STMDB	sp!, {r0}
	MRS	r0, CPSR
	BIC	r0, r0, #NoInt
	MSR	CPSR_cxsf, r0
	LDMIA	sp!, {r0}
	MOV	pc, lr


	.GLOBAL	ARMIsDisableInt
ARMIsDisableInt:		@return value [disable: 1      enable: 0]
	MRS	a1, CPSR
	AND	a1, a1, #NoInt
	MOV	pc, lr		



	.END
