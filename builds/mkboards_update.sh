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
	echo "Going to update fastpath image on mkboards of olt1t0...press Ctrl+C to abort"
	sleep 1
fi
cd $mkBoardsPath/CXOLT1T0
if  [ $scriptDebugCommands -eq 1 ]; then
	echo "OLT1T0 -> sudo sh fastpath_olt1t0.sh"
fi
echo $password | sudo -S sh fastpath_olt1t0.sh

if  [ $scriptDebugCommands -eq 1 ]; then
        echo "OLT1T0-AC -> sudo sh fastpath_olt1t0.sh"
fi
cd $mkBoardsPath/CXOLT1T0-AC 
echo $password | sudo -S sh fastpath_olt1t0.sh

if  [ $? -ne 0 ]; then
	echo -e "
      ######################################################################################################################\n
      # Failed to update MK Boards of OLT1T0 | MKBoardsVersion=$oltVersion | SVNVersion=$svnRelease ################################################\n
      ######################################################################################################################"

exit $?
fi

if  [ $scriptVerbose -eq 1 ]; then
        echo "Going to update fastpath image on mkboards of olt1t0f...press Ctrl+C to abort"
        sleep 1
fi
cd $mkBoardsPath/CXOLT1T0-F
if  [ $scriptDebugCommands -eq 1 ]; then
        echo "OLT1T0F -> sudo sh fastpath_olt1t0.sh"
fi
echo $password | sudo -S sh fastpath_olt1t0.sh

if  [ $? -ne 0 ]; then
    echo -e "
    ######################################################################################################################\n
    # Failed to update MK Boards of OLT1T0F | MKBoardsVersion=$oltVersion | SVNVersion=$svnRelease ###############################################\n
    ######################################################################################################################"

    exit $?
fi

if  [ $scriptVerbose -eq 1 ]; then
	echo "Going to update fastpath image on mkboards of olt1t1...press Ctrl+C to abort"
	sleep 1
fi
if  [ $scriptDebugCommands -eq 1 ]; then
	echo "sudo sh fastpath_cxo160g.sh"
fi
cd ../CXO160G 
echo $password | sudo -S sh fastpath_cxo160g.sh 

if  [ $? -ne 0 ]; then
	echo -e "
      ######################################################################################################################\n
      # Failed to update MK Boards of OLT1T1 | MKBoardsVersion=$oltVersion | SVNVersion=$svnRelease ################################################\n
      ######################################################################################################################"

exit $?
fi
############Fastpath MKBoards Update####################
