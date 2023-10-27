PHP_ARG_ENABLE([encoding],
  [whether to enable encoding support],
  [AS_HELP_STRING([--enable-encoding],
    [Enable encoding support])],
  [no])

if test "$PHP_ENCODING" != "no"; then
  PHP_REQUIRE_CXX()

  dnl the 6th parameter here is required for C++ shared extensions
  PHP_NEW_EXTENSION(encoding, encoding.cpp, $ext_shared,,-std=c++14 -Wall -Werror, yes)
fi
