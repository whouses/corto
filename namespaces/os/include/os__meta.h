/* os__meta.h
 *
 * Loads objects in database.
 * This file contains generated code. Do not modify!
 */

#ifndef os_META_H
#define os_META_H

#include "cortex.h"

#include "os__type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ::cortex */
extern cx_object _o;

/* ::cortex::os */
extern cx_void (*os_o);

/* ::cortex::os::exit(::cortex::lang::bool success) */
extern cx_function os_exit_o;

/* ::cortex::os::loadavg(::cortex::os::loadAvgKind kind) */
extern cx_function os_loadavg_o;

/* ::cortex::os::loadAvgKind */
extern cx_enum os_loadAvgKind_o;

/* ::cortex::os::loadAvgKind::MIN_1 */
extern cx_constant (*os_loadAvgKind_MIN_1_o);

/* ::cortex::os::loadAvgKind::MIN_15 */
extern cx_constant (*os_loadAvgKind_MIN_15_o);

/* ::cortex::os::loadAvgKind::MIN_5 */
extern cx_constant (*os_loadAvgKind_MIN_5_o);

/* ::cortex::os::sleep(::cortex::lang::uint32 sec,::cortex::lang::uint32 nsec) */
extern cx_function os_sleep_o;

/* ::cortex::os::system(::cortex::lang::string cmd) */
extern cx_function os_system_o;

/* ::cortex::os::thread */
extern cx_class os_thread_o;

/* ::cortex::os::thread::construct(::cortex::os::thread object) */
extern cx_callback os_thread_construct_o;

/* ::cortex::os::thread::destruct(::cortex::os::thread object) */
extern cx_callback os_thread_destruct_o;

/* ::cortex::os::thread::handle */
extern cx_member os_thread_handle_o;

/* ::cortex::os::thread::join() */
extern cx_method os_thread_join_o;

/* ::cortex::os::thread::run() */
extern cx_virtual os_thread_run_o;

/* ::cortex::os::thread::start() */
extern cx_method os_thread_start_o;

/* ::cortex::os::thread::stop() */
extern cx_method os_thread_stop_o;

/* ::cortex::os::thread::stopping */
extern cx_member os_thread_stopping_o;

/* ::cortex::os::time */
extern cx_class os_time_o;

/* ::cortex::os::time::add(::cortex::os::time t) */
extern cx_method os_time_add_o;

/* ::cortex::os::time::get() */
extern cx_method os_time_get_o;

/* ::cortex::os::time::nanoseconds */
extern cx_member os_time_nanoseconds_o;

/* ::cortex::os::time::seconds */
extern cx_member os_time_seconds_o;

/* ::cortex::os::time::sub(::cortex::os::time t) */
extern cx_method os_time_sub_o;

/* ::cortex::os::time::toFloat() */
extern cx_method os_time_toFloat_o;

/* ::cortex::os::timer */
extern cx_class os_timer_o;

/* ::cortex::os::timer::nanosec */
extern cx_member os_timer_nanosec_o;

/* ::cortex::os::timer::run() */
extern cx_method os_timer_run_o;

/* ::cortex::os::timer::sec */
extern cx_member os_timer_sec_o;

/* ::cortex::os::timer::stop() */
extern cx_method os_timer_stop_o;

int os_load(void);
#ifdef __cplusplus
}
#endif
#endif

