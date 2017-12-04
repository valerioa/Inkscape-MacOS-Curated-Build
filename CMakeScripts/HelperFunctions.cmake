# pkg_check_variable() - a function to retrieve pkg-config variables in CMake
#
# source: http://bloerg.net/2015/03/06/pkg-config-variables-in-cmake.html
function(pkg_check_variable _pkg _name)
    find_package(PkgConfig REQUIRED)
    string(TOUPPER ${_pkg} _pkg_upper)
    string(TOUPPER ${_name} _name_upper)
    string(REPLACE "-" "_" _pkg_upper ${_pkg_upper})
    string(REPLACE "-" "_" _name_upper ${_name_upper})
    set(_output_name "${_pkg_upper}_${_name_upper}")

    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=${_name} ${_pkg}
        OUTPUT_VARIABLE _pkg_result
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    set("${_output_name}" "${_pkg_result}" CACHE STRING "pkg-config variable ${_name} of ${_pkg}")
endfunction()

# Join a cmake list of strings with a given glue character/string
# E.g. join(MY_RESULT, ",", "1; 2; 3;") returns "1, 2, 3"
function(join OUTPUT GLUE)
    set(_TMP_RESULT "")
    set(_GLUE "") # effective glue is empty at the beginning
    foreach(arg ${ARGN})
	# Skip empty lines
	if(NOT arg STREQUAL "\n")
	    set(_TMP_RESULT "${_TMP_RESULT}${_GLUE}${arg}")
	    set(_GLUE "${GLUE}")
	endif()
    endforeach()
    set(${OUTPUT} "${_TMP_RESULT}" PARENT_SCOPE)
endfunction()



# Checks if the last call to execute_process() was sucessful and throws an error otherwise.
# ${result} and ${stderr} should hold the value of RESULT_VARIABLE and ERROR_VARIABLE respectively
# ${command} can be empty or the command that was executed during the last call of execute_process()
function(check_error result stderr command)
    string(STRIP "${result}" result)
    string(STRIP "${stderr}" stderr)
    string(STRIP "${command}" command)

    if ("${command}" STREQUAL "")
        set(command "Process")
    else()
        set(command "'${command}'")
    endif()

    if("${result}" STREQUAL 0)
        if(NOT "${stderr}" STREQUAL "")
            MESSAGE(WARNING "${command} returned sucessfully but the following was output to stderr: ${stderr}")
        endif()
    else()
        if("${stderr}" STREQUAL "")
            MESSAGE(FATAL_ERROR "${command} failed with error code: ${result}")
        else()
            MESSAGE(FATAL_ERROR "${command} failed with error code: ${result} (stderr: ${stderr})")
        endif()
    endif()
endfunction(check_error)



# Get the list of files installed by pacman for package ${package_name} and return it as ${file_list}.
# Paths are relative to the root directory of the MinGW installations (the directory returned by function "get_mingw_root()")
function(list_files_pacman package_name file_list)
    set(MINGW_PACKAGE_PREFIX $ENV{MINGW_PACKAGE_PREFIX}) # e.g. "mingw-w64-x86_64"
    get_filename_component(MINGW_PREFIX $ENV{MINGW_PREFIX} NAME)  # e.g. "mingw64"

    # use pacman to list all files/folders installed by the package
    execute_process(
        COMMAND pacman -Ql ${MINGW_PACKAGE_PREFIX}-${package_name}
        OUTPUT_FILE list_files_pacman_temp.txt
        RESULT_VARIABLE res
        ERROR_VARIABLE err
    )
    check_error("${res}" "${err}" "pacman -Ql ${MINGW_PACKAGE_PREFIX}-${package_name}")

    # clean up output
    execute_process(
        COMMAND grep -v '/$' # get rid of folders
        COMMAND sed -e 's/^${MINGW_PACKAGE_PREFIX}-${package_name} //' # remove package name
        COMMAND sed -e 's/^\\/${MINGW_PREFIX}\\///' # remove root path
        COMMAND tr '\n' '\;' # finally replace newlines with semicolon
        INPUT_FILE list_files_pacman_temp.txt
        OUTPUT_VARIABLE out
        RESULT_VARIABLE res
        ERROR_VARIABLE err
    )
    check_error("${res}" "${err}" "Parsing result of 'pacman -Ql ${MINGW_PACKAGE_PREFIX}-${package_name}'")

    SET(${file_list} ${out} PARENT_SCOPE)
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/list_files_pacman_temp.txt)
endfunction(list_files_pacman)



# Get the list of files installed by pip for package ${package_name} and return it as ${file_list}.
# Paths are relative to the python distributions "site-packages" folder, i.e. "${root}/lib/python2.7/site-packages"
function(list_files_pip package_name file_list)
    # use pip to show package information including full list of files installed by the package
    execute_process(
        COMMAND pip show -f ${package_name}
        OUTPUT_FILE list_files_pip_temp.txt
        RESULT_VARIABLE res
        ERROR_VARIABLE err
    )
    check_error("${res}" "${err}" "pip show -f ${package_name}")

    # clean up output
    execute_process(
        COMMAND sed -e '1,/Files:/d' # strip everything but the files list
        COMMAND tr -d ' ' # strip spaces
        COMMAND tr '\n' '\;' # finally replace newlines with semicolon
        INPUT_FILE list_files_pip_temp.txt
        OUTPUT_VARIABLE out
        RESULT_VARIABLE res
        ERROR_VARIABLE err
    )
    check_error("${res}" "${err}" "Parsing result of 'pip show -f ${package_name}'")

    SET(${file_list} ${out} PARENT_SCOPE)
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/list_files_pip_temp.txt)
endfunction(list_files_pip)



# Install a list of files maintaining directory structure
#
# Options:
#   FILES       - the list of files (absolute or relative paths)
#   ROOT        - the root to search the files in (if file paths are relative)
#   DESTINATION - the destination where to install files to
#   INCLUDE     - a (list of) regular expression(s) specifying which files to include
#                 (omit or leave empty to inlcude all files)
#   EXCLUDE     - a (list of) regular expression(s) specifying which files to exclude
#                 (takes precedence over include rules)
function(install_list)
    # parse arguments
    set(oneValueArgs ROOT DESTINATION)
    set(multiValueArgs FILES INCLUDE EXCLUDE)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    #MESSAGE("ARG_FILES: ${ARG_FILES}" )
    #MESSAGE("ARG_ROOT: ${ARG_ROOT}" )
    #MESSAGE("ARG_DESTINATION: ${ARG_DESTINATION}" )
    #MESSAGE("ARG_INCLUDE: ${ARG_INCLUDE}" )
    #MESSAGE("ARG_EXCLUDE: ${ARG_EXCLUDE}" )
    #MESSAGE("ARG_UNPARSED_ARGUMENTS: ${ARG_UNPARSED_ARGUMENTS}" )

    # install the files
    foreach(file ${ARG_FILES})
        #MESSAGE("file: " ${file})

        # check includes and excludes (excludes take precedence)
        set(include_file 0)
        if("${ARG_INCLUDE}" STREQUAL "") # start with the assumption to include all files by default
            set(include_file 1)
        endif()
        foreach(include ${ARG_INCLUDE})
            if("${file}" MATCHES "${include}")
                set(include_file 1)
            endif()
        endforeach()
        foreach(exclude ${ARG_EXCLUDE})
            if("${file}" MATCHES "${exclude}")
                set(include_file 0)
            endif()
        endforeach()

        # install if file should be included
        if(${include_file})
            get_filename_component(directory ${file} DIRECTORY)
            install(FILES "${ARG_ROOT}/${file}" DESTINATION "${ARG_DESTINATION}${directory}")
        endif()
    endforeach()
endfunction(install_list)
