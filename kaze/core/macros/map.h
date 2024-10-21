#pragma once
#ifndef kaze_core_macros_map_h_
#define kaze_core_macros_map_h_

#define KAZE_EVAL0(...) __VA_ARGS__
#define KAZE_EVAL1(...) KAZE_EVAL0(KAZE_EVAL0(KAZE_EVAL0(__VA_ARGS__)))
#define KAZE_EVAL2(...) KAZE_EVAL1(KAZE_EVAL1(KAZE_EVAL1(__VA_ARGS__)))
#define KAZE_EVAL3(...) KAZE_EVAL2(KAZE_EVAL2(KAZE_EVAL2(__VA_ARGS__)))
#define KAZE_EVAL4(...) KAZE_EVAL3(KAZE_EVAL3(KAZE_EVAL3(__VA_ARGS__)))
#define KAZE_EVAL(...)  KAZE_EVAL4(KAZE_EVAL4(KAZE_EVAL4(__VA_ARGS__)))

#define KAZE_MAP_END(...)
#define KAZE_MAP_OUT
#define KAZE_MAP_COMMA ,

#define KAZE_MAP_GET_END2() 0, KAZE_MAP_END
#define KAZE_MAP_GET_END1(...) KAZE_MAP_GET_END2
#define KAZE_MAP_GET_END(...) KAZE_MAP_GET_END1
#define KAZE_MAP_NEXT0(test, next, ...) next KAZE_MAP_OUT
#define KAZE_MAP_NEXT1(test, next) KAZE_MAP_NEXT0(test, next, 0)
#define KAZE_MAP_NEXT(test, next)  KAZE_MAP_NEXT1(KAZE_MAP_GET_END test, next)

#define KAZE_MAP0(f, x, peek, ...) f(x) KAZE_MAP_NEXT(peek, KAZE_MAP1)(f, peek, __VA_ARGS__)
#define KAZE_MAP1(f, x, peek, ...) f(x) KAZE_MAP_NEXT(peek, KAZE_MAP0)(f, peek, __VA_ARGS__)

#define KAZE_MAP_LIST_NEXT1(test, next) KAZE_MAP_NEXT0(test, KAZE_MAP_COMMA next, 0)
#define KAZE_MAP_LIST_NEXT(test, next)  KAZE_MAP_LIST_NEXT1(KAZE_MAP_GET_END test, next)

#define KAZE_MAP_LIST0(f, x, peek, ...) f(x) KAZE_MAP_LIST_NEXT(peek, KAZE_MAP_LIST1)(f, peek, __VA_ARGS__)
#define KAZE_MAP_LIST1(f, x, peek, ...) f(x) KAZE_MAP_LIST_NEXT(peek, KAZE_MAP_LIST0)(f, peek, __VA_ARGS__)

/**
 * Applies the function macro `f` to each of the remaining parameters.
 */
#define KAZE_MAP(f, ...) KAZE_EVAL(KAZE_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/**
 * Applies the function macro `f` to each of the remaining parameters and
 * inserts commas between the results.
 */
#define KAZE_MAP_LIST(f, ...) KAZE_EVAL(KAZE_MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#endif // kaze_core_macros_map_h_
