void hal_init_vm_map(void);
void phantom_timed_call_init(void);


void hal_cpu_reset_real(void) __attribute__((noreturn));

void phantom_threads_init();


void phantom_load_gdt(void);