/*
 * cx_value.h
 *
 *  Created on: Aug 23, 2012
 *      Author: sander
 */

#ifndef DB_VALUE_H_
#define DB_VALUE_H_

#include "cx__type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum cx_valueKind {
    DB_OBJECT = 0,
    DB_BASE = 1, /* serialize inheritance relation */
    DB_VALUE = 2,
    DB_LITERAL = 3,
    DB_MEMBER = 4,
    DB_CALL = 5,
    DB_ELEMENT = 6,
    DB_MAP_ELEMENT = 7,
    DB_CONSTANT = 8/* must be last */
}cx_valueKind;

typedef enum cx_literalKind {
	DB_LITERAL_BOOLEAN,
	DB_LITERAL_CHARACTER,
	DB_LITERAL_INTEGER,
	DB_LITERAL_UNSIGNED_INTEGER,
	DB_LITERAL_FLOATING_POINT,
	DB_LITERAL_STRING,
	DB_LITERAL_NULL
}cx_literalKind;

/* cx_value
 *  Structure that is capable of expressing values in a metadata-organized stack. Used by serializer and as expression result.
 */
typedef struct cx_value cx_value;
struct cx_value {
    cx_value* parent;
    cx_valueKind kind;
    union {
        cx_object o;
        struct {
            cx_object o;
            cx_typedef t;
            cx_void *v;
        }base;
        struct {
            cx_object o;
            cx_typedef t;
            cx_void* v;
            cx_uint64 storage; /* Optional storage for a value. */
        }value;
        struct {
        	cx_literalKind kind;
        	union {
        		cx_bool _boolean;
        		cx_char _character;
        		cx_int64 _integer;
        		cx_uint64 _unsigned_integer;
        		cx_float64 _floating_point;
        		cx_string _string;
        	}v;
        }literal;
        struct {
            cx_object o;
            cx_member t;
            cx_void* v;
        }member;
        struct {
        	cx_object o;
        	cx_function t;
        }call;
        struct {
            cx_object o;
            cx_constant* t;
            cx_void* v;
        }constant;
        struct {
            cx_object o;
            struct {
                cx_typedef type;
                cx_uint32 index;
            } t;
            cx_void* v;
        }element;
        struct {
            cx_object o;
            struct {
                cx_typedef type;
                cx_typedef keyType;
                cx_void *key;
            }t;
            cx_void* v;
        }mapElement;
    } is;
};

typedef cx_value cx_valueStack[64];

cx_typedef cx_valueType(cx_value* val);
cx_void* cx_valueValue(cx_value* val);
cx_object cx_valueObject(cx_value* val);
cx_uint32 cx_valueIndex(cx_value* val);
cx_function cx_valueFunction(cx_value* val);
cx_void *cx_valueThis(cx_value* val);
char* cx_valueString(cx_value* val, char* buffer, unsigned int length);
char* cx_valueExpr(cx_value* val, char* buffer, unsigned int length);
void cx_valueFree(cx_value* val);
void cx_valueStackFree(cx_value* valueStack, cx_uint32 count);

/* Initializers */
void cx_valueObjectInit(cx_value* val, cx_object o);
void cx_valueBaseInit(cx_value* val, cx_void *v, cx_typedef t);
void cx_valueValueInit(cx_value* val, cx_object o, cx_typedef t, cx_void* v);
void cx_valueMemberInit(cx_value* val, cx_object o, cx_member t, cx_void* v);
void cx_valueCallInit(cx_value* val, cx_object o, cx_function t);
void cx_valueConstantInit(cx_value* val, cx_object o, cx_constant* c, cx_void* v);
void cx_valueElementInit(cx_value* val, cx_object o, cx_typedef t, cx_uint32 index, cx_void* v);
void cx_valueMapElementInit(cx_value* val, cx_object o, cx_typedef t, cx_typedef keyType, cx_void *key, cx_void* v);
void cx_valueLiteralInit(cx_value* val, cx_literalKind kind, cx_void* value);

/* Helpers */
void cx_valueSetValue(cx_value* val, cx_void* v);

#ifdef __cplusplus
}
#endif

#endif /* DB_VALUE_H_ */