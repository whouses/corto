/*
 * corto_metawalk.c

 *
 *  Created on: Aug 28, 2012
 *      Author: sander
 */

#define corto_lang_LIB
#include "corto_metawalk.h"
#include "corto__object.h" /* To mimic an object */
#include "corto_err.h"
#include "corto_util.h"
#include "corto_mem.h"
#include "string.h"

/* Do metawalk on type */
corto_int16 corto_metaWalk(corto_serializer s, corto_type type, void* userData) {
    corto__object* o;
    corto_int16 result;

    /* Instantiate dummy-object */
    o = corto_alloc(sizeof(corto__object) + type->size); /* alloca is dangerous here because objects can get large, causing stack overflows. */
    memset(o, 0, sizeof(corto__object) + type->size);
    o->type = corto_type(type);
    o->refcount = 1;

    result = corto_serialize(s, CORTO_OFFSET(o, sizeof(corto__object)), userData);
    corto_dealloc(o);

    return result;
}

/* Serialize constants of enumeration */
corto_int16 corto_serializeConstants(corto_serializer s, corto_value* v, void* userData) {
    corto_enum t;
    corto_uint32 i;

    t = corto_enum(corto_valueType(v));

    /* If there is a callback for constants, serialize them */
    if (s->metaprogram[CORTO_CONSTANT]) {
        corto_value info;
        for(i=0; i<t->constants.length; i++) {
            /* Fill info */
            info.parent = v;
            info.kind = CORTO_CONSTANT;
            info.is.constant.t = t->constants.buffer[i];
            info.is.constant.v = NULL;
            info.is.constant.o = corto_valueObject(v);

            /* Serialize constant */
            if (s->metaprogram[CORTO_CONSTANT](s, &info, userData)) {
                goto error;
            }
        }
    }

    return 0;
error:
    return -1;
}
