#/bin/bash
QT_HOME="/usr/local/qt/Qt5.14.1/5.14.1/gcc_64"
cmd="$QT_HOME/bin/qmake fm.pro -o Makefile"
echo $cmd
eval $cmd
