set(KAZE_MODULE_NAME PLATFORM)

# Validate backend. The name must correspond to the name of a folder in:
# "kaze/platform/backend/", which contains a "module.cmake" file.
# See cmake function `kaze_target_module`, located in `cmake/modules/kaze/utils.cmake` for more details.
if (EXISTS "${KAZE_BACKEND_ROOT}/${KAZE_BACKEND}/module.cmake")
    message("Kaze Backend: ${KAZE_BACKEND}")
else()
    message(FATAL_ERROR "Backend ${KAZE_BACKEND} is not supported by Kaze")
endif()

set(PLATFORM_SOURCES_PRIVATE
    backend/backend.cpp
    backend/backend.h

    BackendInitGuard.cpp
    BackendInitGuard.h
    defines.h
)

kaze_add_submodule(backend/${KAZE_BACKEND})
kaze_add_submodule(filesys)

if (KAZE_PLATFORM_ANDROID)
    kaze_add_submodule(android)
endif()
