/*
 * corto_object.h
 *
 *  Created on: Aug 2, 2012
 *      Author: sander
 */

#ifndef CORTO_OBJECT_H_
#define CORTO_OBJECT_H_

#include "corto__type.h"
#include "corto_ll.h"
#include "corto_value.h"
#include "corto_async.h"
#include "corto_time.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef char corto_id[512];

typedef int (*corto_scopeWalkAction)(corto_object o, void* userData);
typedef corto_equalityKind ___ (*corto_equalsAction)(corto_type _this, const void* o1, const void* o2);

/* Event-kinds */
#define CORTO_EVENT_NONE       (0)
#define CORTO_EVENT_OBSERVABLE (1)

/* Object lifecycle */
corto_attr corto_setAttr(corto_attr attr);
corto_attr corto_getAttr(void);
corto_object _corto_create(corto_type type);
corto_object _corto_createChild(corto_object parent, corto_string name, corto_type type);
corto_object _corto_declare(corto_type type);
corto_object _corto_declareChild(corto_object parent, corto_string name, corto_type type);
corto_int16 corto_define(corto_object o);
void corto_delete(corto_object o);
void corto_drop(corto_object o);
corto_int32 corto_claim(corto_object o);
corto_int32 corto_release(corto_object o);
void corto_invalidate(corto_object o);
void corto_attach(corto_object parent, corto_object child);
void corto_detach(corto_object parent, corto_object child);

/* Generic object data */
corto_type corto_typeof(corto_object o);
corto_int32 corto_countof(corto_object o);
corto_int8 corto_stateof(corto_object o);
corto_bool corto_checkState(corto_object o, corto_int8 state);
corto_bool corto_checkAttr(corto_object o, corto_int8 attr);
corto_object corto_assertType(corto_type type, corto_object o);
corto_bool _corto_instanceof(corto_type type, corto_object o);

/* Scoped data */
corto_string corto_nameof(corto_object o);
corto_object corto_parentof(corto_object o);
corto_uint32 corto_scopeSize(corto_object o); /* Returns number of objects (non-recursive) in scope */
corto_objectseq corto_scopeClaim(corto_object o); /* Safe way to access scope contents */
void corto_scopeRelease(corto_objectseq scope);
corto_int32 corto_scopeWalk(corto_object o, corto_scopeWalkAction action, void *userData); /* Safe object-walk */
corto_string corto_fullname(corto_object o, corto_id buffer);
corto_string corto_relname(corto_object from, corto_object o, corto_id buffer);

/* Persistent data */
corto_time corto_timestampof(corto_object o);
corto_object corto_ownerof(corto_object o);

/* Find objects by name */
corto_object corto_lookup(corto_object scope, corto_string name);
corto_object corto_resolve(corto_object scope, corto_string expr);

typedef struct corto_selectItem {
	corto_string parent;
	corto_string name;
	corto_string type;
} corto_selectItem;

corto_int16 corto_select(corto_object scope, corto_string expr, corto_iter *iter_out);

/* Notifications */
corto_object corto_setOwner(corto_object owner);
corto_object corto_getOwner(void);
corto_int32 corto_listen(corto_object _this, corto_observer observer, corto_eventMask mask, corto_object observable, corto_dispatcher dispatcher);
corto_int32 corto_silence(corto_object _this, corto_observer observer, corto_eventMask mask, corto_object observable);
corto_bool corto_listening(corto_object observable, corto_observer, corto_object _this);
corto_int32 corto_update(corto_object observable);
corto_int32 corto_updateBegin(corto_object observable);
corto_int32 corto_updateTry(corto_object observable);
corto_int32 corto_updateEnd(corto_object observable);
corto_int32 corto_updateCancel(corto_object observable);

/* Read locking */
corto_int32 corto_readBegin(corto_object object);
corto_int32 corto_readEnd(corto_object object);

/* Write locking */
corto_int32 corto_lock(corto_object object);
corto_int32 corto_unlock(corto_object object);

/* Waiting */
corto_int32 corto_waitfor(corto_object observable);
corto_object corto_wait(corto_int32 timeout_sec, corto_int32 timeout_nanosec);

/* REPL functionality */
corto_int16 corto_expr(corto_object scope, corto_string expr, corto_value *value);

/* Documentation */
corto_object corto_man(corto_object o);

/* Set reference field */
void corto_setref(void* ptr, corto_object value);

/* Set string field */
void corto_setstr(corto_string* ptr, corto_string value);

/* Serialize to string */
corto_string corto_str(corto_object object, corto_uint32 maxLength);
corto_string corto_strv(corto_value* v, corto_uint32 maxLength);
corto_string _corto_strp(void *p, corto_type type, corto_uint32 maxLength);
corto_string corto_stra(corto_any a, corto_uint32 maxLength);

/* Deserialize from string */
corto_int16 corto_fromStr(void *o, corto_string string);
corto_int16 corto_fromStrv(corto_value *v, corto_string string);
corto_int16 _corto_fromStrp(void *p, corto_type type, corto_string string);
corto_int16 corto_fromStra(corto_any *a, corto_string string);

/* Copy */
corto_int16 corto_copy(corto_object *dst, corto_object src);
corto_int16 corto_copyv(corto_value *dst, corto_value *src);
corto_int16 _corto_copyp(void *dst, corto_type type, void *src);
corto_int16 corto_copya(corto_any *dst, corto_any src);

/* Compare */
corto_equalityKind corto_compare(corto_object o1, corto_object o2);
corto_equalityKind corto_comparev(corto_value *v1, corto_value *value2);
corto_equalityKind _corto_comparep(void *p1, corto_type type, void *p2);
corto_equalityKind corto_comparea(corto_any a1, corto_any a2);

/* Init */
corto_int16 corto_init(corto_object o);
corto_int16 corto_initv(corto_value *v);
corto_int16 _corto_initp(void *v, corto_type type);
corto_int16 corto_inita(corto_any a);

/* Deinit */
corto_int16 corto_deinit(corto_object o);
corto_int16 corto_deinitv(corto_value *v);
corto_int16 _corto_deinitp(void *v, corto_type type);
corto_int16 corto_deinita(corto_any a);

/* Macro's that automate casting of parameters */
#define corto_create(type) _corto_create(corto_type(type))
#define corto_createChild(parent, name, type) _corto_createChild(parent, name, corto_type(type))
#define corto_declare(type) _corto_declare(corto_type(type))
#define corto_declareChild(parent, name, type) _corto_declareChild(parent, name, corto_type(type))
#define corto_strp(p, type, maxLength) _corto_strp(p, corto_type(type), maxLength)
#define corto_fromStrp(out, type, string) _corto_fromStrp(out, corto_type(type), string)
#define corto_copyp(p, type, src) _corto_copyp(p, corto_type(type), src)
#define corto_comparep(p1, type, p2) _corto_comparep(p1, corto_type(type), p2)
#define corto_initp(p, type) _corto_initp(p, corto_type(type))
#define corto_deinitp(p, type) _corto_deinitp(p, corto_type(type))
#define corto_instanceof(type, o) _corto_instanceof((corto_type)type, o)

#ifdef __cplusplus
}
#endif

#endif /* CORTO_OBJECT_H_ */
