/* Fast_Character.h
 *
 * This file contains generated code. Do not modify!
 */

#ifndef Fast_Character_H
#define Fast_Character_H

#include "cortex.h"
#include "Fast_Literal.h"
#include "Fast__type.h"

#include "Fast__api.h"

#include "Fast__meta.h"

#ifdef __cplusplus
extern "C" {
#endif

/* callback ::cortex::lang::type::init(lang::object object) -> ::cortex::Fast::Character::init(Character object) */
cx_int16 Fast_Character_init(Fast_Character object);

/* ::cortex::Fast::Character::serialize(lang::type dstType,lang::word dst) */
cx_int16 Fast_Character_serialize(Fast_Character _this, cx_type dstType, cx_word dst);

/* virtual ::cortex::Fast::Character::toIc(lang::alias{"cx_icProgram"} program,lang::alias{"cx_icStorage"} storage,lang::bool stored) */
cx_ic Fast_Character_toIc(Fast_Character _this, cx_icProgram program, cx_icStorage storage, cx_bool stored);

/* ::cortex::Fast::Character::toIc(lang::alias{"cx_icProgram"} program,lang::alias{"cx_icStorage"} storage,lang::bool stored) */
cx_ic Fast_Character_toIc_v(Fast_Character _this, cx_icProgram program, cx_icStorage storage, cx_bool stored);

#ifdef __cplusplus
}
#endif
#endif

