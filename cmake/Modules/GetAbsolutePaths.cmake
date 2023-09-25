function(get_absolute_paths ALL_RELATIVE_PATHS ALL_ABS_PATHS_VAR_NAME)

    foreach(RELATIVE_PATH ${ALL_RELATIVE_PATHS})
        file(REAL_PATH ${RELATIVE_PATH} ABSOLUTE_PATH)

        set(ALL_ABSOLUTE_PATHS
            ${ALL_ABSOLUTE_PATHS}
            ${ABSOLUTE_PATH})
        
    endforeach()

    set(${ALL_ABS_PATHS_VAR_NAME}
        ${ALL_ABSOLUTE_PATHS})

    return(PROPAGATE ${ALL_ABS_PATHS_VAR_NAME})

endfunction()
