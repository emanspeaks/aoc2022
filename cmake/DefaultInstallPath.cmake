## define the project default install path, if necessary
##
## CMake's native defaults are:
## Windows: C:\\Program Files
## Else: /usr/local
##
## The lines below default to using this file's directory as install root
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR} CACHE PATH "Choose the installation directory" FORCE)
  ## In case this build includes multiple projects,
  ## only set for the first (in theory, topmost) project
  unset(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif()
