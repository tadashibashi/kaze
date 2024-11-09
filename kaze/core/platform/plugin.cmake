set(KAZE_PLUGIN_NAME PLATFORM)

# Validate backend. The name must correspond to the name of a folder in:
# "kaze/platform/backend/", which contains a "plugin.cmake" file.
# See cmake function `kaze_target_plugin`, located in `cmake/modules/kaze/utils.cmake` for more details.
if (EXISTS "${KAZE_BACKEND_ROOT}/${KAZE_BACKEND}/plugin.cmake")
    message("Kaze Backend: ${KAZE_BACKEND}")
else()
    message(FATAL_ERROR "Backend ${KAZE_BACKEND} is not supported by Kaze")
endif()

kaze_add_subplugin(backend/${KAZE_BACKEND})
kaze_add_subplugin(filesys)

if (KAZE_PLATFORM_ANDROID)
    kaze_add_subplugin(android)
endif()
