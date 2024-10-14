#include <casemate-impl.h>

//////////////////////////
// Initialisation

struct casemate_options sm_options;
struct ghost_driver driver;

struct casemate_model_state *the_ghost_state;
struct casemate_model_state *the_ghost_state_pre;
bool is_initialised = false;

/**
 * opts() - Get model options.
 */
struct casemate_options *opts()
{
	return &sm_options;
}

/**
 * side_effect() - Perform a side-effect using the ghost driver.
 */
struct ghost_driver *side_effect()
{
	return &driver;
}

void initialise_ghost_driver(struct ghost_driver *d)
{
	/* copy their driver into ours
	 * so we don't have a reference to some unstable state */
	driver = *d;
}

void initialise_casemate_model(struct casemate_options *cfg, phys_addr_t phys, u64 size, unsigned long sm_virt, u64 sm_size)
{

#ifndef CONFIG_NVHE_casemate_model_LOG_ONLY
	lock_sm();
	GHOST_LOG_CONTEXT_ENTER();

	the_ghost_state = (struct casemate_model_state*)sm_virt;
	the_ghost_state_pre = the_ghost_state + 1;
	transition_id = 0;

	/* copy their configuration into ours
	 * so we don't have a reference to some unstable state */
	*opts() = *cfg;
	initialise_ghost_ptes_memory(phys, size);

	GHOST_LOG_CONTEXT_EXIT();
	unlock_sm();
#endif /* CONFIG_NVHE_casemate_model_LOG_ONLY */
}
