#############################################
#                                           #
#           MGMD upgrade script             #
#                                           #
#	Daniel Figueira, 2014                    #
#	daniel-f-figueira@ext.ptinovacao.pt      #
#                                           #
#############################################
#!/bin/sh

# README!
#
# Use this script to upgrade the MGMD module.
#
# Input variables:
#		- $1 -> Name of the MGMD source package
#		- $2 -> Target with which the switching component is to be compiled ('switching'; 'clean-switching') 
#
# This script copies the MGMD source package to the src/application/switching folder. Then, it extracts and replaces the content in the mgmd folder.
#
# Note: The MGMD source package MUST be in Fastpath's root directory

MGMD_SOURCE_PACKAGE=$1
MGMD_PACKAGE_FOLDER=`echo $MGMD_SOURCE_PACKAGE | sed s/".tar.gz"//`
SWITCHING_FOLDER=src/application/switching

echo -n "Starting mgmd upgrade..."
cp $MGMD_SOURCE_PACKAGE $SWITCHING_FOLDER
cd $SWITCHING_FOLDER
tar -xzf $MGMD_SOURCE_PACKAGE
rm -rf mgmd
mv $MGMD_PACKAGE_FOLDER mgmd/
rm -rf $MGMD_PACKAGE_FOLDER $MGMD_SOURCE_PACKAGE
echo "Finished!"




