/* Fast_FloatingPoint.h
 *
 * This file contains generated code. Do not modify!
 */

#ifndef Fast_FloatingPoint_H
#define Fast_FloatingPoint_H

#include "cortex.h"
#include "Fast_Literal.h"
#include "Fast__type.h"

#include "Fast__api.h"

#include "Fast__meta.h"

#ifdef __cplusplus
extern "C" {
#endif

/* callback ::cortex::lang::type::init(lang::object object) -> ::cortex::Fast::FloatingPoint::init(FloatingPoint object) */
cx_int16 Fast_FloatingPoint_init(Fast_FloatingPoint object);

/* ::cortex::Fast::FloatingPoint::serialize(lang::type dstType,lang::word dst) */
cx_int16 Fast_FloatingPoint_serialize(Fast_FloatingPoint _this, cx_type dstType, cx_word dst);

/* virtual ::cortex::Fast::FloatingPoint::toIc(lang::alias{"cx_icProgram"} program,lang::alias{"cx_icStorage"} storage,lang::bool stored) */
cx_ic Fast_FloatingPoint_toIc(Fast_FloatingPoint _this, cx_icProgram program, cx_icStorage storage, cx_bool stored);

/* ::cortex::Fast::FloatingPoint::toIc(lang::alias{"cx_icProgram"} program,lang::alias{"cx_icStorage"} storage,lang::bool stored) */
cx_ic Fast_FloatingPoint_toIc_v(Fast_FloatingPoint _this, cx_icProgram program, cx_icStorage storage, cx_bool stored);

#ifdef __cplusplus
}
#endif
#endif

