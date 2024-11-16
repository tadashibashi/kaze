set(KAZE_MODULE KAZE_IO)

set(KAZE_IO_SOURCES_PRIVATE
    BufferIO.cpp
    BufferIO.h
    BufferView.cpp
    BufferView.h
    BufferWriter.cpp
    BufferWriter.h
    FileBuffer.cpp
    FileBuffer.h
    io.cpp
    io.h
    StructLayout.h

    stream/Rstream.cpp
    stream/Rstream.h
    stream/Rstreamable.h
    stream/RstreamableFile.cpp
    stream/RstreamableFile.h
    stream/RstreamableMemory.cpp
    stream/RstreamableMemory.h
)

if (KAZE_PLATFORM_ANDROID)
    list(APPEND KAZE_IO_SOURCES_PRIVATE
        stream/RstreamableAAsset.cpp
        stream/RstreamableAAsset.h
    )
endif()
