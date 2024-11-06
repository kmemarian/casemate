/* autogenerated file, do not edit */
#ifndef CASEMATE_H
#define CASEMATE_H

/*
 * Casemate public interface
 */

/* auto-included by Makefile */
/* Types also defined by UoC's pKVM ghost code headers
 * so do not try to include them in the top-level casemate.h if they are already defined
 */

#ifndef __KVM_NVHE_HYPERVISOR__

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

typedef unsigned long u64;
typedef signed long s64;
typedef unsigned int u32;
typedef signed int s32;
typedef int u8;
typedef u64 phys_addr_t;

#else

#include <linux/stdarg.h>
#include <linux/types.h>

#endif /* __KVM_NVHE_HYPERVISOR */


/* auto-included by Makefile */
#ifndef CASEMATE_CONFIG_H
#define CASEMATE_CONFIG_H

//////////////
// Configuration

typedef enum {
	CM_PRINT_NONE = 0,

	CM_PRINT_WHOLE_STATE_ON_STEP = 1,
	CM_PRINT_DIFF_TO_STATE_ON_STEP = 2,
	CM_PRINT_ONLY_UNCLEAN = 4,

	CM_PRINT_ALL = CM_PRINT_WHOLE_STATE_ON_STEP | CM_PRINT_DIFF_TO_STATE_ON_STEP,
	CM_PRINT_ALL_CONDENSED = CM_PRINT_ALL | CM_PRINT_ONLY_UNCLEAN,
} casemate_print_opts_t;

#define CASEMATE_DEFAULT_PRINT_OPTS \
	CM_PRINT_NONE

struct casemate_checker_options {
	/**
	 * @promote_DSB_nsh: Silently promote all DSB NSH to DSB ISH
	 */
	bool promote_DSB_nsh;

	/**
	 * @promote_TLBI_nsh: Silently promote all TLBI to broadcast ones
	 */
	bool promote_TLBI_nsh;

	/**
	 * @promote_TLBI_by_id: Silently promote all TLBI-by-ASID and by-VMID to ALL
	 */
	bool promote_TLBI_by_id;


	/**
	 * @check_synchronisation: check that locks are respected.
	 *
	 * WARNING: disabling is unsafe for online checking.
	 */
	bool check_synchronisation;

	/**
	 * @enable_printing: print out the current state of the
	 * @print_opts: logging to perform.
	 */
	bool enable_printing;
	casemate_print_opts_t print_opts;
};

#define CASEMATE_DEFAULT_CHECK_OPTS \
	(struct casemate_checker_options){ \
		.promote_DSB_nsh = false, \
		.promote_TLBI_nsh = false, \
		.promote_TLBI_by_id = false, \
		.check_synchronisation = true, \
		.enable_printing = false, \
		.print_opts = CASEMATE_DEFAULT_PRINT_OPTS, \
	}

struct casemate_log_options {
	/**
	 * @log_format_version: Version of the log format
	 */
	unsigned int log_format_version;

	/**
	 * @condensed_format: if true, trace omits the keys in key/value pairs.
	 */
	bool condensed_format;
};

#define CASEMATE_DEFAULT_LOG_OPTS \
	(struct casemate_log_options){ \
		.log_format_version = 1, \
		.condensed_format = false, \
	}

/**
 * struct casemate_options - Global configuration of ghost model behaviour
 *
 * Provides selective enabling/disabling of supported behaviours.
 */
struct casemate_options {
	/**
	 * @enable_tracing: Printing tracepoints
	 */
	bool enable_tracing;

	/**
	 * @enable_checking: Turn on/off runtime model checking
	 */
	bool enable_checking;

	/**
	 * @track_watchpoints: Only print/trace on watched locations.
	 */
	bool track_watchpoints;

	/**
	 * @log_opts: Options for logging
	 */
	struct casemate_log_options log_opts;

	/**
	 * @check_opts: Options for checker
	 */
	struct casemate_checker_options check_opts;

	/**
	 * @enable_safety_checks: Enables non-functional model-internal consistency checks.
	 */
	bool enable_safety_checks;
};

#define CASEMATE_DEFAULT_OPTS \
	(struct casemate_options){ \
		.enable_tracing = false, \
		.enable_checking = false, \
		.track_watchpoints = false, \
		.log_opts = CASEMATE_DEFAULT_LOG_OPTS, \
		.check_opts = CASEMATE_DEFAULT_CHECK_OPTS, \
		.enable_safety_checks = false, \
	}

enum ghost_sysreg_kind {
	SYSREG_VTTBR,
	SYSREG_TTBR_EL2,
	SYSREG_VTCR_EL2,
	SYSREG_TCR_EL2,
	SYSREG_MAIR_EL2,
};

struct casemate_model_step;
typedef int (*vprintf_cb)(void* arg, const char *format, va_list ap);
typedef void* (*sprint_make_buf_cb)(char* arg, u64 n);
typedef void (*sprint_free_buf_cb)(void *buf);
typedef void (*abort_cb)(const char *msg);
typedef u64 (*read_physmem_cb)(u64);
typedef u64 (*read_sysreg_cb)(enum ghost_sysreg_kind sysreg);
typedef void (*trace_cb)(const char *record);

/**
 * struct ghost_driver - Callbacks for driving the casemate model
 *
 * @print: callback for printing, with printf-like arguments.
 * @sprint_create_buffer: callback for making a buffer to pass to driver->print().
 * @sprint_destroy_buffer: frees a buffer created with `sprint_create_buffer`.
 * @halt: callback for failed assertion.
 * @read_physmem: perform a read of memory.
 * @read_sysreg: callback for reading system registers.
 * @trace: tracepoint callback.
 *
 * Casemate is standalone, but sometimes performs side-effects,
 * which must be handled by the parent kernel.
 *
 */
struct ghost_driver {
  vprintf_cb print;
	sprint_make_buf_cb sprint_create_buffer;
	sprint_free_buf_cb sprint_destroy_buffer;
  abort_cb abort;
	read_physmem_cb read_physmem;
	read_sysreg_cb read_sysreg;
	trace_cb trace;
};

#define CASEMATE_DEFAULT_EMPTY_DRIVER \
	(struct ghost_driver){ \
		.print = NULL, \
		.sprint_create_buffer = NULL, \
		.halt = NULL, \
		.read_physmem = NULL, \
		.read_sysreg = NULL, \
		.trace = NULL, \
	}

/**
 * casemate_watch_location() - Watch for changes to a particular physical address.
 *
 * Only has an effect when opts()->track_watchpoints is set.
 *
 * Returns -1 on error.
 */
int casemate_watch_location(u64 loc);

/**
 * initialise_ghost_cm_driver() - Setup the global ghost driver.
 */
void initialise_ghost_driver(struct ghost_driver *driver);

#endif /* CASEMATE_CONFIG_H */

/* auto-included by Makefile */

/*
 * Model types
 *
 * We try to create opaque typedefs where possible, when we do not care
 * about the implementation details (e.g. various integer types and sync primitives)
 */

// TODO: BS: make variable-sized data structures instead of fixed CPUs
#define MAX_CPU 4

/**
 * typedef gsm_lock_addr_t - ghost model lock
 */
typedef u64 gsm_lock_addr_t;

/**
 * typedef thread_identifier - ID for hardware thread/CPU.
 */
typedef int thread_identifier;

/**
 * typedef sm_owner_t - ID for ownership
 *
 * This is the physical address of the pagetable root.
 */
typedef u64 sm_owner_t;

/**
 * enum LVS - Local (this CPU) Valid State of a single non-invalid PTE.
 * @LVS_unguarded: a valid value has been written by this core, but not DSB'd.
 * @LVS_dsbed: a valid value has been written by this core, and now DSB'd.
 * @LVS_dsb_csed: a valid value has been written by this core,
 *                a subsequent DSB has been performed,
 *                and also a context-synchronisation event on this core.
 */
enum LVS {
	LVS_unguarded,
	LVS_dsbed,
	LVS_dsb_csed
};

/**
 * struct aut_valid - Automata state for a valid PTE.
 * @lvs: per-CPU local-valid-state.
 *
 * TODO: JP: should we remember writer thread?
 */
struct aut_valid {
	enum LVS lvs[MAX_CPU];
};

/**
 * enum LIS - Local (this CPU) Invalid State of a single invalid PTE.
 * @LIS_unguarded: an invalid value has been written by this core, but not DSB'd.
 * @LIS_dsbed: an invalid value has been written by this core, and now DSB'd, but not TLBI'd.
 * @LIS_dsb_tlbi_ipa: the invalid write has been written by this core, DSB'd, and only a TLBI that hit the IPA mappings for this loc.
 * @LIS_dsb_tlbi_ipa_dsb: the invalid write has been written by this core, DSB'd, and only a TLBI that hit the IPA mappings for this loc, and now a DSB has been performed.
 * @LIS_dsb_tlbied: the invalid write has been written by this core, DSB'd, and now fully TLBI'd.
 */
enum LIS {
	LIS_unguarded,
	LIS_dsbed,
	LIS_dsb_tlbi_ipa,
	LIS_dsb_tlbi_ipa_dsb,
	LIS_dsb_tlbied,
};

/**
 * struct aut_invalid - Automata state for an invalid PTE
 * @invalidator_tid: thread id of the thread which wrote invalid.
 * @old_valid_desc: the descriptor which got overwritten.
 * @lis: sub-invalid-state, for thread with tid invalidator_tid.
 */
struct aut_invalid {
	thread_identifier invalidator_tid;
	u64 old_valid_desc;
	enum LIS lis;
};

/**
 * struct aut_invalid_clean - Automata state for an invalid+sufficiently globally TLBI'd PTE.
 * @invalidator_tid: thread id of the thread which wrote invalid.
 */
struct aut_invalid_clean {
	thread_identifier invalidator_tid;
};

/**
 * enum automaton_state_kind - Global top-level per-PTE tracker state.
 * @STATE_PTE_VALID: a valid and cleaned location, i.e. all threads agree the pgtable has been updated.
 * @STATE_PTE_INVALID_UNCLEAN: a thread has written an invalid descriptor to this location,
 *                             but any required break-before-make has not been completed yet.
 * @STATE_PTE_INVALID: all break-before-make requirements are complete and all cores agree the location is clean.
 * @STATE_PTE_NOT_WRITABLE: the location is frozen, and no thread is permitted to write to it.
 */
enum automaton_state_kind {
	STATE_PTE_VALID,
	STATE_PTE_INVALID_UNCLEAN,
	STATE_PTE_INVALID,
	STATE_PTE_NOT_WRITABLE,
};

/**
 * struct pte_state - Automata state of a single PTE location.
 */
struct sm_pte_state {
	enum automaton_state_kind kind;
	union {
		struct aut_valid valid_state;
		struct aut_invalid invalid_unclean_state;
		struct aut_invalid_clean invalid_clean_state;
	};
};

/**
 * enum pte_kind - Pagetable descriptor kind.
 * @PTE_KIND_TABLE: A table entry with a pointer to another pagetable.
 * @PTE_KIND_MAP: Either a block or page entry, with a pointer to an output page.
 * @PTE_KIND_INVALID: An invalid descriptor.
 */
enum pte_kind {
	PTE_KIND_TABLE,
	PTE_KIND_MAP,  /* BLOCK,PAGE */
	PTE_KIND_INVALID,
};

/**
 * struct addr_range - A range start+size
 */
struct addr_range {
	u64 range_start;
	u64 range_size;
};


/**
 * enum entry_stage - (optional) stage of translation
 */
typedef enum {
	ENTRY_STAGE2 = 2,
	ENTRY_STAGE1 = 1,

	/**
	 * @ENTRY_STAGE_NONE: for memblocks and other non-pgtable mappings.
	 */
	ENTRY_STAGE_NONE = 0,
} entry_stage_t;

/**
 * enum entry_permissions - Abstract permissions for a range of OA, as bitflags
 */
enum entry_permissions {
	ENTRY_PERM_R = 1,
	ENTRY_PERM_W = 2,
	ENTRY_PERM_X = 4,

	/*
	 * ENTRY_PERM_UNKNOWN for encodings that do not correspond to any of the above.
	 */
	ENTRY_PERM_UNKNOWN = 8,
};
#define ENTRY_PERM_RW (ENTRY_PERM_R | ENTRY_PERM_W)
#define ENTRY_PERM_RWX (ENTRY_PERM_R | ENTRY_PERM_W | ENTRY_PERM_X)

/**
 * enum entry_memtype_attr - Abstract memory type.
 */
enum entry_memtype_attr {
	ENTRY_MEMTYPE_DEVICE,
	ENTRY_MEMTYPE_NORMAL_CACHEABLE,

	/* ENTRY_MEMTYPE_UNKNOWN for encodings that do not correspond to any of the above */
	ENTRY_MEMTYPE_UNKNOWN,
};


struct entry_attributes {
	enum entry_permissions prot;
	enum entry_memtype_attr memtype;

	/**
	 * @raw_arch_attrs: the raw descriptor, masked to the attribute bits
	 * Not semantically meaningful, but used in printing and diffs.
	 */
	u64 raw_arch_attrs;
};


/**
 * struct  entry_exploded_descriptor - Cached information about a PTE.
 * @kind: Whether the descriptor is invalid/a table/a block or page mapping.
 * @region: the input-address region this PTE covers.
 * @level: the level within the pgtable this entry is at.
 * @s2: whether this descriptor is for a Stage2 table.
 * @table_data: if kind is PTE_KIND_TABLE, the table descriptor data (next level table address).
 * @map_data: if kind is PTE_KIND_MAP, the mapping data (output address range, and other attributes).
 *
 * TODO: replace with entry_target...
 */
struct entry_exploded_descriptor {
	enum pte_kind kind;
	struct addr_range ia_region;
	u64 level;
	entry_stage_t stage;
	union {
		struct {
			u64 next_level_table_addr;
		} table_data;

		struct {
			struct addr_range oa_region;
			struct entry_attributes attrs;
		} map_data;
	};
};

/**
 * struct sm_location - A (64-bit) Location in the ghost model memory.
 * @initialised: whether this mem block has been initialised.
 * @phys_addr: the physical address of this location.
 * @val: if initialised, value stored by model for this location.
 * @is_pte: if initialised, whether this location is tracked as a PTE.
 * @descriptor: if initialised and is_pte, the value as a pre-computed descriptor kind.
 *              If the state is invalid unclean, then it is the last valid descriptor.
 * @state: if initialised and is_pte, the automata state for this location.
 * @owner: if initialised, the root of the tree that owns this location.
 * @thread_owner: if positive, the ID of the thread that can freely access this location
 *
 * The owner and descriptor are here as helpful cached values,
 * and could be computed by doing translation table walks.
 */
struct sm_location {
	bool initialised;
	u64 phys_addr;
	u64 val;
	bool is_pte;
	struct entry_exploded_descriptor descriptor;
	struct sm_pte_state state;
	sm_owner_t owner;
	int thread_owner;
};

/*
 * Memory
 *
 * To not duplicate the entire machines memory,
 * we instead only track "blobs" (arbitrary aligned chunks)
 * of memory that the ghost model checking machinery is actually aware of.
 *
 * These blobs are not really part of the public interface, but in C one cannot split
 * the private and public parts of the state so easily.
 */

#define SLOTS_PER_PAGE (512)

#define SLOT_SHIFT 3

#define BLOB_SHIFT 12
#define MAX_BLOBS (0x2000)
#define MAX_ROOTS 10
#define MAX_UNCLEAN_LOCATIONS 10

/**
 * struct casemate_memory_blob - A page of memory.
 * @valid: whether this blob is being used.
 * @phys: if valid, the physical address of the start of this region.
 * @slots: if valid, the array of memory locations within this region.
 *
 * Each blob is a aligned and contiguous page of memory.
 */
struct casemate_memory_blob {
	bool valid;
	u64 phys;
	struct sm_location slots[SLOTS_PER_PAGE];
};

/**
 * struct casemate_model_memory - ghost model memory.
 * @blobs_backing: the set of memory blobs.
 * @nr_allocated_blobs: the number of blobs created so far.
 * @ordered_blob_list: a list of indices of allocated blobs, in order of their physical addresses.
 */
struct casemate_model_memory {
	struct casemate_memory_blob blobs_backing[MAX_BLOBS];

	u64 nr_allocated_blobs;
	u64 ordered_blob_list[MAX_BLOBS];
};

/**
 * struct unclean_locations - set of locations
 */

struct location_set {
	struct sm_location *locations[MAX_UNCLEAN_LOCATIONS];
	u64 len;
};

#define casemate_model_MAX_LOCKS 8

/**
 * struct lock_owner_map - Map of pgtable root to lock that owns it.
 */
struct lock_owner_map {
	u64 len;
	sm_owner_t owner_ids[casemate_model_MAX_LOCKS];
	gsm_lock_addr_t *locks[casemate_model_MAX_LOCKS];
};

/**
 * enum write_authorization - Permission to write to the pagetable
 * @AUTHORIZED: Can perform any write to the locked object without constraints.
 * @UNAUTHORIZED_PLAIN: Cannot perform a plain (non-atomic) write to valid entries.
 *
 * Captures which kinds of writes to a locked object are permitted.
 */
enum write_authorization {
	AUTHORIZED,
	UNAUTHORIZED_PLAIN,
};

/**
 * struct lock_state - The current ghost state of a lock.
 * @id: The thread ID of the thread that currently owns the lock, or -1 if not held.
 * @write_authorization: what permission the owner of the lock has to the protected object.
 */
struct lock_state {
	thread_identifier id;
	enum write_authorization write_authorization;
};

/**
 * struct lock_state_map - Map of the locks to their current state.
 */
struct lock_state_map {
	u64 len;
	gsm_lock_addr_t *address[casemate_model_MAX_LOCKS];
	struct lock_state locker[casemate_model_MAX_LOCKS];
};

/**
 * owner_lock() - Get hyp spinlock for an owner.
 *
 * Returns NULL if no lock for that owner_id.
 */
gsm_lock_addr_t *owner_lock(sm_owner_t owner_id);

/**
 * struct casemate_model_state - Top-level ghost model state.
 * @base_addr: the physical address of the start of the (ghost) memory.
 * @size: the number of bytes in the ghost memory to track.
 * @memory: the actual ghost model memory.
 * @unclean_locations: set of all the unclean locations
 * @nr_s1_roots: number of EL2 stage1 pagetable roots being tracked.
 * @s1_roots: set of known EL2 stage1 pagetable roots.
 * @nr_s2_roots: number of EL2 stage2 pagetable roots being tracked.
 * @s2_roots: set of known EL2 stage2 pagetable roots.
 * @locks: map from root physical address to lock physical address.
 * @lock_state: map from lock physical address to thread which owns the lock.
 */
struct casemate_model_state {
	u64 base_addr;
	u64 size;
	struct casemate_model_memory memory;

	struct location_set unclean_locations;

	u64 nr_s1_roots;
	u64 s1_roots[MAX_ROOTS];

	u64 nr_s2_roots;
	u64 s2_roots[MAX_ROOTS];

	struct lock_owner_map locks;
	struct lock_state_map lock_state;
};

int ghost_dump_model_state(void *arg, struct casemate_model_state *st);

/* auto-included by Makefile */
/**
 * enum memory_order_t - A custom subset of standard `memory_order`.
 */
enum memory_order_t {
	WMO_plain,
	WMO_release,
};

/// Decoded TLBIs

enum sm_tlbi_op_stage {
	TLBI_OP_STAGE1 = 1,
	TLBI_OP_STAGE2 = 2,
	TLBI_OP_BOTH_STAGES = TLBI_OP_STAGE1 | TLBI_OP_STAGE2,
};

enum sm_tlbi_op_method_kind {
	TLBI_OP_BY_ALL        = 0, /* TLBI ALL* only */
	TLBI_OP_BY_INPUT_ADDR = 1, /* by Input-Address */
	TLBI_OP_BY_ADDR_SPACE = 2, /* by ASID/VMID */

	TLBI_OP_BY_VA = TLBI_OP_BY_INPUT_ADDR,
	TLBI_OP_BY_IPA = TLBI_OP_BY_INPUT_ADDR,

	TLBI_OP_BY_VMID = TLBI_OP_BY_ADDR_SPACE,
	TLBI_OP_BY_ASID = TLBI_OP_BY_ADDR_SPACE,
};

enum sm_tlbi_op_regime_kind {
	TLBI_REGIME_EL10 = 1, /* EL1&0 regime */
	TLBI_REGIME_EL2  = 2, /* EL2 regime */
};

/**
 * struct sm_tlbi_op_method - Decoded TLBI by-method
 * @kind: whether this is "all", by address, or by address-space-identifier.
 */
struct sm_tlbi_op_method {
	enum sm_tlbi_op_method_kind kind;
	union {
		struct tlbi_op_method_by_address_data {
			u64 page;

			bool has_level_hint;
			u8 level_hint;

			bool affects_last_level_only;
		} by_address_data;

		struct tlbi_op_method_by_address_space_id_data {
			u64 asid_or_vmid;
		} by_id_data;
	};
};

/**
 * struct sm_tlbi_op - A decoded TLB maintenance operation.
 * @stage: whether this affects cached stage1 translations, cached stage2 translations, or both.
 * @regime: the translation regime that this TLB maintenance operation would affect the cached entries of.
 * @method: whether this TLBI is by IPA, by VA, or by VMID, etc., and the relevant address/vmid, etc.
 * @shootdown: whether this TLB maintenance operation is broadcast to other cores.
 */
struct sm_tlbi_op {
	enum sm_tlbi_op_stage stage;
	enum sm_tlbi_op_regime_kind regime;
	struct sm_tlbi_op_method method;
	bool shootdown;
};

/// Encoded TLBIs

enum tlbi_kind {
	TLBI_vmalls12e1,
	TLBI_vmalls12e1is,
	TLBI_vmalle1is,
	TLBI_alle1is,
	TLBI_vmalle1,
	TLBI_vale2is,
	TLBI_vae2is,
	TLBI_ipas2e1is
};

enum dxb_kind {
	DxB_ish,
	DxB_ishst,
	DxB_nsh,
};

enum barrier_kind {
	BARRIER_DSB,
	BARRIER_ISB
};

enum casemate_model_step_kind {
	/**
	 * @TRANS_HW_STEP - Hardware instruction
	 */
	TRANS_HW_STEP,

	/**
	 * @TRANS_ABS_STEP - An abstract software state transition
	 * These generally transition some abstract reified ghost state in the model
	 * e.g. for locks and C initialisations that are not exposed to the model
	 */
	TRANS_ABS_STEP,

	/**
	 * @TRANS_HINT - A non-hardware-model transition
	 * These generally provide additional information to the casemate model,
	 * such as ownership, to resolve otherwise unbounded non-determinism
	 *
	 * Removing HINTs should not change soundness
	 */
	TRANS_HINT,
};

enum ghost_hw_step_kind {
	HW_MEM_WRITE,
	HW_MEM_READ,
	HW_BARRIER,
	HW_TLBI,
	HW_MSR,
};

struct ghost_hw_step {
	enum ghost_hw_step_kind kind;
	union {
		struct trans_write_data {
			enum memory_order_t mo;
			u64 phys_addr;
			u64 val;
		} write_data;

		struct trans_read_data {
			u64 phys_addr;
			u64 val;
		} read_data;

		struct trans_barrier_data {
			enum barrier_kind kind;
			enum dxb_kind dxb_data;
		} barrier_data;

		struct trans_tlbi_data {
			enum tlbi_kind tlbi_kind;
			u64 page;
			u64 level;
		} tlbi_data;

		struct trans_msr_data {
			enum ghost_sysreg_kind sysreg;
			u64 val;
		} msr_data;
	};
};

enum ghost_abs_kind {
	/**
	 * @GHOST_ABS_LOCK - Acquire a mutex
	 */
	GHOST_ABS_LOCK,

	/**
	 * @GHOST_ABS_UNLOCK - Release a mutex
	 */
	GHOST_ABS_UNLOCK,

	/**
	 * @GHOST_ABS_INIT - Zeroed initialisation of some fresh memory
	 */
	GHOST_ABS_INIT,

	/**
	 * @GHOST_ABS_MEMSET - A C memset() call.
	 */
	GHOST_ABS_MEMSET,
};

struct ghost_abs_step {
	enum ghost_abs_kind kind;
	union {
		struct trans_init_data {
			u64 location;
			u64 size;
		} init_data;

		struct trans_lock_data {
			u64 address;
		} lock_data;

		struct trans_memset_data {
			u64 address;
			u64 size;
			u64 value;
		} memset_data;
	};
};

enum ghost_hint_kind {
	/**
	 * @GHOST_HINT_SET_ROOT_LOCK - Set the lock owning a pgtable root.
	 */
	GHOST_HINT_SET_ROOT_LOCK,

	/**
	 * @GHOST_HINT_SET_OWNER_ROOT - Set the pgtable root which owns a pte
	 */
	GHOST_HINT_SET_OWNER_ROOT,

	/**
	 * @GHOST_HINT_RELEASE_TABLE - Stop tracking a whole table (and subtables recursively)
	 */
	GHOST_HINT_RELEASE_TABLE,

	/**
	 * @GHOST_HINT_SET_PTE_THREAD_OWNER - Set the a thread to be the owner of a PTE (only for leaves)
	 */
	GHOST_HINT_SET_PTE_THREAD_OWNER,
};

struct ghost_hint_step {
	enum ghost_hint_kind kind;
	u64 location;
	u64 value;
};

/**
 * Source location info
 */
struct src_loc {
	const char *file;
	const char *func;
	int lineno;
};

struct casemate_model_step {
	/**
	 * @tid: thread identifier.
	 */
	thread_identifier tid;

	/**
	 * @seq_id: sequence id number of the transition.
	 */
	u64 seq_id;

	/**
	 * @src_loc: string location (path, function name, lineno etc)
	 *           of where the transition happens in the source code.
	 *           For debugging/pretty printing.
	 */
	struct src_loc src_loc;

	enum casemate_model_step_kind kind;
	union {
		struct ghost_hw_step hw_step;
		struct ghost_abs_step abs_step;
		struct ghost_hint_step hint_step;
	};
};

/**
 * initialise_casemate_model() - One-shot initialisation of model state.
 * @opts: a reference to an initial configuration to use during setup.
 * @phys: the start physical address of the memory given to pKVM.
 * @size: the size of the region of physical address space given to pKVM.
 * @sm_virt: the start of the virtual address of the memory the ghost model state can live in
 * @sm_size: the space given for the ghost model memory.
 *
 * `phys` and `size` define the region of memory that the model reserves for its own state.
 *
 * NOTE: After this the target must manually initialise the already-existing pagetable memory with steps.
 */
void initialise_casemate_model(struct casemate_options *opts, phys_addr_t phys, u64 size, unsigned long sm_virt, u64 sm_size);

/**
 * casemate_model_step() - Take a step in the ghost model.
 */
void casemate_model_step(struct casemate_model_step trans);


/* auto-included by Makefile */
//////////////
// Step helpers

#define SRC_LOC (struct src_loc){.file=__FILE__, .lineno=__LINE__, .func=__func__}

/**
 * casemate_cpu_id() - Return current CPU identifier.
 *
 * Users should implement this if they want to use the helper macros.
 */
extern u64 casemate_cpu_id(void);
#define THREAD_ID casemate_cpu_id()

#define casemate_model_step_write(...) __casemate_model_step_write(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_write(u64 tid, struct src_loc src_loc, enum memory_order_t mo, phys_addr_t phys, u64 val)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_HW_STEP,
		.hw_step = (struct ghost_hw_step){
			.kind = HW_MEM_WRITE,
			.write_data = (struct trans_write_data){
				.mo = mo,
				.phys_addr = phys,
				.val = val,
			},
		},
	});
}

#define casemate_model_step_read(...) __casemate_model_step_read(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_read(u64 tid, struct src_loc src_loc, phys_addr_t phys, u64 val)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_HW_STEP,
		.hw_step = (struct ghost_hw_step){
			.kind = HW_MEM_READ,
			.read_data = (struct trans_read_data){
				.phys_addr = phys,
				.val = val,
			},
		},
	});
}

#define casemate_model_step_dsb(...) __casemate_model_step_dsb(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_dsb(u64 tid, struct src_loc src_loc, enum dxb_kind kind)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_HW_STEP,
		.hw_step = (struct ghost_hw_step){
			.kind = HW_BARRIER,
			.barrier_data = (struct trans_barrier_data){
				.kind = BARRIER_DSB,
				.dxb_data = kind,
			},
		},
	});
}

#define casemate_model_step_isb() __casemate_model_step_isb(THREAD_ID, SRC_LOC)
static inline void __casemate_model_step_isb(u64 tid, struct src_loc src_loc)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_HW_STEP,
		.hw_step = (struct ghost_hw_step){
			.kind = HW_BARRIER,
			.barrier_data = (struct trans_barrier_data){
				.kind = BARRIER_ISB,
			},
		},
	});
}

#define casemate_model_step_tlbi3(...) __casemate_model_step_tlbi3(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_tlbi3(u64 tid, struct src_loc src_loc, enum tlbi_kind kind, u64 page, int level)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_HW_STEP,
		.hw_step = (struct ghost_hw_step){
			.kind = HW_TLBI,
			.tlbi_data = (struct trans_tlbi_data){
				.tlbi_kind = kind,
				.page = page,
				.level = level,
			},
		},
	});
}

#define casemate_model_step_tlbi1(...) __casemate_model_step_tlbi1(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_tlbi1(u64 tid, struct src_loc src_loc, enum tlbi_kind kind)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_HW_STEP,
		.hw_step = (struct ghost_hw_step){
			.kind = HW_TLBI,
			.tlbi_data = (struct trans_tlbi_data){
				.tlbi_kind = kind,
			},
		},
	});
}

#define casemate_model_step_msr(...) __casemate_model_step_msr(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_msr(u64 tid, struct src_loc src_loc, enum ghost_sysreg_kind sysreg, u64 val)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_HW_STEP,
		.hw_step = (struct ghost_hw_step){
			.kind = HW_MSR,
			.msr_data = (struct trans_msr_data){
				.sysreg = sysreg,
				.val = val,
			},
		},
	});
}

#define casemate_model_step_hint(...) __casemate_model_step_hint(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_hint(u64 tid, struct src_loc src_loc, enum ghost_hint_kind kind, u64 location, u64 value)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_HINT,
		.hint_step = (struct ghost_hint_step){
			.kind = kind,
			.location = location,
			.value = value,
		},
	});
}

#define casemate_model_step_init(...) __casemate_model_step_init(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_init(u64 tid, struct src_loc src_loc, u64 location, u64 size)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_ABS_STEP,
		.abs_step = (struct ghost_abs_step){
			.kind = GHOST_ABS_INIT,
			.init_data = (struct trans_init_data){
				.location = location,
				.size = size,
			},
		},
	});
}

#define casemate_model_step_memset(...) __casemate_model_step_memset(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_memset(u64 tid, struct src_loc src_loc, u64 location, u64 value, u64 size)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_ABS_STEP,
		.abs_step = (struct ghost_abs_step){
			.kind = GHOST_ABS_MEMSET,
			.memset_data = (struct trans_memset_data){
				.address = location,
				.size = size,
				.value = value,
			},
		},
	});
}

#define casemate_model_step_lock(...) __casemate_model_step_lock(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_lock(u64 tid, struct src_loc src_loc, u64 address)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_ABS_STEP,
		.abs_step = (struct ghost_abs_step){
			.kind = GHOST_ABS_LOCK,
			.lock_data = (struct trans_lock_data){
				.address = address,
			},
		},
	});
}

#define casemate_model_step_unlock(...) __casemate_model_step_unlock(THREAD_ID, SRC_LOC, __VA_ARGS__)
static inline void __casemate_model_step_unlock(u64 tid, struct src_loc src_loc, u64 address)
{
	casemate_model_step((struct casemate_model_step){
		.tid = tid,
		.src_loc = src_loc,
		.kind = TRANS_ABS_STEP,
		.abs_step = (struct ghost_abs_step){
			.kind = GHOST_ABS_UNLOCK,
			.lock_data = (struct trans_lock_data){
				.address = address,
			},
		},
	});
}



#endif /* CASEMATE_H */