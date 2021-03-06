/*
 * corto_operator.c
 *
 *  Created on: Aug 7, 2013
 *      Author: sander
 */

#include "corto.h"
#include "corto__meta.h"
#include "corto__primitive.h"

#ifdef CORTO_OPERATORS

typedef void (*corto__unaryOperator)(void* operand, void* result);
typedef void (*corto__binaryOperator)(void* operand1, void* operand2, void* result);

static corto__unaryOperator corto_unaryOps[CORTO_PRIMITIVE_MAX_CONVERTID+1][CORTO_SHIFT_RIGHT+1];
static corto__binaryOperator corto_binaryOps[CORTO_PRIMITIVE_MAX_CONVERTID+1][CORTO_SHIFT_RIGHT+1];

#define CORTO_NAME_UNARYOP(type, name) __corto_##type##_##name##_unaryOp
#define CORTO_NAME_BINARYOP(type, name) __corto_##type##_##name##_binaryOp

#define CORTO_NUMERIC_UNARY_OP(type, _op, name)\
void CORTO_NAME_UNARYOP(type,name)(void* op, void* result) {\
        *(corto_##type*)result = _op *(corto_##type*)op;\
}

#define CORTO_NUMERIC_BINARY_OP(type, op, name)\
static void CORTO_NAME_BINARYOP(type,name)(void* op1, void* op2, void* result) {\
    *(corto_##type*)result = *(corto_##type*)op1 op *(corto_##type*)op2;\
}

#define CORTO_NUMERIC_COND_UNARY_OP(type, op, name)\
static void CORTO_NAME_UNARYOP(type,name)(void* op, void* result) {\
    *(corto_bool*)result = op *(corto_##type*)op;\
}

#define CORTO_NUMERIC_COND_BINARY_OP(type, op, name)\
static void CORTO_NAME_BINARYOP(type,name)(void* op1, void* op2, void* result) {\
    *(corto_bool*)result = *(corto_##type*)op1 op *(corto_##type*)op2;\
}

static void CORTO_NAME_BINARYOP(string,cond_eq)(void* op1, void* op2, void* result) {
    *(corto_bool*)result = !strcmp(*(corto_string*)op1, *(corto_string*)op2);
}
static void CORTO_NAME_BINARYOP(string,cond_neq)(void* op1, void* op2, void* result) {
    *(corto_bool*)result = strcmp(*(corto_string*)op1, *(corto_string*)op2) != 0;
}
static void CORTO_NAME_BINARYOP(string,add)(void* op1, void* op2, void* result) {
    corto_uint32 len = strlen(*(corto_string*)op1) + strlen(*(corto_string*)op2);
    if (*(corto_string*)result) {
        corto_dealloc(*(corto_string*)result);
    }
    *(corto_string*)result = corto_alloc(len + 1);
    sprintf(*(corto_string*)result, "%s%s", *(corto_string*)op1, *(corto_string*)op2);
}


#define CORTO_INTEGER_UNARY_OPS(type) \
CORTO_NUMERIC_UNARY_OP(type, ++, inc)\
CORTO_NUMERIC_UNARY_OP(type, --, dec)\
CORTO_NUMERIC_UNARY_OP(type, -, sign)\
CORTO_NUMERIC_UNARY_OP(type, ~, not)\
CORTO_NUMERIC_UNARY_OP(type, !, cond_not)

#define CORTO_FLOAT_UNARY_OPS(type) \
CORTO_NUMERIC_UNARY_OP(type, ++, inc)\
CORTO_NUMERIC_UNARY_OP(type, --, dec)\
CORTO_NUMERIC_UNARY_OP(type, -, sign)\
CORTO_NUMERIC_UNARY_OP(type, !, cond_not)\

#define CORTO_INTEGER_BINARY_OPS(type) \
CORTO_NUMERIC_BINARY_OP(type, =, assign)\
CORTO_NUMERIC_BINARY_OP(type, +=, assign_add)\
CORTO_NUMERIC_BINARY_OP(type, -=, assign_sub)\
CORTO_NUMERIC_BINARY_OP(type, *=, assign_mul)\
CORTO_NUMERIC_BINARY_OP(type, /=, assign_div)\
CORTO_NUMERIC_BINARY_OP(type, %=, assign_mod)\
CORTO_NUMERIC_BINARY_OP(type, |=, assign_or)\
CORTO_NUMERIC_BINARY_OP(type, ^=, assign_xor)\
CORTO_NUMERIC_BINARY_OP(type, &=, assign_and)\
CORTO_NUMERIC_BINARY_OP(type, +, add)\
CORTO_NUMERIC_BINARY_OP(type, -, sub)\
CORTO_NUMERIC_BINARY_OP(type, *, mul)\
CORTO_NUMERIC_BINARY_OP(type, /, div)\
CORTO_NUMERIC_BINARY_OP(type, %, mod)\
CORTO_NUMERIC_BINARY_OP(type, |, or)\
CORTO_NUMERIC_BINARY_OP(type, ^, xor)\
CORTO_NUMERIC_BINARY_OP(type, &, and)\
CORTO_NUMERIC_BINARY_OP(type, ^, not)\
CORTO_NUMERIC_COND_BINARY_OP(type, ==, cond_eq)\
CORTO_NUMERIC_COND_BINARY_OP(type, !=, cond_neq)\
CORTO_NUMERIC_COND_BINARY_OP(type, <, cond_lt)\
CORTO_NUMERIC_COND_BINARY_OP(type, >, cond_gt)\
CORTO_NUMERIC_COND_BINARY_OP(type, <=, cond_lteq)\
CORTO_NUMERIC_COND_BINARY_OP(type, >=, cond_gteq)\
CORTO_NUMERIC_COND_BINARY_OP(type, &&, cond_and)\
CORTO_NUMERIC_COND_BINARY_OP(type, ||, cond_or)

#define CORTO_FLOAT_BINARY_OPS(type) \
CORTO_NUMERIC_BINARY_OP(type, =, assign)\
CORTO_NUMERIC_BINARY_OP(type, +=, assign_add)\
CORTO_NUMERIC_BINARY_OP(type, -=, assign_sub)\
CORTO_NUMERIC_BINARY_OP(type, *=, assign_mul)\
CORTO_NUMERIC_BINARY_OP(type, /=, assign_div)\
CORTO_NUMERIC_BINARY_OP(type, +, add)\
CORTO_NUMERIC_BINARY_OP(type, -, sub)\
CORTO_NUMERIC_BINARY_OP(type, *, mul)\
CORTO_NUMERIC_BINARY_OP(type, /, div)\
CORTO_NUMERIC_COND_BINARY_OP(type, ==, cond_eq)\
CORTO_NUMERIC_COND_BINARY_OP(type, !=, cond_neq)\
CORTO_NUMERIC_COND_BINARY_OP(type, <, cond_lt)\
CORTO_NUMERIC_COND_BINARY_OP(type, >, cond_gt)\
CORTO_NUMERIC_COND_BINARY_OP(type, <=, cond_lteq)\
CORTO_NUMERIC_COND_BINARY_OP(type, >=, cond_gteq)\
CORTO_NUMERIC_COND_BINARY_OP(type, &&, cond_and)\
CORTO_NUMERIC_COND_BINARY_OP(type, ||, cond_or)

#define CORTO_INTEGER_OPS(type) \
CORTO_INTEGER_UNARY_OPS(type) \
CORTO_INTEGER_BINARY_OPS(type)

#define CORTO_FLOAT_OPS(type)\
CORTO_FLOAT_UNARY_OPS(type)\
CORTO_FLOAT_BINARY_OPS(type)

/* Boolean operator implementation */
CORTO_NUMERIC_UNARY_OP(bool, !, cond_not)
CORTO_NUMERIC_BINARY_OP(bool, ==, cond_eq)
CORTO_NUMERIC_BINARY_OP(bool, !=, cond_neq)
CORTO_NUMERIC_BINARY_OP(bool, ||, cond_or)
CORTO_NUMERIC_BINARY_OP(bool, &&, cond_and)

/* Integer operator implementations */
CORTO_INTEGER_OPS(octet)
CORTO_INTEGER_OPS(word)
CORTO_INTEGER_OPS(int8)
CORTO_INTEGER_OPS(int16)
CORTO_INTEGER_OPS(int32)
CORTO_INTEGER_OPS(int64)
CORTO_INTEGER_OPS(uint8)
CORTO_INTEGER_OPS(uint16)
CORTO_INTEGER_OPS(uint32)
CORTO_INTEGER_OPS(uint64)

/* Float operator implementations */
CORTO_FLOAT_OPS(float32)
CORTO_FLOAT_OPS(float64)

#define CORTO_UNARY_OP_INIT(typeKind, typeWidth, operatorKind, type, name)\
        corto_unaryOps[corto__primitive_convertId(typeKind, typeWidth)][operatorKind] = CORTO_NAME_UNARYOP(type, name);

#define CORTO_BINARY_OP_INIT(typeKind, typeWidth, operatorKind, type, name)\
    corto_binaryOps[corto__primitive_convertId(typeKind, typeWidth)][operatorKind] = CORTO_NAME_BINARYOP(type, name);

#define CORTO_STRING_OP_INIT(operatorKind, name)\
    corto_binaryOps[corto__primitive_convertId(CORTO_TEXT, CORTO_WIDTH_WORD)][operatorKind] = CORTO_NAME_BINARYOP(string, name);

#define CORTO_INTEGER_UNARY_OPS_INIT(typeKind, typeWidth, type)\
CORTO_UNARY_OP_INIT(typeKind, typeWidth, CORTO_INC, type, inc)\
CORTO_UNARY_OP_INIT(typeKind, typeWidth, CORTO_DEC, type, dec)\
CORTO_UNARY_OP_INIT(typeKind, typeWidth, CORTO_SUB, type, sign)\
CORTO_UNARY_OP_INIT(typeKind, typeWidth, CORTO_NOT, type, not)\
CORTO_UNARY_OP_INIT(typeKind, typeWidth, CORTO_COND_NOT, type, cond_not)

#define CORTO_INTEGER_BINARY_OPS_INIT(typeKind, typeWidth, type)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN, type, assign)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_ADD, type, assign_add)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_SUB, type, assign_sub)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_MUL, type, assign_mul)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_DIV, type, assign_div)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_MOD, type, assign_mod)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_OR, type, assign_or)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_XOR, type, assign_xor)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_AND, type, assign_and)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ADD, type, add)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_SUB, type, sub)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_MUL, type, mul)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_DIV, type, div)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_MOD, type, mod)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_OR, type, or)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_XOR, type, xor)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_AND, type, and)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_NOT, type, not)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_EQ, type, cond_eq)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_NEQ, type, cond_neq)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_LT, type, cond_lt)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_GT, type, cond_gt)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_LTEQ, type, cond_lteq)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_GTEQ, type, cond_gteq)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_AND, type, cond_and)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_OR, type, cond_or)

#define CORTO_FLOAT_UNARY_OPS_INIT(typeKind, typeWidth, type)\
CORTO_UNARY_OP_INIT(typeKind, typeWidth, CORTO_INC, type, inc)\
CORTO_UNARY_OP_INIT(typeKind, typeWidth, CORTO_DEC, type, dec)\
CORTO_UNARY_OP_INIT(typeKind, typeWidth, CORTO_SUB, type, sign)\
CORTO_UNARY_OP_INIT(typeKind, typeWidth, CORTO_COND_NOT, type, cond_not)

#define CORTO_FLOAT_BINARY_OPS_INIT(typeKind, typeWidth, type)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN, type, assign)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_ADD, type, assign_add)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_SUB, type, assign_sub)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_MUL, type, assign_mul)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ASSIGN_DIV, type, assign_div)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_ADD, type, add)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_SUB, type, sub)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_MUL, type, mul)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_DIV, type, div)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_EQ, type, cond_eq)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_NEQ, type, cond_neq)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_LT, type, cond_lt)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_GT, type, cond_gt)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_LTEQ, type, cond_lteq)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_GTEQ, type, cond_gteq)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_AND, type, cond_and)\
CORTO_BINARY_OP_INIT(typeKind, typeWidth, CORTO_COND_OR, type, cond_or)

#define CORTO_INTEGER_OPS_INIT(typeKind, typeWidth, type) \
CORTO_INTEGER_UNARY_OPS_INIT(typeKind, typeWidth, type)\
CORTO_INTEGER_BINARY_OPS_INIT(typeKind, typeWidth, type)

#define CORTO_FLOAT_OPS_INIT(typeKind, typeWidth, type) \
CORTO_FLOAT_UNARY_OPS_INIT(typeKind, typeWidth, type)\
CORTO_FLOAT_BINARY_OPS_INIT(typeKind, typeWidth, type)

#define CORTO_STRING_OPS_INIT()\
CORTO_STRING_OP_INIT(CORTO_COND_EQ, cond_eq)\
CORTO_STRING_OP_INIT(CORTO_COND_NEQ, cond_neq)\
CORTO_STRING_OP_INIT(CORTO_ADD, add)


void corto_operatorInit(void) {
    CORTO_UNARY_OP_INIT(CORTO_BOOLEAN, CORTO_WIDTH_8, CORTO_COND_NOT, bool, cond_not);
    CORTO_BINARY_OP_INIT(CORTO_BOOLEAN, CORTO_WIDTH_8, CORTO_COND_EQ, bool, cond_eq);
    CORTO_BINARY_OP_INIT(CORTO_BOOLEAN, CORTO_WIDTH_8, CORTO_COND_NEQ, bool, cond_neq);
    CORTO_BINARY_OP_INIT(CORTO_BOOLEAN, CORTO_WIDTH_8, CORTO_COND_OR, bool, cond_or);
    CORTO_BINARY_OP_INIT(CORTO_BOOLEAN, CORTO_WIDTH_8, CORTO_COND_AND, bool, cond_and);

    CORTO_INTEGER_OPS_INIT(CORTO_BINARY, CORTO_WIDTH_8, octet);
    CORTO_INTEGER_OPS_INIT(CORTO_BINARY, CORTO_WIDTH_WORD, word);

    CORTO_INTEGER_OPS_INIT(CORTO_INTEGER, CORTO_WIDTH_8, int8);
    CORTO_INTEGER_OPS_INIT(CORTO_INTEGER, CORTO_WIDTH_16, int16);
    CORTO_INTEGER_OPS_INIT(CORTO_INTEGER, CORTO_WIDTH_32, int32);
    CORTO_INTEGER_OPS_INIT(CORTO_INTEGER, CORTO_WIDTH_64, int64);

    CORTO_INTEGER_OPS_INIT(CORTO_UINTEGER, CORTO_WIDTH_8, uint8);
    CORTO_INTEGER_OPS_INIT(CORTO_UINTEGER, CORTO_WIDTH_16, uint16);
    CORTO_INTEGER_OPS_INIT(CORTO_UINTEGER, CORTO_WIDTH_32, uint32);
    CORTO_INTEGER_OPS_INIT(CORTO_UINTEGER, CORTO_WIDTH_64, uint64);

    CORTO_INTEGER_OPS_INIT(CORTO_ENUM, CORTO_WIDTH_32, int32);
    CORTO_INTEGER_OPS_INIT(CORTO_BITMASK, CORTO_WIDTH_32, int32);

    CORTO_FLOAT_OPS_INIT(CORTO_FLOAT, CORTO_WIDTH_32, float32);
    CORTO_FLOAT_OPS_INIT(CORTO_FLOAT, CORTO_WIDTH_64, float64);

    CORTO_STRING_OPS_INIT();
}

corto_int16 corto_unaryOperator(corto_type type, corto_operatorKind operator, void* operand, void* result) {
    if (type->kind == CORTO_PRIMITIVE) {
        corto__unaryOperator impl = corto_unaryOps[corto_primitive(type)->convertId][operator];
        if (impl) {
            impl(operand, result);
        } else {
            corto_id id;
            corto_error("unary operator '%s' is not implemented for type '%s'", corto_nameof(corto_enum_constant(corto_operatorKind_o, operator)), corto_fullname(type, id));
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

corto_int16 corto_binaryOperator(corto_type type, corto_operatorKind operator, void *operand1, void *operand2, void *result) {
    if (type->kind == CORTO_PRIMITIVE) {
        corto__binaryOperator impl = corto_binaryOps[corto_primitive(type)->convertId][operator];
        if (impl) {
            impl(operand1, operand2, result);
        } else {
            corto_id id;
            corto_error("binary operator '%s' is not implemented for type '%s'", corto_nameof(corto_enum_constant(corto_operatorKind_o, operator)), corto_fullname(type, id));
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

#endif
