# Behaves like `option` but for string values
# Arguments:
#     NAME          - name of the option
#     DESCRIPTION   - brief description of the option
#     DEFAULT_VALUE - default value for the option
function(string_option NAME DOCUMENTATION DEFAULT_VALUE)
    if (NOT EXISTS ${NAME})
        set(VALUE ${DEFAULT_VALUE})
    else()
        set(VALUE ${NAME})
    endif()

    set(${NAME} ${VALUE} STRING ${DOCUMENTATION} FORCE)
endfunction()
