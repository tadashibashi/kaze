#pragma once
#include <kaze/kaze.h>

#define KTK_NS KAZE_NS::tk
#define KTK_NS_INTERNAL KAZE_NS_INTERNAL::tk
#define KTK_NS_BEGIN namespace KTK_NS_INTERNAL {
#define KTK_NS_END }

#define USING_KTK_NS using namespace KTK_NS; USING_KAZE_NS

#define KTK_PUBLIC_NS_BEGIN namespace KTK_NS {
#define KTK_PUBLIC_NS_END }
