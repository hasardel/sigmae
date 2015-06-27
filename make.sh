#!/bin/sh


#
# Copyright (C) 2015 Florent Pouthier
# Copyright (C) 2015 Emmanuel Pouthier
#
# This file is part of SIGMAE.
#
# Aye-Aye is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Aye-Aye is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

cc=gcc
opts="-O2 -Wall"
builddir="build"
includes="-I include -I include/extern"
libs="-lm -ldl -lpthread -l:libasound.so.2 -l:libsndfile.so.1"
srcs="dsaa mem error sgladspa elem mod core plugin audev-oss audev-alsa audev boundvar misc utils maths main"
objs=""
for src in $srcs
do
  objs="$objs$src.o "
done

compile()
{
  echo compiling $1.o...
  $cc $includes $opts -c $1.c -o $builddir/$1.o
}

link()
{
  echo linking...
  cd $builddir
  $cc $opts $libs $objs -o main
}

if [ "$1" = "" ]
then
  set $1 all
fi

case $1 in
  all)
    for src in $srcs
    do
      compile $src
    done
    link;;
  clean)
    cd $builddir
    rm $objs;;
  comp)
    for src in $srcs
    do
      compile $src
    done;;
  link)
    link;;
  *)
    compile $1 && link;;
esac

