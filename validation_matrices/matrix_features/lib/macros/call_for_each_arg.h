#ifndef CALL_FOR_EACH_ARG_H
#define CALL_FOR_EACH_ARG_H

#include "macrolib.h"
#include "recursion.h"


#define _FOREACH_EXPAND(fun, ...)       fun(__VA_ARGS__)

#define _FOREACH_0(fun, code)            code
#define _FOREACH_REC(fun, code, a, ...)  fun, code _FOREACH_EXPAND(fun, UNPACK(a));, ##__VA_ARGS__

#define FOREACH_UNGUARDED(fun, ...)     RECURSION_FORM_ITER(__VA_ARGS__)(_FOREACH_REC, _FOREACH_0, (fun, , __VA_ARGS__))
#define FOREACH(fun, ...)               do { FOREACH_UNGUARDED(fun, ##__VA_ARGS__); } while (0)


#endif /* CALL_FOR_EACH_ARG_H */

