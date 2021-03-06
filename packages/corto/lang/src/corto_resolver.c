
#define corto_lang_LIB
#include "corto.h"

/* Resolve anonymous object */
static corto_char* corto_resolveAnonymous(corto_object scope, corto_object o, corto_string str, corto_object* out) {
    corto_object result;
    corto_string_deser_t data;

    result = corto_declare(o);
    data.out = result;
    data.scope = scope;
    data.type = o;

    if (corto_type(o)->kind == CORTO_PRIMITIVE) {
        str += 1;
    }

    str = corto_string_deser(str, &data);
    *out = result;

    corto_define(result);

    return str;
}

/* Resolve address-identifier */
static corto_object corto_resolveAddress(corto_string str) {
    corto_word addr;

    addr = strtoul(str+1, NULL, 16);

    corto_claim((corto_object)addr);

    return (corto_object)addr;
}

/* Use private function to do a lookup with a string that is guaranteed lowercase */
#include "ctype.h"
corto_object corto_lookupLowercase(corto_object o, corto_string name);

/* Resolve fully scoped name */
corto_object corto_resolve(corto_object _scope, corto_string str) {
    corto_object scope, _scope_start, o, lookup;
    const char* ptr;
    char *bptr, *bptrLc;
    corto_id buffer;
    corto_id bufferLc;
    corto_char ch;
    corto_bool overload;
    corto_bool fullyQualified = FALSE;
    corto_bool cortoSearched = FALSE;
    int step = 2;

    if (!str) {
        str = "::";
    }

    if (!*str) {
        return NULL;
    }

    if ((str[0] == '.') && (str[1] == '\0')) {
        corto_claim(_scope);
        return _scope;
    }

    if (*str == '<') {
        return corto_resolveAddress(str);
    }

    _scope_start = corto_lang_o;
    scope = _scope_start;

    if (!_scope) {
        _scope = root_o;
    }

    if (*(corto_uint16*)str == CORTO_SCOPE_HEX) {
        str += 2;
        scope = root_o;
        fullyQualified = TRUE;
    } else if (*str == '/') {
        str += 1;
        scope = root_o;
        fullyQualified = TRUE;
    }

repeat:
    lookup = NULL;
    do {
        o = scope;

        ptr = str;
        ch = *ptr;
        if (!ch) {
            break;
        }
        while (ch) {
            overload = FALSE;
            if (scope == corto_o) {
                cortoSearched = TRUE;
            }

            bptr = buffer;
            bptrLc = bufferLc;
            while ((ch = *ptr) && (ch != ':') && (ch != '{') && (ch != '/')) {
                *bptr = ch;
                *bptrLc = tolower(ch);
                bptr++;
                bptrLc++;
                ptr++;
                if (ch == '(') {
                    overload = TRUE;
                    while ((ch = *ptr) && (ch != ')')) {
                        *bptrLc = tolower(ch);
                        *bptr = ch;
                        bptrLc++;
                        bptr++;
                        ptr++;
                    }
                }
            }
            *bptr = '\0';
            *bptrLc = '\0';

            if (corto_scopeSize(o)) {
                if (!overload) {
                    corto_object prev = o;
                    int i;
                    for (i = 0; i < 2; i++) {
                        o = corto_lookupLowercase(o, bufferLc);
                        if (lookup) {
                            corto_release(lookup); /* Free reference */
                        }
                        lookup = o;
                        if (o) {
                            if (corto_instanceof(corto_function_o, o)) {
                                if (corto_function(o)->overloaded == TRUE) {
                                    corto_release(o);
                                    corto_seterr("ambiguous reference to '%s'", str);
                                    goto error;
                                }
                            }
                        }

                        if (!o) {
                            if (!i && (prev != corto_lang_o) && corto_instanceof(corto_type(corto_package_o), prev)) {
                                corto_id load, id;
                                if (prev != root_o) {
                                    sprintf(load, "%s/%s", corto_fullname(prev, id), buffer);
                                } else {
                                    sprintf(load, "/%s", buffer);
                                }
                                corto_load(load, 0, NULL);
                                o = prev;
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    }

                    if (!o) {
                        break;
                    }
                } else {
                    /* If argumentlist is provided, look for closest match */
                    o = corto_lookupFunction(o, bufferLc, NULL);
                    if (lookup) {
                        corto_release(lookup);
                    }
                    lookup = o;
                    if (!o) {
                        break;
                    }
                }
            } else {
                o = NULL;
                if (lookup) {
                    corto_release(lookup);
                    lookup = NULL;
                }
                break;
            }

            /* Expect scope or serializable string */
            if (ch) {
                if (ch == '{') {
                    corto_object prev = o;
                    ptr = lookup = corto_resolveAnonymous(_scope, o, (char*)ptr, &o);
                    if (!ptr) {
                        o = NULL;
                    }
                    corto_release(prev);
                    break;
                } else if (*(corto_uint16*)ptr == CORTO_SCOPE_HEX) {
                    ptr += 2;
                } else if (ch == '/') {
                    ptr += 1;
                } else {
                    corto_seterr("invalid ':' in expression '%s'", str);
                    o = NULL;
                    break;
                }
            }
        }
        if (o) break;
    } while((step == 1) && (scope = corto_parentof(scope)));

    /* Do lookup in actual scope first, then in corto */
    if (!o && step && !fullyQualified) {
        switch(--step) {
        case 1:
            if ((_scope == corto_o) || (_scope == corto_lang_o)) {
                _scope_start = scope = root_o;
            } else {
                _scope_start = scope = _scope;
            }
            goto repeat;
            break;
        case 0:
            if (!cortoSearched) {
                _scope_start = scope = corto_o;
                goto repeat;
            }
            break;    
        }

         /* Do goto instead of a recursive call. Besides saving (a little bit of) performance,
            this also preserves the original searchscope, which is needed in anonymous type lookups, which
            uses the stringserializer. In a serialized string references to other objects may be relatively
            scoped. For example: the string sequence{F} results in an anonymous sequence object with
            elementType 'F', which is looked up in scope '_scope_start'. */
    }

    /* If the current object is not obtained by a lookup, it is not yet keeped. */
    if (!lookup && o) {
        corto_claim(o);
    }

    return o;
error:
    return NULL;
}
