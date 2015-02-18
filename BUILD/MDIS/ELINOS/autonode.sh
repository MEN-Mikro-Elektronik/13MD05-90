#*****************************************************************************
#  
#         Author: kp
#          $Date: 2004/06/09 11:12:51 $
#      $Revision: 1.1 $
#  
#    Description: Make device node for /dev/mdis
#
# The only purpose of this script is to supply the /dev/mdis
# device node to the ELinOS project (if CONFIG_DEVFS_FS is not defined)
#
# At the end, it calls the regular autonode.sh from ELinOS
#                      
#       Switches: -
#                      
#-----------------------------------------------------------------------------
#   (c) Copyright 2002 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

KCONFIG=$1

# check if DEVFS is defined
grep -q '^CONFIG_DEVFS_FS' $KCONFIG
nothavedevfs=$?

if [ $nothavedevfs ]
then
	# locate MDIS_MAJOR_NUMBER in MDIS makefile
	major=`awk '/^MDIS_MAJOR_NUMBER/ {print $3}' src/mdis/Makefile`
	if [ "${major}z" != "z" ]
	then 
		echo node "/dev/mdis" c $major 0 0666 0 0 ""
	fi	
fi

$ELINOS_PREFIX/bin/autonode.sh $KCONFIG
