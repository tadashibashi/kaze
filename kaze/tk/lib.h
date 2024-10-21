#pragma once
#ifndef kaze_tk_lib_h_
#define kaze_tk_lib_h_

#include <kaze/core/lib.h>

#if KAZE_NO_NAMESPACE
#define   KAZE_TK_NAMESPACE tk
#else
#define   KAZE_TK_NAMESPACE KAZE_NAMESPACE::tk
#endif

#define KAZE_TK_NAMESPACE_BEGIN namespace KAZE_TK_NAMESPACE {
#define KAZE_TK_NAMESPACE_END }
#define USING_KAZE_TK_NAMESPACE using namespace KAZE_TK_NAMESPACE

#endif // kaze_tk_lib_h_
