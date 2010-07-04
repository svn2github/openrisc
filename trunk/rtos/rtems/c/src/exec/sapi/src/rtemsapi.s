# file rtemsapi.c
.file	"rtemsapi.c"
gcc2_compiled.:
___gnu_compiled_c:
.text
	.def	_proc_ptr
	.scl	13
	.type	021
	.endef
	.def	_unsigned8
	.scl	13
	.type	014
	.endef
	.def	_unsigned16
	.scl	13
	.type	015
	.endef
	.def	_unsigned32
	.scl	13
	.type	016
	.endef
	.def	_unsigned64
	.scl	13
	.type	017
	.endef
	.def	_Priority_Bit_map_control
	.scl	13
	.type	015
	.endef
	.def	_signed8
	.scl	13
	.type	02
	.endef
	.def	_signed16
	.scl	13
	.type	03
	.endef
	.def	_signed32
	.scl	13
	.type	04
	.endef
	.def	_signed64
	.scl	13
	.type	05
	.endef
	.def	_boolean
	.scl	13
	.type	016
	.endef
	.def	_single_precision
	.scl	13
	.type	06
	.endef
	.def	_double_precision
	.scl	13
	.type	07
	.endef
	.def	_no_cpu_isr
	.scl	13
	.type	01
	.endef
	.def	_no_cpu_isr_entry
	.scl	13
	.type	0221
	.endef
	.def	_.0fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_SR_SUPV
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_SR_EXR
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_SR_EIR
	.val	4
	.scl	16
	.type	013
	.endef
	.def	_SR_DCE
	.val	8
	.scl	16
	.type	013
	.endef
	.def	_SR_ICE
	.val	16
	.scl	16
	.type	013
	.endef
	.def	_SR_DME
	.val	32
	.scl	16
	.type	013
	.endef
	.def	_SR_IME
	.val	64
	.scl	16
	.type	013
	.endef
	.def	_SR_LEE
	.val	128
	.scl	16
	.type	013
	.endef
	.def	_SR_CE
	.val	256
	.scl	16
	.type	013
	.endef
	.def	_SR_F
	.val	512
	.scl	16
	.type	013
	.endef
	.def	_SR_CY
	.val	1024
	.scl	16
	.type	013
	.endef
	.def	_SR_OV
	.val	2048
	.scl	16
	.type	013
	.endef
	.def	_SR_OVE
	.val	4096
	.scl	16
	.type	013
	.endef
	.def	_SR_DSX
	.val	8192
	.scl	16
	.type	013
	.endef
	.def	_SR_EP
	.val	16384
	.scl	16
	.type	013
	.endef
	.def	_SR_PXR
	.val	32768
	.scl	16
	.type	013
	.endef
	.def	_SR_SUMRA
	.val	65536
	.scl	16
	.type	013
	.endef
	.def	_SR_CID
	.val	-134217728
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.0fake
	.size	4
	.endef
	.def	_StatusRegisterBits
	.scl	13
	.tag	_.0fake
	.size	4
	.type	012
	.endef
	.def	_.1fake
	.scl	10
	.type	010
	.size	144
	.endef
	.def	_sr
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_esr
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_ear
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_epc
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_r
	.val	16
	.scl	8
	.dim	31
	.size	124
	.type	076
	.endef
	.def	_pc
	.val	140
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	144
	.scl	102
	.tag	_.1fake
	.size	144
	.endef
	.def	_Context_Control
	.scl	13
	.tag	_.1fake
	.size	144
	.type	010
	.endef
	.def	_Context_Control_fp
	.scl	13
	.type	04
	.endef
	.def	_CPU_Interrupt_frame
	.scl	13
	.tag	_.1fake
	.size	144
	.type	010
	.endef
	.def	_.2fake
	.scl	10
	.type	010
	.size	40
	.endef
	.def	_pretasking_hook
	.val	0
	.scl	8
	.type	0221
	.endef
	.def	_predriver_hook
	.val	4
	.scl	8
	.type	0221
	.endef
	.def	_postdriver_hook
	.val	8
	.scl	8
	.type	0221
	.endef
	.def	_idle_task
	.val	12
	.scl	8
	.type	0221
	.endef
	.def	_do_zero_of_workspace
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_idle_task_stack_size
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_interrupt_stack_size
	.val	24
	.scl	8
	.type	016
	.endef
	.def	_extra_mpci_receive_server_stack
	.val	28
	.scl	8
	.type	016
	.endef
	.def	_stack_allocate_hook
	.val	32
	.scl	8
	.type	0621
	.endef
	.def	_stack_free_hook
	.val	36
	.scl	8
	.type	0221
	.endef
	.def	.eos
	.val	40
	.scl	102
	.tag	_.2fake
	.size	40
	.endef
	.def	_rtems_cpu_table
	.scl	13
	.tag	_.2fake
	.size	40
	.type	010
	.endef
	.def	_.3fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_RTEMS_SUCCESSFUL
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_TASK_EXITTED
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_MP_NOT_CONFIGURED
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_INVALID_NAME
	.val	3
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_INVALID_ID
	.val	4
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_TOO_MANY
	.val	5
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_TIMEOUT
	.val	6
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_OBJECT_WAS_DELETED
	.val	7
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_INVALID_SIZE
	.val	8
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_INVALID_ADDRESS
	.val	9
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_INVALID_NUMBER
	.val	10
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_NOT_DEFINED
	.val	11
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_RESOURCE_IN_USE
	.val	12
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_UNSATISFIED
	.val	13
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_INCORRECT_STATE
	.val	14
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_ALREADY_SUSPENDED
	.val	15
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_ILLEGAL_ON_SELF
	.val	16
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_ILLEGAL_ON_REMOTE_OBJECT
	.val	17
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_CALLED_FROM_ISR
	.val	18
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_INVALID_PRIORITY
	.val	19
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_INVALID_CLOCK
	.val	20
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_INVALID_NODE
	.val	21
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_NOT_CONFIGURED
	.val	22
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_NOT_OWNER_OF_RESOURCE
	.val	23
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_NOT_IMPLEMENTED
	.val	24
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_INTERNAL_ERROR
	.val	25
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_NO_MEMORY
	.val	26
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_IO_ERROR
	.val	27
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_PROXY_BLOCKING
	.val	28
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.3fake
	.size	4
	.endef
	.def	_rtems_status_code
	.scl	13
	.tag	_.3fake
	.size	4
	.type	012
	.endef
	.global __Status_Object_name_errors_to_status
.data
	.align 4
__Status_Object_name_errors_to_status:
	.word 0
	.word 3
	.word 21
.text
	.def	_Chain_Node
	.scl	13
	.type	010
	.endef
	.def	_Chain_Node_struct
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_next
	.val	0
	.scl	8
	.tag	_Chain_Node_struct
	.size	8
	.type	030
	.endef
	.def	_previous
	.val	4
	.scl	8
	.tag	_Chain_Node_struct
	.size	8
	.type	030
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_Chain_Node_struct
	.size	8
	.endef
	.def	_.4fake
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_first
	.val	0
	.scl	8
	.tag	_Chain_Node_struct
	.size	8
	.type	030
	.endef
	.def	_permanent_null
	.val	4
	.scl	8
	.tag	_Chain_Node_struct
	.size	8
	.type	030
	.endef
	.def	_last
	.val	8
	.scl	8
	.tag	_Chain_Node_struct
	.size	8
	.type	030
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_.4fake
	.size	12
	.endef
	.def	_Chain_Control
	.scl	13
	.tag	_.4fake
	.size	12
	.type	010
	.endef
	.def	_Objects_Name
	.scl	13
	.type	021
	.endef
	.def	_Objects_Name_comparators
	.scl	13
	.type	0236
	.endef
	.def	_Objects_Id
	.scl	13
	.type	016
	.endef
	.def	_.5fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_OBJECTS_NO_CLASS
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_INTERNAL_THREADS
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_RTEMS_TASKS
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_POSIX_THREADS
	.val	3
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_ITRON_TASKS
	.val	4
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_RTEMS_TIMERS
	.val	5
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_RTEMS_SEMAPHORES
	.val	6
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_RTEMS_MESSAGE_QUEUES
	.val	7
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_RTEMS_PARTITIONS
	.val	8
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_RTEMS_REGIONS
	.val	9
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_RTEMS_PORTS
	.val	10
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_RTEMS_PERIODS
	.val	11
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_RTEMS_EXTENSIONS
	.val	12
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_POSIX_KEYS
	.val	13
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_POSIX_INTERRUPTS
	.val	14
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_POSIX_MESSAGE_QUEUES
	.val	15
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_POSIX_MUTEXES
	.val	16
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_POSIX_SEMAPHORES
	.val	17
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_POSIX_CONDITION_VARIABLES
	.val	18
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_ITRON_EVENTFLAGS
	.val	19
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_ITRON_MAILBOXES
	.val	20
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_ITRON_MESSAGE_BUFFERS
	.val	21
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_ITRON_PORTS
	.val	22
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_ITRON_SEMAPHORES
	.val	23
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_ITRON_VARIABLE_MEMORY_POOLS
	.val	24
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_ITRON_FIXED_MEMORY_POOLS
	.val	25
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.5fake
	.size	4
	.endef
	.def	_Objects_Classes
	.scl	13
	.tag	_.5fake
	.size	4
	.type	012
	.endef
	.def	_.6fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_OBJECTS_LOCAL
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_REMOTE
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_ERROR
	.val	2
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.6fake
	.size	4
	.endef
	.def	_Objects_Locations
	.scl	13
	.tag	_.6fake
	.size	4
	.type	012
	.endef
	.def	_.7fake
	.scl	10
	.type	010
	.size	16
	.endef
	.def	_Node
	.val	0
	.scl	8
	.tag	_Chain_Node_struct
	.size	8
	.type	010
	.endef
	.def	_id
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_name
	.val	12
	.scl	8
	.type	021
	.endef
	.def	.eos
	.val	16
	.scl	102
	.tag	_.7fake
	.size	16
	.endef
	.def	_Objects_Control
	.scl	13
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_.8fake
	.scl	10
	.type	010
	.size	76
	.endef
	.def	_the_class
	.val	0
	.scl	8
	.tag	_.5fake
	.size	4
	.type	012
	.endef
	.def	_minimum_id
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_maximum_id
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_maximum
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_auto_extend
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_allocation_size
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_size
	.val	24
	.scl	8
	.type	016
	.endef
	.def	_local_table
	.val	28
	.scl	8
	.tag	_.7fake
	.size	16
	.type	0130
	.endef
	.def	_name_table
	.val	32
	.scl	8
	.type	0121
	.endef
	.def	_global_table
	.val	36
	.scl	8
	.tag	_.4fake
	.size	12
	.type	030
	.endef
	.def	_Inactive
	.val	40
	.scl	8
	.tag	_.4fake
	.size	12
	.type	010
	.endef
	.def	_inactive
	.val	52
	.scl	8
	.type	016
	.endef
	.def	_inactive_per_block
	.val	56
	.scl	8
	.type	036
	.endef
	.def	_object_blocks
	.val	60
	.scl	8
	.type	0121
	.endef
	.def	_is_string
	.val	64
	.scl	8
	.type	016
	.endef
	.def	_name_length
	.val	68
	.scl	8
	.type	016
	.endef
	.def	_is_thread
	.val	72
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	76
	.scl	102
	.tag	_.8fake
	.size	76
	.endef
	.def	_Objects_Information
	.scl	13
	.tag	_.8fake
	.size	76
	.type	010
	.endef
	.def	_.9fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_OBJECTS_SUCCESSFUL
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_INVALID_NAME
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_OBJECTS_INVALID_NODE
	.val	2
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.9fake
	.size	4
	.endef
	.def	_Objects_Name_to_id_errors
	.scl	13
	.tag	_.9fake
	.size	4
	.type	012
	.endef
	.def	_.10fake
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_thread_entry
	.val	0
	.scl	8
	.type	0621
	.endef
	.def	_stack_size
	.val	4
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_.10fake
	.size	8
	.endef
	.def	_posix_initialization_threads_table
	.scl	13
	.tag	_.10fake
	.size	8
	.type	010
	.endef
	.def	_.11fake
	.scl	10
	.type	010
	.size	40
	.endef
	.def	_maximum_threads
	.val	0
	.scl	8
	.type	04
	.endef
	.def	_maximum_mutexes
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_maximum_condition_variables
	.val	8
	.scl	8
	.type	04
	.endef
	.def	_maximum_keys
	.val	12
	.scl	8
	.type	04
	.endef
	.def	_maximum_timers
	.val	16
	.scl	8
	.type	04
	.endef
	.def	_maximum_queued_signals
	.val	20
	.scl	8
	.type	04
	.endef
	.def	_maximum_message_queues
	.val	24
	.scl	8
	.type	04
	.endef
	.def	_maximum_semaphores
	.val	28
	.scl	8
	.type	04
	.endef
	.def	_number_of_initialization_threads
	.val	32
	.scl	8
	.type	04
	.endef
	.def	_User_initialization_threads_table
	.val	36
	.scl	8
	.tag	_.10fake
	.size	8
	.type	030
	.endef
	.def	.eos
	.val	40
	.scl	102
	.tag	_.11fake
	.size	40
	.endef
	.def	_posix_api_configuration_table
	.scl	13
	.tag	_.11fake
	.size	40
	.type	010
	.endef
	.def	_B
	.scl	13
	.type	02
	.endef
	.def	_H
	.scl	13
	.type	03
	.endef
	.def	_W
	.scl	13
	.type	04
	.endef
	.def	_UB
	.scl	13
	.type	014
	.endef
	.def	_UH
	.scl	13
	.type	015
	.endef
	.def	_UW
	.scl	13
	.type	016
	.endef
	.def	_VW
	.scl	13
	.type	016
	.endef
	.def	_VH
	.scl	13
	.type	015
	.endef
	.def	_VB
	.scl	13
	.type	014
	.endef
	.def	_VP
	.scl	13
	.type	021
	.endef
	.def	_FP
	.scl	13
	.type	0221
	.endef
	.def	_INT
	.scl	13
	.type	04
	.endef
	.def	_UINT
	.scl	13
	.type	016
	.endef
	.def	_BOOL
	.scl	13
	.type	016
	.endef
	.def	_FN
	.scl	13
	.type	03
	.endef
	.def	_ID
	.scl	13
	.type	04
	.endef
	.def	_BOOL_ID
	.scl	13
	.type	016
	.endef
	.def	_HNO
	.scl	13
	.type	04
	.endef
	.def	_RNO
	.scl	13
	.type	04
	.endef
	.def	_NODE
	.scl	13
	.type	04
	.endef
	.def	_ATR
	.scl	13
	.type	04
	.endef
	.def	_ER
	.scl	13
	.type	04
	.endef
	.def	_PRI
	.scl	13
	.type	016
	.endef
	.def	_TMO
	.scl	13
	.type	04
	.endef
	.def	_t_cflg
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_flgatr
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_iflgptn
	.val	8
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_t_cflg
	.size	12
	.endef
	.def	_T_CFLG
	.scl	13
	.tag	_t_cflg
	.size	12
	.type	010
	.endef
	.def	_t_rflg
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_wtsk
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_flgptn
	.val	8
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_t_rflg
	.size	12
	.endef
	.def	_T_RFLG
	.scl	13
	.tag	_t_rflg
	.size	12
	.type	010
	.endef
	.def	_t_cmpf
	.scl	10
	.type	010
	.size	16
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_mpfatr
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_mpfcnt
	.val	8
	.scl	8
	.type	04
	.endef
	.def	_blfsz
	.val	12
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	16
	.scl	102
	.tag	_t_cmpf
	.size	16
	.endef
	.def	_T_CMPF
	.scl	13
	.tag	_t_cmpf
	.size	16
	.type	010
	.endef
	.def	_t_rmpf
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_wtsk
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_frbcnt
	.val	8
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_t_rmpf
	.size	12
	.endef
	.def	_T_RMPF
	.scl	13
	.tag	_t_rmpf
	.size	12
	.type	010
	.endef
	.def	_t_dint
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_intatr
	.val	0
	.scl	8
	.type	04
	.endef
	.def	_inthdr
	.val	4
	.scl	8
	.type	0221
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_t_dint
	.size	8
	.endef
	.def	_T_DINT
	.scl	13
	.tag	_t_dint
	.size	8
	.type	010
	.endef
	.def	_t_cmbx
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_mbxatr
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_bufcnt
	.val	8
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_t_cmbx
	.size	12
	.endef
	.def	_T_CMBX
	.scl	13
	.tag	_t_cmbx
	.size	12
	.type	010
	.endef
	.def	_t_msg
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_msgpri
	.val	0
	.scl	8
	.type	04
	.endef
	.def	_msgcont
	.val	4
	.scl	8
	.dim	1
	.size	1
	.type	074
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_t_msg
	.size	8
	.endef
	.def	_T_MSG
	.scl	13
	.tag	_t_msg
	.size	8
	.type	010
	.endef
	.def	_t_rmbx
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_wtsk
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_pk_msg
	.val	8
	.scl	8
	.tag	_t_msg
	.size	8
	.type	030
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_t_rmbx
	.size	12
	.endef
	.def	_T_RMBX
	.scl	13
	.tag	_t_rmbx
	.size	12
	.type	010
	.endef
	.def	_t_cmbf
	.scl	10
	.type	010
	.size	16
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_mbfatr
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_bufsz
	.val	8
	.scl	8
	.type	04
	.endef
	.def	_maxmsz
	.val	12
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	16
	.scl	102
	.tag	_t_cmbf
	.size	16
	.endef
	.def	_T_CMBF
	.scl	13
	.tag	_t_cmbf
	.size	16
	.type	010
	.endef
	.def	_t_rmbf
	.scl	10
	.type	010
	.size	20
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_wtsk
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_stsk
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_msgsz
	.val	12
	.scl	8
	.type	04
	.endef
	.def	_frbufsz
	.val	16
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	20
	.scl	102
	.tag	_t_rmbf
	.size	20
	.endef
	.def	_T_RMBF
	.scl	13
	.tag	_t_rmbf
	.size	20
	.type	010
	.endef
	.def	_t_ver
	.scl	10
	.type	010
	.size	20
	.endef
	.def	_maker
	.val	0
	.scl	8
	.type	015
	.endef
	.def	_id
	.val	2
	.scl	8
	.type	015
	.endef
	.def	_spver
	.val	4
	.scl	8
	.type	015
	.endef
	.def	_prver
	.val	6
	.scl	8
	.type	015
	.endef
	.def	_prno
	.val	8
	.scl	8
	.dim	4
	.size	8
	.type	075
	.endef
	.def	_cpu
	.val	16
	.scl	8
	.type	015
	.endef
	.def	_var
	.val	18
	.scl	8
	.type	015
	.endef
	.def	.eos
	.val	20
	.scl	102
	.tag	_t_ver
	.size	20
	.endef
	.def	_T_VER
	.scl	13
	.tag	_t_ver
	.size	20
	.type	010
	.endef
	.def	_t_rsys
	.scl	10
	.type	010
	.size	4
	.endef
	.def	_sysstat
	.val	0
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_t_rsys
	.size	4
	.endef
	.def	_T_RSYS
	.scl	13
	.tag	_t_rsys
	.size	4
	.type	010
	.endef
	.def	_t_rcfg
	.scl	10
	.type	010
	.size	0
	.endef
	.def	.eos
	.val	0
	.scl	102
	.tag	_t_rcfg
	.size	0
	.endef
	.def	_T_RCFG
	.scl	13
	.tag	_t_rcfg
	.size	0
	.type	010
	.endef
	.def	_t_dsvc
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_svcatr
	.val	0
	.scl	8
	.type	04
	.endef
	.def	_svchdr
	.val	4
	.scl	8
	.type	0221
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_t_dsvc
	.size	8
	.endef
	.def	_T_DSVC
	.scl	13
	.tag	_t_dsvc
	.size	8
	.type	010
	.endef
	.def	_t_dexc
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_excatr
	.val	0
	.scl	8
	.type	04
	.endef
	.def	_exchdr
	.val	4
	.scl	8
	.type	0221
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_t_dexc
	.size	8
	.endef
	.def	_T_DEXC
	.scl	13
	.tag	_t_dexc
	.size	8
	.type	010
	.endef
	.def	_t_cpor
	.scl	10
	.type	010
	.size	16
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_poratr
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_maxcmsz
	.val	8
	.scl	8
	.type	04
	.endef
	.def	_maxrmsz
	.val	12
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	16
	.scl	102
	.tag	_t_cpor
	.size	16
	.endef
	.def	_T_CPOR
	.scl	13
	.tag	_t_cpor
	.size	16
	.type	010
	.endef
	.def	_t_rpor
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_wtsk
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_atsk
	.val	8
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_t_rpor
	.size	12
	.endef
	.def	_T_RPOR
	.scl	13
	.tag	_t_rpor
	.size	12
	.type	010
	.endef
	.def	_t_csem
	.scl	10
	.type	010
	.size	16
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_sematr
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_isemcnt
	.val	8
	.scl	8
	.type	04
	.endef
	.def	_maxsem
	.val	12
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	16
	.scl	102
	.tag	_t_csem
	.size	16
	.endef
	.def	_T_CSEM
	.scl	13
	.tag	_t_csem
	.size	16
	.type	010
	.endef
	.def	_t_rsem
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_wtsk
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_semcnt
	.val	8
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_t_rsem
	.size	12
	.endef
	.def	_T_RSEM
	.scl	13
	.tag	_t_rsem
	.size	12
	.type	010
	.endef
	.def	_t_ctsk
	.scl	10
	.type	010
	.size	20
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_tskatr
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_task
	.val	8
	.scl	8
	.type	0221
	.endef
	.def	_itskpri
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_stksz
	.val	16
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	20
	.scl	102
	.tag	_t_ctsk
	.size	20
	.endef
	.def	_T_CTSK
	.scl	13
	.tag	_t_ctsk
	.size	20
	.type	010
	.endef
	.def	_t_rtsk
	.scl	10
	.type	010
	.size	44
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_tskpri
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_tskstat
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_tskwait
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_wid
	.val	16
	.scl	8
	.type	04
	.endef
	.def	_wupcnt
	.val	20
	.scl	8
	.type	04
	.endef
	.def	_suscnt
	.val	24
	.scl	8
	.type	04
	.endef
	.def	_tskatr
	.val	28
	.scl	8
	.type	04
	.endef
	.def	_task
	.val	32
	.scl	8
	.type	0221
	.endef
	.def	_itskpri
	.val	36
	.scl	8
	.type	016
	.endef
	.def	_stksz
	.val	40
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	44
	.scl	102
	.tag	_t_rtsk
	.size	44
	.endef
	.def	_T_RTSK
	.scl	13
	.tag	_t_rtsk
	.size	44
	.type	010
	.endef
	.def	_t_systime
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_utime
	.val	0
	.scl	8
	.type	03
	.endef
	.def	_ltime
	.val	4
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_t_systime
	.size	8
	.endef
	.def	_SYSTIME
	.scl	13
	.tag	_t_systime
	.size	8
	.type	010
	.endef
	.def	_CYCTIME
	.scl	13
	.tag	_t_systime
	.size	8
	.type	010
	.endef
	.def	_ALMTIME
	.scl	13
	.tag	_t_systime
	.size	8
	.type	010
	.endef
	.def	_DLYTIME
	.scl	13
	.type	04
	.endef
	.def	_t_dcyc
	.scl	10
	.type	010
	.size	24
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_cycatr
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_cychdr
	.val	8
	.scl	8
	.type	0221
	.endef
	.def	_cycact
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_cyctim
	.val	16
	.scl	8
	.tag	_t_systime
	.size	8
	.type	010
	.endef
	.def	.eos
	.val	24
	.scl	102
	.tag	_t_dcyc
	.size	24
	.endef
	.def	_T_DCYC
	.scl	13
	.tag	_t_dcyc
	.size	24
	.type	010
	.endef
	.def	_t_rcyc
	.scl	10
	.type	010
	.size	16
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_lfttim
	.val	4
	.scl	8
	.tag	_t_systime
	.size	8
	.type	010
	.endef
	.def	_cycact
	.val	12
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	16
	.scl	102
	.tag	_t_rcyc
	.size	16
	.endef
	.def	_T_RCYC
	.scl	13
	.tag	_t_rcyc
	.size	16
	.type	010
	.endef
	.def	_t_dalm
	.scl	10
	.type	010
	.size	24
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_almatr
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_almhdr
	.val	8
	.scl	8
	.type	0221
	.endef
	.def	_tmmode
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_almtim
	.val	16
	.scl	8
	.tag	_t_systime
	.size	8
	.type	010
	.endef
	.def	.eos
	.val	24
	.scl	102
	.tag	_t_dalm
	.size	24
	.endef
	.def	_T_DALM
	.scl	13
	.tag	_t_dalm
	.size	24
	.type	010
	.endef
	.def	_t_ralm
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_lfttim
	.val	4
	.scl	8
	.tag	_t_systime
	.size	8
	.type	010
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_t_ralm
	.size	12
	.endef
	.def	_T_RALM
	.scl	13
	.tag	_t_ralm
	.size	12
	.type	010
	.endef
	.def	_t_cmpl
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_mplatr
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_mplsz
	.val	8
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_t_cmpl
	.size	12
	.endef
	.def	_T_CMPL
	.scl	13
	.tag	_t_cmpl
	.size	12
	.type	010
	.endef
	.def	_t_rmpl
	.scl	10
	.type	010
	.size	16
	.endef
	.def	_exinf
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_wtsk
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_frsz
	.val	8
	.scl	8
	.type	04
	.endef
	.def	_maxsz
	.val	12
	.scl	8
	.type	04
	.endef
	.def	.eos
	.val	16
	.scl	102
	.tag	_t_rmpl
	.size	16
	.endef
	.def	_T_RMPL
	.scl	13
	.tag	_t_rmpl
	.size	16
	.type	010
	.endef
	.def	_.12fake
	.scl	10
	.type	010
	.size	24
	.endef
	.def	_id
	.val	0
	.scl	8
	.type	04
	.endef
	.def	_attributes
	.val	4
	.scl	8
	.tag	_t_ctsk
	.size	20
	.type	010
	.endef
	.def	.eos
	.val	24
	.scl	102
	.tag	_.12fake
	.size	24
	.endef
	.def	_itron_initialization_tasks_table
	.scl	13
	.tag	_.12fake
	.size	24
	.type	010
	.endef
	.def	_.13fake
	.scl	10
	.type	010
	.size	40
	.endef
	.def	_maximum_tasks
	.val	0
	.scl	8
	.type	04
	.endef
	.def	_maximum_semaphores
	.val	4
	.scl	8
	.type	04
	.endef
	.def	_maximum_eventflags
	.val	8
	.scl	8
	.type	04
	.endef
	.def	_maximum_mailboxes
	.val	12
	.scl	8
	.type	04
	.endef
	.def	_maximum_message_buffers
	.val	16
	.scl	8
	.type	04
	.endef
	.def	_maximum_ports
	.val	20
	.scl	8
	.type	04
	.endef
	.def	_maximum_memory_pools
	.val	24
	.scl	8
	.type	04
	.endef
	.def	_maximum_fixed_memory_pools
	.val	28
	.scl	8
	.type	04
	.endef
	.def	_number_of_initialization_tasks
	.val	32
	.scl	8
	.type	04
	.endef
	.def	_User_initialization_tasks_table
	.val	36
	.scl	8
	.tag	_.12fake
	.size	24
	.type	030
	.endef
	.def	.eos
	.val	40
	.scl	102
	.tag	_.13fake
	.size	40
	.endef
	.def	_itron_api_configuration_table
	.scl	13
	.tag	_.13fake
	.size	40
	.type	010
	.endef
	.def	_Priority_Control
	.scl	13
	.type	016
	.endef
	.def	_.14fake
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_minor
	.val	0
	.scl	8
	.type	035
	.endef
	.def	_ready_major
	.val	4
	.scl	8
	.type	015
	.endef
	.def	_ready_minor
	.val	6
	.scl	8
	.type	015
	.endef
	.def	_block_major
	.val	8
	.scl	8
	.type	015
	.endef
	.def	_block_minor
	.val	10
	.scl	8
	.type	015
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_.14fake
	.size	12
	.endef
	.def	_Priority_Information
	.scl	13
	.tag	_.14fake
	.size	12
	.type	010
	.endef
	.def	_Watchdog_Interval
	.scl	13
	.type	016
	.endef
	.def	_Watchdog_Service_routine
	.scl	13
	.type	01
	.endef
	.def	_Watchdog_Service_routine_entry
	.scl	13
	.type	0221
	.endef
	.def	_.15fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_WATCHDOG_INACTIVE
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_WATCHDOG_BEING_INSERTED
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_WATCHDOG_ACTIVE
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_WATCHDOG_REMOVE_IT
	.val	3
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.15fake
	.size	4
	.endef
	.def	_Watchdog_States
	.scl	13
	.tag	_.15fake
	.size	4
	.type	012
	.endef
	.def	_.16fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_WATCHDOG_FORWARD
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_WATCHDOG_BACKWARD
	.val	1
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.16fake
	.size	4
	.endef
	.def	_Watchdog_Adjust_directions
	.scl	13
	.tag	_.16fake
	.size	4
	.type	012
	.endef
	.def	_.17fake
	.scl	10
	.type	010
	.size	40
	.endef
	.def	_Node
	.val	0
	.scl	8
	.tag	_Chain_Node_struct
	.size	8
	.type	010
	.endef
	.def	_state
	.val	8
	.scl	8
	.tag	_.15fake
	.size	4
	.type	012
	.endef
	.def	_initial
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_delta_interval
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_start_time
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_stop_time
	.val	24
	.scl	8
	.type	016
	.endef
	.def	_routine
	.val	28
	.scl	8
	.type	0221
	.endef
	.def	_id
	.val	32
	.scl	8
	.type	016
	.endef
	.def	_user_data
	.val	36
	.scl	8
	.type	021
	.endef
	.def	.eos
	.val	40
	.scl	102
	.tag	_.17fake
	.size	40
	.endef
	.def	_Watchdog_Control
	.scl	13
	.tag	_.17fake
	.size	40
	.type	010
	.endef
	.def	_.18fake
	.scl	10
	.type	010
	.size	28
	.endef
	.def	_year
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_month
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_day
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_hour
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_minute
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_second
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_ticks
	.val	24
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	28
	.scl	102
	.tag	_.18fake
	.size	28
	.endef
	.def	_TOD_Control
	.scl	13
	.tag	_.18fake
	.size	28
	.type	010
	.endef
	.def	_ISR_Level
	.scl	13
	.type	016
	.endef
	.def	_ISR_Vector_number
	.scl	13
	.type	016
	.endef
	.def	_ISR_Handler
	.scl	13
	.type	01
	.endef
	.def	_ISR_Handler_entry
	.scl	13
	.type	0221
	.endef
	.def	_Modes_Control
	.scl	13
	.type	016
	.endef
	.def	_rtems_unsigned8
	.scl	13
	.type	014
	.endef
	.def	_rtems_unsigned16
	.scl	13
	.type	015
	.endef
	.def	_rtems_unsigned32
	.scl	13
	.type	016
	.endef
	.def	_rtems_signed8
	.scl	13
	.type	02
	.endef
	.def	_rtems_signed16
	.scl	13
	.type	03
	.endef
	.def	_rtems_signed32
	.scl	13
	.type	04
	.endef
	.def	_rtems_unsigned64
	.scl	13
	.type	017
	.endef
	.def	_rtems_signed64
	.scl	13
	.type	05
	.endef
	.def	_rtems_single
	.scl	13
	.type	06
	.endef
	.def	_rtems_double
	.scl	13
	.type	07
	.endef
	.def	_rtems_boolean
	.scl	13
	.type	016
	.endef
	.def	_rtems_name
	.scl	13
	.type	016
	.endef
	.def	_rtems_id
	.scl	13
	.type	016
	.endef
	.def	_rtems_context
	.scl	13
	.tag	_.1fake
	.size	144
	.type	010
	.endef
	.def	_rtems_context_fp
	.scl	13
	.type	04
	.endef
	.def	_rtems_interrupt_frame
	.scl	13
	.tag	_.1fake
	.size	144
	.type	010
	.endef
	.def	_rtems_interval
	.scl	13
	.type	016
	.endef
	.def	_rtems_time_of_day
	.scl	13
	.tag	_.18fake
	.size	28
	.type	010
	.endef
	.def	_rtems_mode
	.scl	13
	.type	016
	.endef
	.def	_States_Control
	.scl	13
	.type	016
	.endef
	.def	_.19fake
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_size
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_area
	.val	4
	.scl	8
	.type	021
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_.19fake
	.size	8
	.endef
	.def	_Stack_Control
	.scl	13
	.tag	_.19fake
	.size	8
	.type	010
	.endef
	.def	_.20fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_THREAD_QUEUE_DISCIPLINE_FIFO
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_THREAD_QUEUE_DISCIPLINE_PRIORITY
	.val	1
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.20fake
	.size	4
	.endef
	.def	_Thread_queue_Disciplines
	.scl	13
	.tag	_.20fake
	.size	4
	.type	012
	.endef
	.def	_.21fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_THREAD_QUEUE_SYNCHRONIZED
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_THREAD_QUEUE_NOTHING_HAPPENED
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_THREAD_QUEUE_TIMEOUT
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_THREAD_QUEUE_SATISFIED
	.val	3
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.21fake
	.size	4
	.endef
	.def	_Thread_queue_States
	.scl	13
	.tag	_.21fake
	.size	4
	.type	012
	.endef
	.def	_.22fake
	.scl	12
	.type	011
	.size	48
	.endef
	.def	_Fifo
	.val	0
	.scl	11
	.tag	_.4fake
	.size	12
	.type	010
	.endef
	.def	_Priority
	.val	0
	.scl	11
	.tag	_.4fake
	.dim	4
	.size	48
	.type	070
	.endef
	.def	.eos
	.val	48
	.scl	102
	.tag	_.22fake
	.size	48
	.endef
	.def	_.23fake
	.scl	10
	.type	010
	.size	64
	.endef
	.def	_Queues
	.val	0
	.scl	8
	.tag	_.22fake
	.size	48
	.type	011
	.endef
	.def	_sync_state
	.val	48
	.scl	8
	.tag	_.21fake
	.size	4
	.type	012
	.endef
	.def	_discipline
	.val	52
	.scl	8
	.tag	_.20fake
	.size	4
	.type	012
	.endef
	.def	_state
	.val	56
	.scl	8
	.type	016
	.endef
	.def	_timeout_status
	.val	60
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	64
	.scl	102
	.tag	_.23fake
	.size	64
	.endef
	.def	_Thread_queue_Control
	.scl	13
	.tag	_.23fake
	.size	64
	.type	010
	.endef
	.def	_Thread
	.scl	13
	.type	021
	.endef
	.def	_.24fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_THREAD_START_NUMERIC
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_THREAD_START_POINTER
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_THREAD_START_BOTH_POINTER_FIRST
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_THREAD_START_BOTH_NUMERIC_FIRST
	.val	3
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.24fake
	.size	4
	.endef
	.def	_Thread_Start_types
	.scl	13
	.tag	_.24fake
	.size	4
	.type	012
	.endef
	.def	_Thread_Entry
	.scl	13
	.type	0621
	.endef
	.def	_Thread_Entry_numeric
	.scl	13
	.type	0621
	.endef
	.def	_Thread_Entry_pointer
	.scl	13
	.type	0621
	.endef
	.def	_Thread_Entry_both_pointer_first
	.scl	13
	.type	0621
	.endef
	.def	_Thread_Entry_both_numeric_first
	.scl	13
	.type	0621
	.endef
	.def	_.25fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_THREAD_CPU_BUDGET_ALGORITHM_NONE
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_THREAD_CPU_BUDGET_ALGORITHM_CALLOUT
	.val	3
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.25fake
	.size	4
	.endef
	.def	_Thread_CPU_budget_algorithms
	.scl	13
	.tag	_.25fake
	.size	4
	.type	012
	.endef
	.def	_Thread_Control
	.scl	13
	.type	010
	.endef
	.def	_Thread_CPU_budget_algorithm_callout
	.scl	13
	.type	0221
	.endef
	.def	_rtems_task_variable_tt
	.scl	10
	.type	010
	.size	20
	.endef
	.def	_next
	.val	0
	.scl	8
	.tag	_rtems_task_variable_tt
	.size	20
	.type	030
	.endef
	.def	_ptr
	.val	4
	.scl	8
	.type	0121
	.endef
	.def	_gval
	.val	8
	.scl	8
	.type	021
	.endef
	.def	_tval
	.val	12
	.scl	8
	.type	021
	.endef
	.def	_dtor
	.val	16
	.scl	8
	.type	0221
	.endef
	.def	.eos
	.val	20
	.scl	102
	.tag	_rtems_task_variable_tt
	.size	20
	.endef
	.def	_rtems_task_variable_t
	.scl	13
	.tag	_rtems_task_variable_tt
	.size	20
	.type	010
	.endef
	.def	_.26fake
	.scl	10
	.type	010
	.size	56
	.endef
	.def	_entry_point
	.val	0
	.scl	8
	.type	0621
	.endef
	.def	_prototype
	.val	4
	.scl	8
	.tag	_.24fake
	.size	4
	.type	012
	.endef
	.def	_pointer_argument
	.val	8
	.scl	8
	.type	021
	.endef
	.def	_numeric_argument
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_is_preemptible
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_budget_algorithm
	.val	20
	.scl	8
	.tag	_.25fake
	.size	4
	.type	012
	.endef
	.def	_budget_callout
	.val	24
	.scl	8
	.type	0221
	.endef
	.def	_isr_level
	.val	28
	.scl	8
	.type	016
	.endef
	.def	_initial_priority
	.val	32
	.scl	8
	.type	016
	.endef
	.def	_core_allocated_stack
	.val	36
	.scl	8
	.type	016
	.endef
	.def	_Initial_stack
	.val	40
	.scl	8
	.tag	_.19fake
	.size	8
	.type	010
	.endef
	.def	_fp_context
	.val	48
	.scl	8
	.type	021
	.endef
	.def	_stack
	.val	52
	.scl	8
	.type	021
	.endef
	.def	.eos
	.val	56
	.scl	102
	.tag	_.26fake
	.size	56
	.endef
	.def	_Thread_Start_information
	.scl	13
	.tag	_.26fake
	.size	56
	.type	010
	.endef
	.def	_.27fake
	.scl	10
	.type	010
	.size	40
	.endef
	.def	_id
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_count
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_return_argument
	.val	8
	.scl	8
	.type	021
	.endef
	.def	_return_argument_1
	.val	12
	.scl	8
	.type	021
	.endef
	.def	_option
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_return_code
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_Block2n
	.val	24
	.scl	8
	.tag	_.4fake
	.size	12
	.type	010
	.endef
	.def	_queue
	.val	36
	.scl	8
	.tag	_.23fake
	.size	64
	.type	030
	.endef
	.def	.eos
	.val	40
	.scl	102
	.tag	_.27fake
	.size	40
	.endef
	.def	_Thread_Wait_information
	.scl	13
	.tag	_.27fake
	.size	40
	.type	010
	.endef
	.def	_.28fake
	.scl	10
	.type	010
	.size	120
	.endef
	.def	_Object
	.val	0
	.scl	8
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_current_state
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_current_priority
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_real_priority
	.val	24
	.scl	8
	.type	016
	.endef
	.def	_resource_count
	.val	28
	.scl	8
	.type	016
	.endef
	.def	_Wait
	.val	32
	.scl	8
	.tag	_.27fake
	.size	40
	.type	010
	.endef
	.def	_Timer
	.val	72
	.scl	8
	.tag	_.17fake
	.size	40
	.type	010
	.endef
	.def	_Active
	.val	112
	.scl	8
	.tag	_Chain_Node_struct
	.size	8
	.type	010
	.endef
	.def	.eos
	.val	120
	.scl	102
	.tag	_.28fake
	.size	120
	.endef
	.def	_Thread_Proxy_control
	.scl	13
	.tag	_.28fake
	.size	120
	.type	010
	.endef
	.def	_.29fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_THREAD_API_RTEMS
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_THREAD_API_POSIX
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_THREAD_API_ITRON
	.val	2
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.29fake
	.size	4
	.endef
	.def	_Thread_APIs
	.scl	13
	.tag	_.29fake
	.size	4
	.type	012
	.endef
	.def	_Thread_Control_struct
	.scl	10
	.type	010
	.size	388
	.endef
	.def	_Object
	.val	0
	.scl	8
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_current_state
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_current_priority
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_real_priority
	.val	24
	.scl	8
	.type	016
	.endef
	.def	_resource_count
	.val	28
	.scl	8
	.type	016
	.endef
	.def	_Wait
	.val	32
	.scl	8
	.tag	_.27fake
	.size	40
	.type	010
	.endef
	.def	_Timer
	.val	72
	.scl	8
	.tag	_.17fake
	.size	40
	.type	010
	.endef
	.def	_suspend_count
	.val	112
	.scl	8
	.type	016
	.endef
	.def	_is_global
	.val	116
	.scl	8
	.type	016
	.endef
	.def	_do_post_task_switch_extension
	.val	120
	.scl	8
	.type	016
	.endef
	.def	_is_preemptible
	.val	124
	.scl	8
	.type	016
	.endef
	.def	_rtems_ada_self
	.val	128
	.scl	8
	.type	021
	.endef
	.def	_cpu_time_budget
	.val	132
	.scl	8
	.type	016
	.endef
	.def	_budget_algorithm
	.val	136
	.scl	8
	.tag	_.25fake
	.size	4
	.type	012
	.endef
	.def	_budget_callout
	.val	140
	.scl	8
	.type	0221
	.endef
	.def	_ticks_executed
	.val	144
	.scl	8
	.type	016
	.endef
	.def	_ready
	.val	148
	.scl	8
	.tag	_.4fake
	.size	12
	.type	030
	.endef
	.def	_Priority_map
	.val	152
	.scl	8
	.tag	_.14fake
	.size	12
	.type	010
	.endef
	.def	_Start
	.val	164
	.scl	8
	.tag	_.26fake
	.size	56
	.type	010
	.endef
	.def	_Registers
	.val	220
	.scl	8
	.tag	_.1fake
	.size	144
	.type	010
	.endef
	.def	_fp_context
	.val	364
	.scl	8
	.type	021
	.endef
	.def	_API_Extensions
	.val	368
	.scl	8
	.dim	3
	.size	12
	.type	0161
	.endef
	.def	_extensions
	.val	380
	.scl	8
	.type	0121
	.endef
	.def	_task_variables
	.val	384
	.scl	8
	.tag	_rtems_task_variable_tt
	.size	20
	.type	030
	.endef
	.def	.eos
	.val	388
	.scl	102
	.tag	_Thread_Control_struct
	.size	388
	.endef
	.def	_rtems_event_set
	.scl	13
	.type	016
	.endef
	.def	_rtems_signal_set
	.scl	13
	.type	016
	.endef
	.def	_rtems_asr
	.scl	13
	.type	01
	.endef
	.def	_rtems_asr_entry
	.scl	13
	.type	0221
	.endef
	.def	_.30fake
	.scl	10
	.type	010
	.size	24
	.endef
	.def	_is_enabled
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_handler
	.val	4
	.scl	8
	.type	0221
	.endef
	.def	_mode_set
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_signals_posted
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_signals_pending
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_nest_level
	.val	20
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	24
	.scl	102
	.tag	_.30fake
	.size	24
	.endef
	.def	_ASR_Information
	.scl	13
	.tag	_.30fake
	.size	24
	.type	010
	.endef
	.def	_rtems_attribute
	.scl	13
	.type	016
	.endef
	.def	_rtems_task_priority
	.scl	13
	.type	016
	.endef
	.def	_rtems_tcb
	.scl	13
	.tag	_Thread_Control_struct
	.size	388
	.type	010
	.endef
	.def	_rtems_task
	.scl	13
	.type	01
	.endef
	.def	_rtems_task_argument
	.scl	13
	.type	016
	.endef
	.def	_rtems_task_entry
	.scl	13
	.type	0221
	.endef
	.def	_.31fake
	.scl	10
	.type	010
	.size	28
	.endef
	.def	_name
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_stack_size
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_initial_priority
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_attribute_set
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_entry_point
	.val	16
	.scl	8
	.type	0221
	.endef
	.def	_mode_set
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_argument
	.val	24
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	28
	.scl	102
	.tag	_.31fake
	.size	28
	.endef
	.def	_rtems_initialization_tasks_table
	.scl	13
	.tag	_.31fake
	.size	28
	.type	010
	.endef
	.def	_.32fake
	.scl	10
	.type	010
	.size	96
	.endef
	.def	_Notepads
	.val	0
	.scl	8
	.dim	16
	.size	64
	.type	076
	.endef
	.def	_pending_events
	.val	64
	.scl	8
	.type	016
	.endef
	.def	_event_condition
	.val	68
	.scl	8
	.type	016
	.endef
	.def	_Signal
	.val	72
	.scl	8
	.tag	_.30fake
	.size	24
	.type	010
	.endef
	.def	.eos
	.val	96
	.scl	102
	.tag	_.32fake
	.size	96
	.endef
	.def	_RTEMS_API_Control
	.scl	13
	.tag	_.32fake
	.size	96
	.type	010
	.endef
	.def	_.33fake
	.scl	10
	.type	010
	.size	40
	.endef
	.def	_maximum_tasks
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_maximum_timers
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_maximum_semaphores
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_maximum_message_queues
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_maximum_partitions
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_maximum_regions
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_maximum_ports
	.val	24
	.scl	8
	.type	016
	.endef
	.def	_maximum_periods
	.val	28
	.scl	8
	.type	016
	.endef
	.def	_number_of_initialization_tasks
	.val	32
	.scl	8
	.type	016
	.endef
	.def	_User_initialization_tasks_table
	.val	36
	.scl	8
	.tag	_.31fake
	.size	28
	.type	030
	.endef
	.def	.eos
	.val	40
	.scl	102
	.tag	_.33fake
	.size	40
	.endef
	.def	_rtems_api_configuration_table
	.scl	13
	.tag	_.33fake
	.size	40
	.type	010
	.endef
	.def	_.34fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_INTERNAL_ERROR_CORE
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_RTEMS_API
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_POSIX_API
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_ITRON_API
	.val	3
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.34fake
	.size	4
	.endef
	.def	_Internal_errors_Source
	.scl	13
	.tag	_.34fake
	.size	4
	.type	012
	.endef
	.def	_.35fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_INTERNAL_ERROR_NO_CONFIGURATION_TABLE
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_NO_CPU_TABLE
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_INVALID_WORKSPACE_ADDRESS
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_TOO_LITTLE_WORKSPACE
	.val	3
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_WORKSPACE_ALLOCATION
	.val	4
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL
	.val	5
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_THREAD_EXITTED
	.val	6
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_INCONSISTENT_MP_INFORMATION
	.val	7
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_INVALID_NODE
	.val	8
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_NO_MPCI
	.val	9
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_BAD_PACKET
	.val	10
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_OUT_OF_PACKETS
	.val	11
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_OUT_OF_GLOBAL_OBJECTS
	.val	12
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_OUT_OF_PROXIES
	.val	13
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_INVALID_GLOBAL_ID
	.val	14
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_BAD_STACK_HOOK
	.val	15
	.scl	16
	.type	013
	.endef
	.def	_INTERNAL_ERROR_BAD_ATTRIBUTES
	.val	16
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.35fake
	.size	4
	.endef
	.def	_Internal_errors_Core_list
	.scl	13
	.tag	_.35fake
	.size	4
	.type	012
	.endef
	.def	_.36fake
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_the_source
	.val	0
	.scl	8
	.tag	_.34fake
	.size	4
	.type	012
	.endef
	.def	_is_internal
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_the_error
	.val	8
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_.36fake
	.size	12
	.endef
	.def	_Internal_errors_Information
	.scl	13
	.tag	_.36fake
	.size	12
	.type	010
	.endef
	.def	_User_extensions_routine
	.scl	13
	.type	01
	.endef
	.def	_User_extensions_thread_create_extension
	.scl	13
	.type	0236
	.endef
	.def	_User_extensions_thread_delete_extension
	.scl	13
	.type	0221
	.endef
	.def	_User_extensions_thread_start_extension
	.scl	13
	.type	0221
	.endef
	.def	_User_extensions_thread_restart_extension
	.scl	13
	.type	0221
	.endef
	.def	_User_extensions_thread_switch_extension
	.scl	13
	.type	0221
	.endef
	.def	_User_extensions_thread_post_switch_extension
	.scl	13
	.type	0221
	.endef
	.def	_User_extensions_thread_begin_extension
	.scl	13
	.type	0221
	.endef
	.def	_User_extensions_thread_exitted_extension
	.scl	13
	.type	0221
	.endef
	.def	_User_extensions_fatal_extension
	.scl	13
	.type	0221
	.endef
	.def	_.37fake
	.scl	10
	.type	010
	.size	32
	.endef
	.def	_thread_create
	.val	0
	.scl	8
	.type	0236
	.endef
	.def	_thread_start
	.val	4
	.scl	8
	.type	0221
	.endef
	.def	_thread_restart
	.val	8
	.scl	8
	.type	0221
	.endef
	.def	_thread_delete
	.val	12
	.scl	8
	.type	0221
	.endef
	.def	_thread_switch
	.val	16
	.scl	8
	.type	0221
	.endef
	.def	_thread_begin
	.val	20
	.scl	8
	.type	0221
	.endef
	.def	_thread_exitted
	.val	24
	.scl	8
	.type	0221
	.endef
	.def	_fatal
	.val	28
	.scl	8
	.type	0221
	.endef
	.def	.eos
	.val	32
	.scl	102
	.tag	_.37fake
	.size	32
	.endef
	.def	_User_extensions_Table
	.scl	13
	.tag	_.37fake
	.size	32
	.type	010
	.endef
	.def	_.38fake
	.scl	10
	.type	010
	.size	40
	.endef
	.def	_Node
	.val	0
	.scl	8
	.tag	_Chain_Node_struct
	.size	8
	.type	010
	.endef
	.def	_Callouts
	.val	8
	.scl	8
	.tag	_.37fake
	.size	32
	.type	010
	.endef
	.def	.eos
	.val	40
	.scl	102
	.tag	_.38fake
	.size	40
	.endef
	.def	_User_extensions_Control
	.scl	13
	.tag	_.38fake
	.size	40
	.type	010
	.endef
	.def	_.39fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_HEAP_EXTEND_SUCCESSFUL
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_HEAP_EXTEND_ERROR
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_HEAP_EXTEND_NOT_IMPLEMENTED
	.val	2
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.39fake
	.size	4
	.endef
	.def	_Heap_Extend_status
	.scl	13
	.tag	_.39fake
	.size	4
	.type	012
	.endef
	.def	_Heap_Block
	.scl	13
	.type	010
	.endef
	.def	_Heap_Block_struct
	.scl	10
	.type	010
	.size	16
	.endef
	.def	_back_flag
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_front_flag
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_next
	.val	8
	.scl	8
	.tag	_Heap_Block_struct
	.size	16
	.type	030
	.endef
	.def	_previous
	.val	12
	.scl	8
	.tag	_Heap_Block_struct
	.size	16
	.type	030
	.endef
	.def	.eos
	.val	16
	.scl	102
	.tag	_Heap_Block_struct
	.size	16
	.endef
	.def	_.40fake
	.scl	10
	.type	010
	.size	28
	.endef
	.def	_start
	.val	0
	.scl	8
	.tag	_Heap_Block_struct
	.size	16
	.type	030
	.endef
	.def	_final
	.val	4
	.scl	8
	.tag	_Heap_Block_struct
	.size	16
	.type	030
	.endef
	.def	_first
	.val	8
	.scl	8
	.tag	_Heap_Block_struct
	.size	16
	.type	030
	.endef
	.def	_permanent_null
	.val	12
	.scl	8
	.tag	_Heap_Block_struct
	.size	16
	.type	030
	.endef
	.def	_last
	.val	16
	.scl	8
	.tag	_Heap_Block_struct
	.size	16
	.type	030
	.endef
	.def	_page_size
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_reserved
	.val	24
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	28
	.scl	102
	.tag	_.40fake
	.size	28
	.endef
	.def	_Heap_Control
	.scl	13
	.tag	_.40fake
	.size	28
	.type	010
	.endef
	.def	_rtems_extension
	.scl	13
	.type	01
	.endef
	.def	_rtems_task_create_extension
	.scl	13
	.type	0236
	.endef
	.def	_rtems_task_delete_extension
	.scl	13
	.type	0221
	.endef
	.def	_rtems_task_start_extension
	.scl	13
	.type	0221
	.endef
	.def	_rtems_task_restart_extension
	.scl	13
	.type	0221
	.endef
	.def	_rtems_task_switch_extension
	.scl	13
	.type	0221
	.endef
	.def	_rtems_task_post_switch_extension
	.scl	13
	.type	0221
	.endef
	.def	_rtems_task_begin_extension
	.scl	13
	.type	0221
	.endef
	.def	_rtems_task_exitted_extension
	.scl	13
	.type	0221
	.endef
	.def	_rtems_fatal_extension
	.scl	13
	.type	0221
	.endef
	.def	_rtems_extensions_table
	.scl	13
	.tag	_.37fake
	.size	32
	.type	010
	.endef
	.def	_.41fake
	.scl	10
	.type	010
	.size	56
	.endef
	.def	_Object
	.val	0
	.scl	8
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_Extension
	.val	16
	.scl	8
	.tag	_.38fake
	.size	40
	.type	010
	.endef
	.def	.eos
	.val	56
	.scl	102
	.tag	_.41fake
	.size	56
	.endef
	.def	_Extension_Control
	.scl	13
	.tag	_.41fake
	.size	56
	.type	010
	.endef
	.def	_rtems_device_major_number
	.scl	13
	.type	016
	.endef
	.def	_rtems_device_minor_number
	.scl	13
	.type	016
	.endef
	.def	_rtems_device_driver
	.scl	13
	.tag	_.3fake
	.size	4
	.type	012
	.endef
	.def	_rtems_device_driver_entry
	.scl	13
	.tag	_.3fake
	.size	4
	.type	0232
	.endef
	.def	_.42fake
	.scl	10
	.type	010
	.size	24
	.endef
	.def	_initialization
	.val	0
	.scl	8
	.tag	_.3fake
	.size	4
	.type	0232
	.endef
	.def	_open
	.val	4
	.scl	8
	.tag	_.3fake
	.size	4
	.type	0232
	.endef
	.def	_close
	.val	8
	.scl	8
	.tag	_.3fake
	.size	4
	.type	0232
	.endef
	.def	_read
	.val	12
	.scl	8
	.tag	_.3fake
	.size	4
	.type	0232
	.endef
	.def	_write
	.val	16
	.scl	8
	.tag	_.3fake
	.size	4
	.type	0232
	.endef
	.def	_control
	.val	20
	.scl	8
	.tag	_.3fake
	.size	4
	.type	0232
	.endef
	.def	.eos
	.val	24
	.scl	102
	.tag	_.42fake
	.size	24
	.endef
	.def	_rtems_driver_address_table
	.scl	13
	.tag	_.42fake
	.size	24
	.type	010
	.endef
	.def	_.43fake
	.scl	10
	.type	010
	.size	16
	.endef
	.def	_device_name
	.val	0
	.scl	8
	.type	022
	.endef
	.def	_device_name_length
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_major
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_minor
	.val	12
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	16
	.scl	102
	.tag	_.43fake
	.size	16
	.endef
	.def	_rtems_driver_name_t
	.scl	13
	.tag	_.43fake
	.size	16
	.type	010
	.endef
	.def	_.44fake
	.scl	10
	.type	010
	.size	20
	.endef
	.def	_node
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_maximum_nodes
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_maximum_global_objects
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_maximum_proxies
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_User_mpci_table
	.val	16
	.scl	8
	.type	021
	.endef
	.def	.eos
	.val	20
	.scl	102
	.tag	_.44fake
	.size	20
	.endef
	.def	_rtems_multiprocessing_table
	.scl	13
	.tag	_.44fake
	.size	20
	.type	010
	.endef
	.def	_.45fake
	.scl	10
	.type	010
	.size	56
	.endef
	.def	_work_space_start
	.val	0
	.scl	8
	.type	021
	.endef
	.def	_work_space_size
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_maximum_extensions
	.val	8
	.scl	8
	.type	016
	.endef
	.def	_microseconds_per_tick
	.val	12
	.scl	8
	.type	016
	.endef
	.def	_ticks_per_timeslice
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_maximum_devices
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_number_of_device_drivers
	.val	24
	.scl	8
	.type	016
	.endef
	.def	_Device_driver_table
	.val	28
	.scl	8
	.tag	_.42fake
	.size	24
	.type	030
	.endef
	.def	_number_of_initial_extensions
	.val	32
	.scl	8
	.type	016
	.endef
	.def	_User_extension_table
	.val	36
	.scl	8
	.tag	_.37fake
	.size	32
	.type	030
	.endef
	.def	_User_multiprocessing_table
	.val	40
	.scl	8
	.tag	_.44fake
	.size	20
	.type	030
	.endef
	.def	_RTEMS_api_configuration
	.val	44
	.scl	8
	.tag	_.33fake
	.size	40
	.type	030
	.endef
	.def	_POSIX_api_configuration
	.val	48
	.scl	8
	.tag	_.11fake
	.size	40
	.type	030
	.endef
	.def	_ITRON_api_configuration
	.val	52
	.scl	8
	.tag	_.13fake
	.size	40
	.type	030
	.endef
	.def	.eos
	.val	56
	.scl	102
	.tag	_.45fake
	.size	56
	.endef
	.def	_rtems_configuration_table
	.scl	13
	.tag	_.45fake
	.size	56
	.type	010
	.endef
	.def	_rtems_interrupt_level
	.scl	13
	.type	016
	.endef
	.def	_rtems_vector_number
	.scl	13
	.type	016
	.endef
	.def	_rtems_isr
	.scl	13
	.type	01
	.endef
	.def	_rtems_isr_entry
	.scl	13
	.type	0221
	.endef
	.def	_.46fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_RTEMS_CLOCK_GET_TOD
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_CLOCK_GET_TICKS_SINCE_BOOT
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_CLOCK_GET_TICKS_PER_SECOND
	.val	3
	.scl	16
	.type	013
	.endef
	.def	_RTEMS_CLOCK_GET_TIME_VALUE
	.val	4
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.46fake
	.size	4
	.endef
	.def	_rtems_clock_get_options
	.scl	13
	.tag	_.46fake
	.size	4
	.type	012
	.endef
	.def	_.47fake
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_seconds
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_microseconds
	.val	4
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_.47fake
	.size	8
	.endef
	.def	_rtems_clock_time_value
	.scl	13
	.tag	_.47fake
	.size	8
	.type	010
	.endef
	.def	_.48fake
	.scl	10
	.type	010
	.size	28
	.endef
	.def	_Object
	.val	0
	.scl	8
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_internal_base
	.val	16
	.scl	8
	.type	021
	.endef
	.def	_external_base
	.val	20
	.scl	8
	.type	021
	.endef
	.def	_length
	.val	24
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	28
	.scl	102
	.tag	_.48fake
	.size	28
	.endef
	.def	_Dual_ported_memory_Control
	.scl	13
	.tag	_.48fake
	.size	28
	.type	010
	.endef
	.def	_rtems_option
	.scl	13
	.type	016
	.endef
	.def	_.49fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_EVENT_SYNC_SYNCHRONIZED
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_EVENT_SYNC_NOTHING_HAPPENED
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_EVENT_SYNC_TIMEOUT
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_EVENT_SYNC_SATISFIED
	.val	3
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.49fake
	.size	4
	.endef
	.def	_Event_Sync_states
	.scl	13
	.tag	_.49fake
	.size	4
	.type	012
	.endef
	.def	_Thread_queue_Flush_callout
	.scl	13
	.type	0221
	.endef
	.def	_Thread_queue_Extract_callout
	.scl	13
	.type	0221
	.endef
	.def	_CORE_message_queue_API_mp_support_callout
	.scl	13
	.type	0221
	.endef
	.def	_.50fake
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_size
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_buffer
	.val	4
	.scl	8
	.dim	1
	.size	4
	.type	076
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_.50fake
	.size	8
	.endef
	.def	_CORE_message_queue_Buffer
	.scl	13
	.tag	_.50fake
	.size	8
	.type	010
	.endef
	.def	_.51fake
	.scl	10
	.type	010
	.size	20
	.endef
	.def	_Node
	.val	0
	.scl	8
	.tag	_Chain_Node_struct
	.size	8
	.type	010
	.endef
	.def	_priority
	.val	8
	.scl	8
	.type	04
	.endef
	.def	_Contents
	.val	12
	.scl	8
	.tag	_.50fake
	.size	8
	.type	010
	.endef
	.def	.eos
	.val	20
	.scl	102
	.tag	_.51fake
	.size	20
	.endef
	.def	_CORE_message_queue_Buffer_control
	.scl	13
	.tag	_.51fake
	.size	20
	.type	010
	.endef
	.def	_.52fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY
	.val	1
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.52fake
	.size	4
	.endef
	.def	_CORE_message_queue_Disciplines
	.scl	13
	.tag	_.52fake
	.size	4
	.type	012
	.endef
	.def	_CORE_message_queue_Submit_types
	.scl	13
	.type	04
	.endef
	.def	_.53fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_CORE_MESSAGE_QUEUE_STATUS_TOO_MANY
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED
	.val	3
	.scl	16
	.type	013
	.endef
	.def	_CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT
	.val	4
	.scl	16
	.type	013
	.endef
	.def	_CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED
	.val	5
	.scl	16
	.type	013
	.endef
	.def	_CORE_MESSAGE_QUEUE_STATUS_TIMEOUT
	.val	6
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.53fake
	.size	4
	.endef
	.def	_CORE_message_queue_Status
	.scl	13
	.tag	_.53fake
	.size	4
	.type	012
	.endef
	.def	_.54fake
	.scl	10
	.type	010
	.size	4
	.endef
	.def	_discipline
	.val	0
	.scl	8
	.tag	_.52fake
	.size	4
	.type	012
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.54fake
	.size	4
	.endef
	.def	_CORE_message_queue_Attributes
	.scl	13
	.tag	_.54fake
	.size	4
	.type	010
	.endef
	.def	_CORE_message_queue_Notify_Handler
	.scl	13
	.type	0221
	.endef
	.def	_.55fake
	.scl	10
	.type	010
	.size	116
	.endef
	.def	_Wait_queue
	.val	0
	.scl	8
	.tag	_.23fake
	.size	64
	.type	010
	.endef
	.def	_Attributes
	.val	64
	.scl	8
	.tag	_.54fake
	.size	4
	.type	010
	.endef
	.def	_maximum_pending_messages
	.val	68
	.scl	8
	.type	016
	.endef
	.def	_number_of_pending_messages
	.val	72
	.scl	8
	.type	016
	.endef
	.def	_maximum_message_size
	.val	76
	.scl	8
	.type	016
	.endef
	.def	_Pending_messages
	.val	80
	.scl	8
	.tag	_.4fake
	.size	12
	.type	010
	.endef
	.def	_message_buffers
	.val	92
	.scl	8
	.tag	_.50fake
	.size	8
	.type	030
	.endef
	.def	_notify_handler
	.val	96
	.scl	8
	.type	0221
	.endef
	.def	_notify_argument
	.val	100
	.scl	8
	.type	021
	.endef
	.def	_Inactive_messages
	.val	104
	.scl	8
	.tag	_.4fake
	.size	12
	.type	010
	.endef
	.def	.eos
	.val	116
	.scl	102
	.tag	_.55fake
	.size	116
	.endef
	.def	_CORE_message_queue_Control
	.scl	13
	.tag	_.55fake
	.size	116
	.type	010
	.endef
	.def	___int32_t
	.scl	13
	.type	04
	.endef
	.def	___uint32_t
	.scl	13
	.type	016
	.endef
	.def	_size_t
	.scl	13
	.type	017
	.endef
	.def	_.56fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_MESSAGE_QUEUE_SEND_REQUEST
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_MESSAGE_QUEUE_URGENT_REQUEST
	.val	1
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.56fake
	.size	4
	.endef
	.def	_Message_queue_Submit_types
	.scl	13
	.tag	_.56fake
	.size	4
	.type	012
	.endef
	.def	_.57fake
	.scl	10
	.type	010
	.size	136
	.endef
	.def	_Object
	.val	0
	.scl	8
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_attribute_set
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_message_queue
	.val	20
	.scl	8
	.tag	_.55fake
	.size	116
	.type	010
	.endef
	.def	.eos
	.val	136
	.scl	102
	.tag	_.57fake
	.size	136
	.endef
	.def	_Message_queue_Control
	.scl	13
	.tag	_.57fake
	.size	136
	.type	010
	.endef
	.def	_.58fake
	.scl	10
	.type	010
	.size	48
	.endef
	.def	_Object
	.val	0
	.scl	8
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_starting_address
	.val	16
	.scl	8
	.type	021
	.endef
	.def	_length
	.val	20
	.scl	8
	.type	016
	.endef
	.def	_buffer_size
	.val	24
	.scl	8
	.type	016
	.endef
	.def	_attribute_set
	.val	28
	.scl	8
	.type	016
	.endef
	.def	_number_of_used_blocks
	.val	32
	.scl	8
	.type	016
	.endef
	.def	_Memory
	.val	36
	.scl	8
	.tag	_.4fake
	.size	12
	.type	010
	.endef
	.def	.eos
	.val	48
	.scl	102
	.tag	_.58fake
	.size	48
	.endef
	.def	_Partition_Control
	.scl	13
	.tag	_.58fake
	.size	48
	.type	010
	.endef
	.def	_.59fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_RATE_MONOTONIC_INACTIVE
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_RATE_MONOTONIC_OWNER_IS_BLOCKING
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_RATE_MONOTONIC_ACTIVE
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING
	.val	3
	.scl	16
	.type	013
	.endef
	.def	_RATE_MONOTONIC_EXPIRED
	.val	4
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.59fake
	.size	4
	.endef
	.def	_rtems_rate_monotonic_period_states
	.scl	13
	.tag	_.59fake
	.size	4
	.type	012
	.endef
	.def	_.60fake
	.scl	10
	.type	010
	.size	12
	.endef
	.def	_state
	.val	0
	.scl	8
	.tag	_.59fake
	.size	4
	.type	012
	.endef
	.def	_ticks_since_last_period
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_ticks_executed_since_last_period
	.val	8
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	12
	.scl	102
	.tag	_.60fake
	.size	12
	.endef
	.def	_rtems_rate_monotonic_period_status
	.scl	13
	.tag	_.60fake
	.size	12
	.type	010
	.endef
	.def	_.61fake
	.scl	10
	.type	010
	.size	72
	.endef
	.def	_Object
	.val	0
	.scl	8
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_Timer
	.val	16
	.scl	8
	.tag	_.17fake
	.size	40
	.type	010
	.endef
	.def	_state
	.val	56
	.scl	8
	.tag	_.59fake
	.size	4
	.type	012
	.endef
	.def	_owner_ticks_executed_at_period
	.val	60
	.scl	8
	.type	016
	.endef
	.def	_time_at_period
	.val	64
	.scl	8
	.type	016
	.endef
	.def	_owner
	.val	68
	.scl	8
	.tag	_Thread_Control_struct
	.size	388
	.type	030
	.endef
	.def	.eos
	.val	72
	.scl	102
	.tag	_.61fake
	.size	72
	.endef
	.def	_Rate_monotonic_Control
	.scl	13
	.tag	_.61fake
	.size	72
	.type	010
	.endef
	.def	_rtems_debug_control
	.scl	13
	.type	016
	.endef
	.def	_.62fake
	.scl	10
	.type	010
	.size	132
	.endef
	.def	_Object
	.val	0
	.scl	8
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_Wait_queue
	.val	16
	.scl	8
	.tag	_.23fake
	.size	64
	.type	010
	.endef
	.def	_starting_address
	.val	80
	.scl	8
	.type	021
	.endef
	.def	_length
	.val	84
	.scl	8
	.type	016
	.endef
	.def	_page_size
	.val	88
	.scl	8
	.type	016
	.endef
	.def	_maximum_segment_size
	.val	92
	.scl	8
	.type	016
	.endef
	.def	_attribute_set
	.val	96
	.scl	8
	.type	016
	.endef
	.def	_number_of_used_blocks
	.val	100
	.scl	8
	.type	016
	.endef
	.def	_Memory
	.val	104
	.scl	8
	.tag	_.40fake
	.size	28
	.type	010
	.endef
	.def	.eos
	.val	132
	.scl	102
	.tag	_.62fake
	.size	132
	.endef
	.def	_Region_Control
	.scl	13
	.tag	_.62fake
	.size	132
	.type	010
	.endef
	.def	_CORE_mutex_API_mp_support_callout
	.scl	13
	.type	0221
	.endef
	.def	_.63fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_CORE_MUTEX_DISCIPLINES_FIFO
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_DISCIPLINES_PRIORITY
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING
	.val	3
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.63fake
	.size	4
	.endef
	.def	_CORE_mutex_Disciplines
	.scl	13
	.tag	_.63fake
	.size	4
	.type	012
	.endef
	.def	_.64fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_CORE_MUTEX_STATUS_SUCCESSFUL
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE
	.val	3
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_WAS_DELETED
	.val	4
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_TIMEOUT
	.val	5
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_STATUS_CEILING_VIOLATED
	.val	6
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.64fake
	.size	4
	.endef
	.def	_CORE_mutex_Status
	.scl	13
	.tag	_.64fake
	.size	4
	.type	012
	.endef
	.def	_.65fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_CORE_MUTEX_NESTING_ACQUIRES
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_NESTING_IS_ERROR
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_CORE_MUTEX_NESTING_BLOCKS
	.val	2
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.65fake
	.size	4
	.endef
	.def	_CORE_mutex_Nesting_behaviors
	.scl	13
	.tag	_.65fake
	.size	4
	.type	012
	.endef
	.def	_.66fake
	.scl	10
	.type	010
	.size	16
	.endef
	.def	_lock_nesting_behavior
	.val	0
	.scl	8
	.tag	_.65fake
	.size	4
	.type	012
	.endef
	.def	_only_owner_release
	.val	4
	.scl	8
	.type	016
	.endef
	.def	_discipline
	.val	8
	.scl	8
	.tag	_.63fake
	.size	4
	.type	012
	.endef
	.def	_priority_ceiling
	.val	12
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	16
	.scl	102
	.tag	_.66fake
	.size	16
	.endef
	.def	_CORE_mutex_Attributes
	.scl	13
	.tag	_.66fake
	.size	16
	.type	010
	.endef
	.def	_.67fake
	.scl	10
	.type	010
	.size	96
	.endef
	.def	_Wait_queue
	.val	0
	.scl	8
	.tag	_.23fake
	.size	64
	.type	010
	.endef
	.def	_Attributes
	.val	64
	.scl	8
	.tag	_.66fake
	.size	16
	.type	010
	.endef
	.def	_lock
	.val	80
	.scl	8
	.type	016
	.endef
	.def	_nest_count
	.val	84
	.scl	8
	.type	016
	.endef
	.def	_holder
	.val	88
	.scl	8
	.tag	_Thread_Control_struct
	.size	388
	.type	030
	.endef
	.def	_holder_id
	.val	92
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	96
	.scl	102
	.tag	_.67fake
	.size	96
	.endef
	.def	_CORE_mutex_Control
	.scl	13
	.tag	_.67fake
	.size	96
	.type	010
	.endef
	.def	_CORE_semaphore_API_mp_support_callout
	.scl	13
	.type	0221
	.endef
	.def	_.68fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_CORE_SEMAPHORE_DISCIPLINES_FIFO
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_CORE_SEMAPHORE_DISCIPLINES_PRIORITY
	.val	1
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.68fake
	.size	4
	.endef
	.def	_CORE_semaphore_Disciplines
	.scl	13
	.tag	_.68fake
	.size	4
	.type	012
	.endef
	.def	_.69fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_CORE_SEMAPHORE_STATUS_SUCCESSFUL
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_CORE_SEMAPHORE_WAS_DELETED
	.val	2
	.scl	16
	.type	013
	.endef
	.def	_CORE_SEMAPHORE_TIMEOUT
	.val	3
	.scl	16
	.type	013
	.endef
	.def	_CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED
	.val	4
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.69fake
	.size	4
	.endef
	.def	_CORE_semaphore_Status
	.scl	13
	.tag	_.69fake
	.size	4
	.type	012
	.endef
	.def	_.70fake
	.scl	10
	.type	010
	.size	8
	.endef
	.def	_maximum_count
	.val	0
	.scl	8
	.type	016
	.endef
	.def	_discipline
	.val	4
	.scl	8
	.tag	_.68fake
	.size	4
	.type	012
	.endef
	.def	.eos
	.val	8
	.scl	102
	.tag	_.70fake
	.size	8
	.endef
	.def	_CORE_semaphore_Attributes
	.scl	13
	.tag	_.70fake
	.size	8
	.type	010
	.endef
	.def	_.71fake
	.scl	10
	.type	010
	.size	76
	.endef
	.def	_Wait_queue
	.val	0
	.scl	8
	.tag	_.23fake
	.size	64
	.type	010
	.endef
	.def	_Attributes
	.val	64
	.scl	8
	.tag	_.70fake
	.size	8
	.type	010
	.endef
	.def	_count
	.val	72
	.scl	8
	.type	016
	.endef
	.def	.eos
	.val	76
	.scl	102
	.tag	_.71fake
	.size	76
	.endef
	.def	_CORE_semaphore_Control
	.scl	13
	.tag	_.71fake
	.size	76
	.type	010
	.endef
	.def	_.72fake
	.scl	12
	.type	011
	.size	96
	.endef
	.def	_mutex
	.val	0
	.scl	11
	.tag	_.67fake
	.size	96
	.type	010
	.endef
	.def	_semaphore
	.val	0
	.scl	11
	.tag	_.71fake
	.size	76
	.type	010
	.endef
	.def	.eos
	.val	96
	.scl	102
	.tag	_.72fake
	.size	96
	.endef
	.def	_.73fake
	.scl	10
	.type	010
	.size	116
	.endef
	.def	_Object
	.val	0
	.scl	8
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_attribute_set
	.val	16
	.scl	8
	.type	016
	.endef
	.def	_Core_control
	.val	20
	.scl	8
	.tag	_.72fake
	.size	96
	.type	011
	.endef
	.def	.eos
	.val	116
	.scl	102
	.tag	_.73fake
	.size	116
	.endef
	.def	_Semaphore_Control
	.scl	13
	.tag	_.73fake
	.size	116
	.type	010
	.endef
	.def	_.74fake
	.scl	15
	.type	012
	.size	4
	.endef
	.def	_TIMER_INTERVAL
	.val	0
	.scl	16
	.type	013
	.endef
	.def	_TIMER_TIME_OF_DAY
	.val	1
	.scl	16
	.type	013
	.endef
	.def	_TIMER_DORMANT
	.val	2
	.scl	16
	.type	013
	.endef
	.def	.eos
	.val	4
	.scl	102
	.tag	_.74fake
	.size	4
	.endef
	.def	_Timer_Classes
	.scl	13
	.tag	_.74fake
	.size	4
	.type	012
	.endef
	.def	_rtems_timer_service_routine
	.scl	13
	.type	01
	.endef
	.def	_rtems_timer_service_routine_entry
	.scl	13
	.type	0221
	.endef
	.def	_.75fake
	.scl	10
	.type	010
	.size	60
	.endef
	.def	_Object
	.val	0
	.scl	8
	.tag	_.7fake
	.size	16
	.type	010
	.endef
	.def	_Ticker
	.val	16
	.scl	8
	.tag	_.17fake
	.size	40
	.type	010
	.endef
	.def	_the_class
	.val	56
	.scl	8
	.tag	_.74fake
	.size	4
	.type	012
	.endef
	.def	.eos
	.val	60
	.scl	102
	.tag	_.75fake
	.size	60
	.endef
	.def	_Timer_Control
	.scl	13
	.tag	_.75fake
	.size	60
	.type	010
	.endef
	.align 4
.proc __RTEMS_API_Initialize
	.def	__RTEMS_API_Initialize
	.val	__RTEMS_API_Initialize
	.scl	2
	.type	041
	.endef
	.global __RTEMS_API_Initialize
__RTEMS_API_Initialize:
	.def	.bf
	.val	.
	.scl	101
	.line	53
	.endef
	.def	_configuration_table
	.val	-4
	.scl	9
	.tag	_.45fake
	.size	56
	.type	030
	.endef

	# 00111100010000000000000000000000
	# gpr_save_area 0 vars 8 current_function_outgoing_args_size 0
	l.addi   	r1,r1,-16
	l.sw     	4(r1),r2
	l.addi   	r2,r1,16
	l.sw     	0(r1),r9
	l.sw    	-4(r2),r3
	.ln	2
	.def	_api_configuration
	.val	-8
	.scl	1
	.tag	_.33fake
	.size	40
	.type	030
	.endef
	.ln	4
	l.lwz    	r3,-4(r2)	 # SI load
	l.lwz    	r4,44(r3)	 # SI load
	l.sw    	-8(r2),r4
	.ln	8
	l.jal   	__Interrupt_Manager_initialization
	l.nop			# nop delay slot
	.ln	14
	l.lwz    	r3,-8(r2)	 # SI load
	l.lwz    	r4,-8(r2)	 # SI load
	l.lwz    	r5,-8(r2)	 # SI load
	l.lwz    	r3,0(r3)	 # SI load
	l.lwz    	r4,32(r4)	 # SI load
	l.lwz    	r5,36(r5)	 # SI load
	l.jal   	__RTEMS_tasks_Manager_initialization
	l.nop			# nop delay slot
	.ln	20
	l.lwz    	r4,-8(r2)	 # SI load
	l.lwz    	r3,4(r4)	 # SI load
	l.jal   	__Timer_Manager_initialization
	l.nop			# nop delay slot
	.ln	22
	l.jal   	__Signal_Manager_initialization
	l.nop			# nop delay slot
	.ln	24
	l.jal   	__Event_Manager_initialization
	l.nop			# nop delay slot
	.ln	26
	l.lwz    	r4,-8(r2)	 # SI load
	l.lwz    	r3,12(r4)	 # SI load
	l.jal   	__Message_queue_Manager_initialization
	l.nop			# nop delay slot
	.ln	30
	l.lwz    	r4,-8(r2)	 # SI load
	l.lwz    	r3,8(r4)	 # SI load
	l.jal   	__Semaphore_Manager_initialization
	l.nop			# nop delay slot
	.ln	32
	l.lwz    	r4,-8(r2)	 # SI load
	l.lwz    	r3,16(r4)	 # SI load
	l.jal   	__Partition_Manager_initialization
	l.nop			# nop delay slot
	.ln	34
	l.lwz    	r4,-8(r2)	 # SI load
	l.lwz    	r3,20(r4)	 # SI load
	l.jal   	__Region_Manager_initialization
	l.nop			# nop delay slot
	.ln	36
	l.lwz    	r4,-8(r2)	 # SI load
	l.lwz    	r3,24(r4)	 # SI load
	l.jal   	__Dual_ported_memory_Manager_initialization
	l.nop			# nop delay slot
	.ln	38
	l.lwz    	r4,-8(r2)	 # SI load
	l.lwz    	r3,28(r4)	 # SI load
	l.jal   	__Rate_monotonic_Manager_initialization
	l.nop			# nop delay slot
	.ln	39
L335:
	.def	.ef
	.val	.
	.scl	101
	.line	39
	.endef
	l.lwz    	r9,0(r1)
	l.lwz    	r2,4(r1)
	l.jalr  	r9
	l.addi   	r1,r1,16
.endproc __RTEMS_API_Initialize
	.def	__RTEMS_API_Initialize
	.val	.
	.scl	-1
	.endef
	.def	__RTEMS_tasks_Information
	.val	__RTEMS_tasks_Information
	.scl	2
	.tag	_.8fake
	.size	76
	.type	010
	.endef
.data
	.align 4
.global	__RTEMS_tasks_Information
__RTEMS_tasks_Information:
	.space 76
.text
	.def	__RTEMS_tasks_User_initialization_tasks
	.val	__RTEMS_tasks_User_initialization_tasks
	.scl	2
	.tag	_.31fake
	.size	28
	.type	030
	.endef
.data
	.align 4
.global	__RTEMS_tasks_User_initialization_tasks
__RTEMS_tasks_User_initialization_tasks:
	.space 4
.text
	.def	__RTEMS_tasks_Number_of_initialization_tasks
	.val	__RTEMS_tasks_Number_of_initialization_tasks
	.scl	2
	.type	016
	.endef
.data
	.align 4
.global	__RTEMS_tasks_Number_of_initialization_tasks
__RTEMS_tasks_Number_of_initialization_tasks:
	.space 4
.text
	.def	__Dual_ported_memory_Information
	.val	__Dual_ported_memory_Information
	.scl	2
	.tag	_.8fake
	.size	76
	.type	010
	.endef
.data
	.align 4
.global	__Dual_ported_memory_Information
__Dual_ported_memory_Information:
	.space 76
.text
	.def	__Event_Sync_state
	.val	__Event_Sync_state
	.scl	2
	.tag	_.49fake
	.size	4
	.type	012
	.endef
.data
	.align 4
.global	__Event_Sync_state
__Event_Sync_state:
	.space 4
.text
	.def	__Message_queue_Information
	.val	__Message_queue_Information
	.scl	2
	.tag	_.8fake
	.size	76
	.type	010
	.endef
.data
	.align 4
.global	__Message_queue_Information
__Message_queue_Information:
	.space 76
.text
	.def	__Partition_Information
	.val	__Partition_Information
	.scl	2
	.tag	_.8fake
	.size	76
	.type	010
	.endef
.data
	.align 4
.global	__Partition_Information
__Partition_Information:
	.space 76
.text
	.def	__Rate_monotonic_Information
	.val	__Rate_monotonic_Information
	.scl	2
	.tag	_.8fake
	.size	76
	.type	010
	.endef
.data
	.align 4
.global	__Rate_monotonic_Information
__Rate_monotonic_Information:
	.space 76
.text
	.def	__Region_Information
	.val	__Region_Information
	.scl	2
	.tag	_.8fake
	.size	76
	.type	010
	.endef
.data
	.align 4
.global	__Region_Information
__Region_Information:
	.space 76
.text
	.def	__Semaphore_Information
	.val	__Semaphore_Information
	.scl	2
	.tag	_.8fake
	.size	76
	.type	010
	.endef
.data
	.align 4
.global	__Semaphore_Information
__Semaphore_Information:
	.space 76
.text
	.def	__Timer_Information
	.val	__Timer_Information
	.scl	2
	.tag	_.8fake
	.size	76
	.type	010
	.endef
.data
	.align 4
.global	__Timer_Information
__Timer_Information:
	.space 76
.text
	.def	__Status_Object_name_errors_to_status
	.val	__Status_Object_name_errors_to_status
	.scl	2
	.tag	_.3fake
	.dim	3
	.size	12
	.type	072
	.endef
