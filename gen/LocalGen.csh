#!/bin/csh 
#
source /phenix/u/vassalli/.cshrc
set FUNFRIENDS="/direct/phenix+u/vassalli/sphenix/FunFriends"
set FUN="Fun4All_G4_sPHENIX.C" 
#set FUN="defaultFun.C"
set path = ($FUNFRIENDS $path)

root -b -q $FUN 
#root -b -q myAnalysis.C\(\"$DST_FILE\",\"out${1}Tree.root\"\)
exit 0
