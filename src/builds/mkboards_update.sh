#!/bin/sh

###############Debug Flags#########################
scriptDebugCommands=0
scriptVerbose=1
########End Debug Flags###########

###########Validate Current User####################
builderUser=olt
password=olttera
currentUser=$(whoami)

# Validate arguments
if [ $# -lt 3 ]; then
  echo "Arguments required: $0 <oltVersion> <patchVersion> <svnRelease>"
  exit -1;
fi

oltVersion=$1
patchVersion=$2
svnRelease=$3

############Paths Assignment & Validation#####################
builderScriptPath=.
srcPath=..
buildPath=/home/olt/fastpath_builds
mkBoardsPath=/home/olt/mkboards_$oltVersion

############Fastpath MKBoards Update####################
if  [ $scriptVerbose -eq 1 ]; then
	echo "Going to extract fastpath-olt.image-v$oltVersion.$patchVersion-r$svnRelease.tgz to $mkBoardsPath...press Ctrl+C to abort"
	sleep 1
fi
cd $buildPath
if [ ! -f ./fastpath/fastpath-olt.image-v$oltVersion.$patchVersion-r$svnRelease.tgz ]; then
    echo "File not found: ./fastpath/fastpath-olt.image-v$oltVersion.$patchVersion-r$svnRelease.tgz"
	exit $?
fi


if  [ $scriptDebugCommands -eq 1 ]; then
	echo "sudo sh image_update.sh  ./fastpath/fastpath-olt.image-v$oltVersion.$patchVersion-r$svnRelease.tgz $mkBoardsPath"
fi
echo $password | sudo -S sh image_update.sh fastpath/fastpath-olt.image-v$oltVersion.$patchVersion-r$svnRelease.tgz $mkBoardsPath

if  [ $? -ne 0 ]; then
	echo -e "
      ######################################################################################################################\n
      # Failed to update MK Boards | MKBoardsVersion=$oltVersion | SVNVersion=$svnRelease ################################################\n
      ######################################################################################################################"

exit $?
fi

if  [ $scriptVerbose -eq 1 ]; then
	echo "Going to update fastpath image on mkboards of ag16ga...press Ctrl+C to abort"
	sleep 1
fi
if  [ $scriptDebugCommands -eq 1 ]; then
	echo "sudo sh fastpath_ag16ga.sh"
fi
cd ../ag16ga
echo $password | sudo -S sh fastpath_ag16ga.sh 

if  [ $? -ne 0 ]; then
	echo -e "
      ######################################################################################################################\n
      # Failed to update MK Boards of ag16ga | MKBoardsVersion=$oltVersion | SVNVersion=$svnRelease ################################################\n
      ######################################################################################################################"

exit $?
fi
############Fastpath MKBoards Update####################
