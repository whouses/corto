/*
 * cx_object.h
 *
 *  Created on: Aug 2, 2012
 *      Author: sander
 */

#ifndef CX_OBJECT_H_
#define CX_OBJECT_H_

#include "cx__type.h"
#include "cx_ll.h"
#include "cx_value.h"
#include "cx_async.h"
#include "cx_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Used to hold the fully scoped name of an object */
typedef char cx_id[512];

/* Action-signature for scopeWalk */
typedef int (*cx_scopeWalkAction)(cx_object o, void* userData);
typedef cx_equalityKind (*cx_equalsAction)(cx_type _this, const void* o1, const void* o2);

/* Event-kinds */
#define CX_EVENT_NONE       (0)
#define CX_EVENT_OBSERVABLE (1)

/* Parameter kinds */
#define CX_PARAMETER_REFERENCE          (1)
#define CX_PARAMETER_FORCEREFERENCE     (2)
#define CX_PARAMETER_WILDCARD           (4)
#define CX_PARAMETER_NULL               (8)

/* Object lifecycle */
cx_attr cx_setAttr(cx_attr attr);
cx_attr cx_getAttr(void);
cx_object cx_create(cx_type type);
cx_object cx_declare(cx_object parent, cx_string name, cx_type type);
cx_int16 cx_define(cx_object o);
cx_int32 cx_claim(cx_object o);
cx_int32 cx_release(cx_object o);
void cx_delete(cx_object o);
void cx_drop(cx_object o);
void cx_invalidate(cx_object o);

void cx_attach(cx_object parent, cx_object child); /* Attach lifecycle of unscoped object to scoped object */
void cx_detach(cx_object parent, cx_object child); /* Detach lifecycle of unscoped object from scoped object */

/* Generic object data */
cx_type cx_typeof(cx_object o);
cx_int32 cx_countof(cx_object o);
cx_int8 cx_stateof(cx_object o);
cx_bool cx_checkState(cx_object o, cx_int8 state);
cx_bool cx_checkAttr(cx_object o, cx_int8 attr);
cx_object cx_assertType(cx_type type, cx_object o);
cx_bool cx_instanceof(cx_type type, cx_object o);

/* Scoped data */
cx_string cx_nameof(cx_object o);
cx_object cx_parentof(cx_object o);
cx_uint32 cx_scopeSize(cx_object o); /* Returns number of objects (non-recursive) in scope */
cx_ll cx_scopeClaim(cx_object o); /* Safe way to access scope contents */
void cx_scopeRelease(cx_ll scope);
cx_int32 cx_scopeWalk(cx_object o, cx_scopeWalkAction action, void* userData); /* Safe object-walk */
cx_string cx_fullname(cx_object o, cx_id buffer);
cx_string cx_relname(cx_object from, cx_object o, cx_id buffer);

/* Persistent data */
cx_time cx_timestampof(cx_object o);

/* Lookup objects either using names or fully qualified names. */
cx_object cx_lookup(cx_object scope, cx_string name);
cx_function cx_lookupFunction(cx_object scope, cx_string requested, cx_int32 *d);
cx_object cx_resolve(cx_object scope, cx_string expr);

/* Notifications */
cx_object cx_setSource(cx_object source);
cx_int32 cx_listen(cx_object observable, cx_observer observer, cx_object _this);
cx_int32 cx_silence(cx_object observable, cx_observer observer, cx_object _this);
cx_bool cx_listening(cx_object observable, cx_observer, cx_object _this);
cx_int32 cx_update(cx_object observable);
cx_int32 cx_updateBegin(cx_object observable);
cx_int32 cx_updateTry(cx_object observable);
cx_int32 cx_updateEnd(cx_object observable);
cx_int32 cx_updateCancel(cx_object observable);

/* Waiting */
cx_int32 cx_waitfor(cx_object observable);
cx_object cx_wait(cx_int32 timeout_sec, cx_int32 timeout_nanosec);

/* REPL functionality */
cx_int16 cx_expr(cx_object scope, cx_string expr, cx_value *value);

/* Thread-safe reading */
cx_int32 cx_readBegin(cx_object object);
cx_int32 cx_readEnd(cx_object object);

/* Locking */
cx_int32 cx_lock(cx_object object);
cx_int32 cx_unlock(cx_object object);

/* Set reference field */
void cx_setref(void* ptr, cx_object value);

/* Set string field */
void cx_setstr(cx_string* ptr, cx_string value);

/* Calculate to what extent a function meets requested signature */
cx_int16 cx_overload(cx_object object, cx_string name, cx_int32* distance);

/* Obtain information from signature.
 *   Signatures can be of the following form:
 *     name(type name,type name)
 *     name(type,type)
 *     name --> Only allowed for non-overloaded functions
 *
 *   No extra whitespaces are allowed.
 */
cx_int32 cx_signatureName(cx_string signature, cx_id buffer);
cx_int32 cx_signatureParamCount(cx_string signature);
cx_int32 cx_signatureParamName(cx_string signature, cx_uint32 id, cx_id buffer);
cx_int32 cx_signatureParamType(cx_string signature, cx_uint32 id, cx_id buffer, int* reference);

/* Create request signature */
cx_string cx_signatureOpen(cx_string name);
cx_string cx_signatureAdd(cx_string sig, cx_type type, int flags);
cx_string cx_signatureAddWildcard(cx_string sig, cx_bool isReference);
cx_string cx_signatureClose(cx_string sig);

/* Obtain signature from object */
cx_int16 cx_signature(cx_object o, cx_id buffer);

/* Parse member expression */
cx_value* cx_parseExpr(cx_object o, cx_string expr, cx_valueStack stack, cx_uint32 *count);

/* Serialize object to string */
cx_string cx_toString(cx_object object, cx_uint32 maxLength);

/* Deserialize object from string */
cx_object cx_fromString(cx_string string);

/* Convert value to string */
cx_string cx_valueToString(cx_value* v, cx_uint32 maxLength);

/* Deserialize value from string */
void *cx_valueFromString(cx_string string, void* out, cx_type type);

/* Compare objects */
cx_equalityKind cx_compare(cx_object o1, cx_object o2);

/* Compare value */
cx_equalityKind cx_valueCompare(cx_value *value1, cx_value *value2);
    
/* Init object */
cx_int16 cx_init(cx_object o);
    
/* Init value */
cx_int16 cx_initValue(cx_value *v);
    
/* Deinit object */
cx_int16 cx_deinit(cx_object o);
    
/* Deinit value */
cx_int16 cx_deinitValue(cx_value *v);
    
/* Copy object */
cx_int16 cx_copy(cx_object *dst, cx_object src);
 
/* Copy value */
cx_int16 cx_valueCopy(cx_value *dst, cx_value *src);

#ifdef __cplusplus
}
#endif

#endif /* CX_OBJECT_H_ */
