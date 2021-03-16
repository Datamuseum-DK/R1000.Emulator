#!/bin/sh

if $1 --version | grep -q clang ; then
	echo " -std=gnu99 -Wno-format-zero-length -nobuiltininc"
	echo " -fstack-protector-strong -Wsystem-headers -Werror -Wall"
	echo " -Wno-format-y2k -W -Wno-unused-parameter -Wstrict-prototypes"
	echo " -Wmissing-prototypes -Wpointer-arith -Wreturn-type -Wcast-qual"
	echo " -Wwrite-strings -Wswitch -Wshadow -Wunused-parameter"
	echo " -Wcast-align"
	echo " -Wchar-subscripts -Winline -Wnested-externs -Wredundant-decls"
	echo " -Wold-style-definition -Wno-pointer-sign"
	echo " -Wmissing-variable-declarations -Wthread-safety -Wno-empty-body"
	echo " -Wno-string-plus-int"
	echo " -Wno-missing-field-initializers"
	echo " -Qunused-arguments"
fi

echo " -Wno-format-zero-length"
echo " -Wno-unused-const-variable"

exit 0
