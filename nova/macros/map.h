#pragma once
#ifndef nova_macros_map_h_
#define nova_macros_map_h_

#define NOVA_EVAL0(...) __VA_ARGS__
#define NOVA_EVAL1(...) NOVA_EVAL0(NOVA_EVAL0(NOVA_EVAL0(__VA_ARGS__)))
#define NOVA_EVAL2(...) NOVA_EVAL1(NOVA_EVAL1(NOVA_EVAL1(__VA_ARGS__)))
#define NOVA_EVAL3(...) NOVA_EVAL2(NOVA_EVAL2(NOVA_EVAL2(__VA_ARGS__)))
#define NOVA_EVAL4(...) NOVA_EVAL3(NOVA_EVAL3(NOVA_EVAL3(__VA_ARGS__)))
#define NOVA_EVAL(...)  NOVA_EVAL4(NOVA_EVAL4(NOVA_EVAL4(__VA_ARGS__)))

#define NOVA_MAP_END(...)
#define NOVA_MAP_OUT
#define NOVA_MAP_COMMA ,

#define NOVA_MAP_GET_END2() 0, NOVA_MAP_END
#define NOVA_MAP_GET_END1(...) NOVA_MAP_GET_END2
#define NOVA_MAP_GET_END(...) NOVA_MAP_GET_END1
#define NOVA_MAP_NEXT0(test, next, ...) next NOVA_MAP_OUT
#define NOVA_MAP_NEXT1(test, next) NOVA_MAP_NEXT0(test, next, 0)
#define NOVA_MAP_NEXT(test, next)  NOVA_MAP_NEXT1(NOVA_MAP_GET_END test, next)

#define NOVA_MAP0(f, x, peek, ...) f(x) NOVA_MAP_NEXT(peek, NOVA_MAP1)(f, peek, __VA_ARGS__)
#define NOVA_MAP1(f, x, peek, ...) f(x) NOVA_MAP_NEXT(peek, NOVA_MAP0)(f, peek, __VA_ARGS__)

#define NOVA_MAP_LIST_NEXT1(test, next) NOVA_MAP_NEXT0(test, NOVA_MAP_COMMA next, 0)
#define NOVA_MAP_LIST_NEXT(test, next)  NOVA_MAP_LIST_NEXT1(NOVA_MAP_GET_END test, next)

#define NOVA_MAP_LIST0(f, x, peek, ...) f(x) NOVA_MAP_LIST_NEXT(peek, NOVA_MAP_LIST1)(f, peek, __VA_ARGS__)
#define NOVA_MAP_LIST1(f, x, peek, ...) f(x) NOVA_MAP_LIST_NEXT(peek, NOVA_MAP_LIST0)(f, peek, __VA_ARGS__)

/**
 * Applies the function macro `f` to each of the remaining parameters.
 */
#define NOVA_MAP(f, ...) NOVA_EVAL(NOVA_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/**
 * Applies the function macro `f` to each of the remaining parameters and
 * inserts commas between the results.
 */
#define NOVA_MAP_LIST(f, ...) NOVA_EVAL(NOVA_MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#endif // nova_macros_map_h_
