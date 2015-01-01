/*
 * cx_object.h
 *
 *  Created on: Aug 2, 2012
 *      Author: sander
 */

#ifndef DB_OBJECT_H_
#define DB_OBJECT_H_

#include "cx__type.h"
#include "cx_ll.h"
#include "cx_value.h"
#include "cx_rbtree.h"
#include "cx_async.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Used to hold the fully scoped name of an object */
typedef char cx_id[512];

/* Action-signature for scopeWalk */
typedef int (*cx_scopeWalkAction)(cx_object o, void* userData);

/* Object attribute flags */
#define DB_ATTR_SCOPED      (1)
#define DB_ATTR_WRITABLE    (2)
#define DB_ATTR_OBSERVABLE  (4)

/* Object state flags */
#define DB_VALID      (1)
#define DB_DECLARED   (2)
#define DB_DEFINED    (4)
#define DB_DESTRUCTED (8)

/* Object event flags */
#define DB_ON_DECLARE  			(1)
#define DB_ON_DEFINE 	(2)
#define DB_ON_DESTRUCT 		(4)
#define DB_ON_INVALIDATE 	(8)
#define DB_ON_UPDATE 		(16)
#define DB_ON_SELF 			(32)
#define DB_ON_SCOPE 		(64)
#define DB_ON_VALUE         (128)
#define DB_ON_METAVALUE     (256)

/* Event-kinds */
#define DB_EVENT_NONE		(0)
#define DB_EVENT_OBSERVABLE (1)

/* Object lifecycle */
cx_object cx_new(cx_typedef type);
cx_object cx_new_ext(cx_object src, cx_typedef type, cx_uint8 attrs, cx_string context);
cx_object cx_declare(cx_object parent, cx_string name, cx_typedef type);
cx_int16 cx_define(cx_object o);
void cx_destruct(cx_object o);
void cx_attach(cx_object parent, cx_object child); /* Attach lifecycle of unscoped object to scoped object */
void cx_detach(cx_object parent, cx_object child); /* Detach lifecycle of unscoped object from scoped object */
void cx_invalidate(cx_object o);

/* Object-data */
cx_typedef cx_typeof(cx_object o);
cx_int32 cx_countof(cx_object o);
cx_bool cx_checkState(cx_object o, cx_int8 state);
cx_bool cx_checkAttr(cx_object o, cx_int8 attr);
cx_bool cx_instanceof(cx_typedef type, cx_object o);

/* Scoped object-data */
cx_string cx_nameof(cx_object o);
cx_object cx_parentof(cx_object o);
cx_rbtree cx_scopeof(cx_object o); /* Dangerous function, because it's not protected by locks */
cx_uint32 cx_scopeSize(cx_object o); /* Returns number of objects (non-recursive) in scope */
cx_ll cx_scopeClaim(cx_object o); /* Safe way to access scope contents */
void cx_scopeRelease(cx_ll scope);
cx_int32 cx_scopeWalk(cx_object o, cx_scopeWalkAction action, void* userData); /* Safe object-walk */
cx_string cx_fullname(cx_object o, cx_id buffer);
cx_string cx_relname(cx_object from, cx_object o, cx_id buffer);

/* Resource management */
cx_int32 cx_keep(cx_object o);
cx_int32 cx_free(cx_object o);
cx_int32 cx_free_ext(cx_object src, cx_object o, cx_string context);
cx_int32 cx_keep_ext(cx_object src, cx_object o, cx_string context);
void cx_drop(cx_object o); /* Free all non-orphaned objects in scope. Object self is not free'd. */

/* Lookup objects either using names or fully qualified names. */
cx_object cx_lookup(cx_object scope, cx_string name);
cx_object cx_lookup_ext(cx_object src, cx_object scope, cx_string name, cx_string context);
cx_function cx_lookupFunction(cx_object scope, cx_string requested, cx_bool allowCastableOverloading, cx_int32 *d);
cx_function cx_lookupFunction_ext(cx_object src, cx_object scope, cx_string requested, cx_bool allowCastableOverloading, cx_int32 *d, cx_string context);
cx_object cx_resolve(cx_object scope, cx_string expr);
cx_object cx_resolve_ext(cx_object src, cx_object scope, cx_string expr, cx_bool allowCastableOverloading, cx_string context);

/* Notifications */
cx_int32 cx_listen(cx_object observable, cx_observer observer, cx_object _this);
cx_int32 cx_silence(cx_object observable, cx_observer observer, cx_object _this);
cx_bool cx_listening(cx_object observable, cx_observer, cx_object _this);
cx_int32 cx_update(cx_object observable);
cx_int32 cx_updateFrom(cx_object observable, cx_object _this);
cx_int32 cx_updateBegin(cx_object observable);
cx_int32 cx_updateTry(cx_object observable);
cx_int32 cx_updateEnd(cx_object observable);
cx_int32 cx_updateEndFrom(cx_object observable, cx_object _this);
cx_int32 cx_updateCancel(cx_object observable);

/* Waiting */
cx_int32 cx_waitfor(cx_object observable);
cx_object cx_wait(cx_int32 timeout_sec, cx_int32 timeout_nanosec);

/* Thread-safe reading (polling) */
cx_int32 cx_readBegin(cx_object object);
cx_int32 cx_readEnd(cx_object object);

/* Set reference field */
void cx_set(void* ptr, cx_object value);
void cx_set_ext(cx_object source, void* ptr, cx_object value, cx_string context);

/* Measure to what extend a function meets requested signature */
cx_int16 cx_overload(cx_function object, cx_string name, cx_int32* distance, cx_bool allowCastable);

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
cx_int32 cx_signatureParamType(cx_string signature, cx_uint32 id, cx_id buffer, cx_bool* reference);

/* Create request signature */
cx_string cx_signatureOpen(cx_string name);
cx_string cx_signatureAdd(cx_string sig, cx_typedef type, cx_bool isReference);
cx_string cx_signatureAddWildcard(cx_string sig, cx_bool isReference);
cx_string cx_signatureClose(cx_string sig);

/* Parse member expression */
cx_value* cx_parseExpr(cx_object o, cx_string expr, cx_valueStack stack, cx_uint32 *count);

/* Serialize object to string */
cx_string cx_toString(cx_object object, cx_uint32 maxLength);

/* Deserialize object from string */
cx_object cx_fromString(cx_string string);

/* Convert value to string */
cx_string cx_valueToString(cx_value* v, cx_uint32 maxLength);

/* Deserialize value from string */
void *cx_valueFromString(cx_string string, void* out, cx_typedef type);

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
cx_int16 cx_copy(cx_object from, cx_object to);
 
/* Copy value */
cx_int16 cx_valueCopy(cx_value *from, cx_value *to);

#ifdef __cplusplus
}
#endif

#endif /* DB_OBJECT_H_ */
