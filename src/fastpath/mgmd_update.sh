##############################################
#                                            #
# Script to update MGMD to a given revision  #
#                                            #
#  Daniel Figueira, 2014                     #
#  daniel-f-figueira@telecom.pt              #
#                                            #
##############################################
#!/bin/sh
 
# README!
#
# Use this script to edit svn:externals property for MGMD.
#
# Input variables:
#		- $1 -> Desired folder inside the MGMD repository
#		- $2 -> Desired MGMD SVN repository revision
#
# Examples:
#   - sh mgmd_update trunk 467          -> Update local mgmd folder with the contents of the trunk folder of the MGMD repository in revision 467
#   - sh mgmd_update branches/1.0 300   -> Update local mgmd folder with the contents of the branches/1.0 folder of the MGMD repository in revision 300

MGMD_PATH=$1
MGMD_REV=$2
EXTERNALS_PROP_FILE=svn_externals.txt

#Create the temporary svn property file 
echo "-r $MGMD_REV http://svn.ptin.corppt.com/repo/netband-mgmd/$MGMD_PATH mgmd" > $EXTERNALS_PROP_FILE

#Update the existing svn property with the created file
svn propset svn:externals src/application/switching/ -F svn_externals.txt

#Delete the temporary file
rm -f $EXTERNALS_PROP_FILE