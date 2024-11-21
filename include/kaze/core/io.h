#pragma once
#include "lib.h" // IWYU pragma: export
#include "endian.h" // IWYU pragma: export
#include "memory.h" // IWYU pragma: export
#include <kaze/internal/core/io/io.h>
#include <kaze/internal/core/io/BufferView.h>
#include <kaze/internal/core/io/BufferWriter.h>
#include <kaze/internal/core/io/FileBuffer.h>
#include <kaze/internal/core/io/StructIO.h>
#include <kaze/internal/core/io/StructLayout.h>

KAZE_PUBLIC_NS_BEGIN

/// Contains low-level file i/o functions
namespace file = KAZE_NS_INTERNAL::file;

using BufferView = KAZE_NS_INTERNAL::BufferView;
using BufferWriter = KAZE_NS_INTERNAL::BufferWriter;
using FileBuffer = KAZE_NS_INTERNAL::FileBuffer;
using StructIO = KAZE_NS_INTERNAL::StructIO;

using StructLayoutInit = KAZE_NS_INTERNAL::StructLayoutInit;
using StructLayout = KAZE_NS_INTERNAL::StructLayout;

using SeekBase = KAZE_NS_INTERNAL::SeekBase;

KAZE_PUBLIC_NS_END
