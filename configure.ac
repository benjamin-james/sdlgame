AC_PREREQ([2.61])
AC_INIT([sdlgame], [0.1], [benjamin-james@users.noreply.github.com])
AC_CONFIG_SRCDIR([main.c])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

AC_CHECK_LIB([SDL2], [SDL_Init])
AC_CHECK_LIB([SDL2_gfx], [filledCircleColor])
AC_CHECK_LIB([m], [sqrt])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([sqrt])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
