/* corto_enum.h
 *
 * This file contains generated code. Do not modify!
 */

#ifndef CORTO_LANG_ENUM_H
#define CORTO_LANG_ENUM_H

#include "corto.h"
#include "corto_primitive.h"
#include "corto__interface.h"

#ifdef __cplusplus
extern "C" {
#endif

CORTO_LANG_EXPORT corto_object _corto_enum_constant(corto_enum _this, corto_int32 value);
#define corto_enum_constant(_this, value) _corto_enum_constant(corto_enum(_this), value)

CORTO_LANG_EXPORT corto_int16 _corto_enum_construct(corto_enum _this);
#define corto_enum_construct(_this) _corto_enum_construct(corto_enum(_this))

CORTO_LANG_EXPORT corto_void _corto_enum_destruct(corto_enum _this);
#define corto_enum_destruct(_this) _corto_enum_destruct(corto_enum(_this))

CORTO_LANG_EXPORT corto_int16 _corto_enum_init(corto_enum _this);
#define corto_enum_init(_this) _corto_enum_init(corto_enum(_this))

#ifdef __cplusplus
}
#endif
#endif

