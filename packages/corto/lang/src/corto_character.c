/* $CORTO_GENERATED
 *
 * corto_character.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include "corto.h"

corto_int16 _corto_character_init(corto_character this) {
/* $begin(::corto::lang::character::init) */
    corto_primitive(this)->kind = CORTO_CHARACTER;
    return corto_primitive_init((corto_primitive)this);
/* $end */
}
