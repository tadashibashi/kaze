set(KAZE_MODULE_NAME KAZE_IO)

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

    Rstream/Rstreamable.h
    Rstream/RstreamableFile.cpp
    Rstream/RstreamableFile.h
    Rstream/RstreamableMemory.cpp
    Rstream/RstreamableMemory.h
)

if (KAZE_PLATFORM_ANDROID)
    list(APPEND KAZE_IO_SOURCES_PRIVATE
        Rstream/RstreamableAAsset.cpp
        Rstream/RstreamableAAsset.h
    )
endif()
