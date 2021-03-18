#/bin/bash
cmd="rm -rfv *.o"
echo $cmd
eval $cmd
cmd="rm -rfv moc_*"
echo $cmd
eval $cmd
cmd="rm -fv fm"
echo $cmd
eval $cmd
cmd="rm -fv Makefile"
echo $cmd
eval $cmd
