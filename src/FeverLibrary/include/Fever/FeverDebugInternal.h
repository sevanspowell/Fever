/*
// Store allocations and check if free on renderer shutdown, print errors
with
file, line
extern Buffer *fvr_buffer_setData_debug(const char *file, uint32_t line);

#define fvr_buffer_create() fvr_buffer_setData_debug(__FILE__, __LINE__)
*/
