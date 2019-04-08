#!/bin/bash
#*****************************************************************************
#  
#         Author: Andreas Geissler
#          $Date: 2018-06-20 
#       Revision: 0.1
#  
#    Description: Installation script for MDIS5LINUX system package
#      
#-----------------------------------------------------------------------------
# (c) Copyright 2018-2019 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
#*****************************************************************************

# warn when using uninitialised variables
set -u

this=$0
CWD="$( cd "$(dirname "$0")" ; pwd -P )"

# installation dir
OPT=/opt
MENLINUX=menlinux
MENHISTORY=HISTORY

# default path
MENLINUX_ROOT=${OPT}/${MENLINUX}

# system package git repository name
MDIS_PACKAGE=MDISforLinux

# default install parameter
force_install="0"

# read parameters
while test $# -gt 0 ; do
   case "$1" in 
        -h|--help)
                echo "options:"
                echo "-h, --help        show brief help"
                echo "-p, --path=PATH   specify a installation path"
                echo "-f, --forceyes    install without asking, do not scan the hardware"
                exit 0
                ;;
        -p)
                shift
                if test $# -gt 0; then
                        export MENLINUX_ROOT=$1
                else
                        echo "no path specified"
                        exit 1
                fi
                shift
                ;;
        --path*)
                export MENLINUX_ROOT=$(echo $1 | sed -e 's/^[^=]*=//g')
                shift
                ;;
        -f|--forceyes)
                shift
                export force_install="1"
                echo "Install without asking:"
                echo "- install sources into /opt/menlinux"
                echo "- do not scan the hardware"
                ;;
        *)
                break
                ;;
        esac
done


show_next_steps() {
        echo "The next steps could be:"
        echo " - generate a MDIS project by opening MDIS Wizard and run automatic system scan or"
        echo " - generate a MDIS project by opening MDIS Wizard and manually create a system configuration"
}

show_manual_steps() {
        echo "Please create the MDIS configuration for your target system (see user manual) by:"
        echo " - Running the MDIS Wizard at the host system."
        echo " - Initiate the system scan manually at the target system"
}

show_status_message() {
        echo "________________________________________________________________________________"
        echo "${1}"
        echo ""
}

show_end_message() {
        echo "________________________________________________________________________________"
        echo "Please download and read 13MD05-90 User Manual for further instructions.  "
        echo "https://www.men.de/software/13md05-90/"
}

show_insufficient_rights() {
        echo "*** Insufficient rights"
        echo "*** Please login as root and then call ${this} again"
}

#
# input: arg1 = message to print
#        check for y/n/q
# returns:      0="y"
#               1="n"
#               2="q"
get_ynq_answer() {
        while true
        do
                echo -e -n $1 '(y/n/q): '
                read answer
                case ${answer} in
                [Yy]) return 0;;
                [Nn]) return 1;;
                [Qq]) return 2;;
                esac
        done
}     

#
# input: directory of installed MDIS sources
# returns scan system result
run_scan_system() {
	echo "scanning system. Calling $1/scan_system.sh $1"
	/$1/scan_system.sh $1
}

#
# Function creates/overwrites HISTORY dir when .git is accessible
#
# returns :     0 - HISTORY is available
#       `       1 - HISTORY is unavailable
make_history_script() {

        # Check if .git files exists in directory
        # If YES -> remove, and create fresh HISTORY
        # If NO -> do not remove HISTORY, copy history into MENLINUX_ROOT dir
        #
        local GIT_VERSION
        local MDIS_HISTORY_PATH="${CWD}/${MENHISTORY}"
        GIT_VERSION=$(which git 2> /dev/null)

        if [ -d "${CWD}/.git" ] && [ "${GIT_VERSION}" != "" ]; then
                echo "Removing old history entry ..."
                rm -rf ${MDIS_HISTORY_PATH} &> /dev/null
                result=$?
                if [ ${result} -ne 0 ]; then
                        show_insufficient_rights
                        return 1
                fi
        else
                if [ -d "${MDIS_HISTORY_PATH}" ]; then
                        echo "GIT repository not present, ${MDIS_HISTORY_PATH} availaiable"
                        return 0
                else
                        echo "GIT repository not present, ${MDIS_HISTORY_PATH} not availaiable"
                        echo "MDIS package is broken - exit"
                        return 1
                fi
        fi

        echo "Creating ${MENHISTORY}..."
        mkdir "${MDIS_HISTORY_PATH}"

        # Add 13MD05-90 History
        git --git-dir "${CWD}/.git" log > "${MDIS_HISTORY_PATH}/13MD05-90_history.txt"
        git --git-dir "${CWD}/.git" remote -v > "${MDIS_HISTORY_PATH}/13MD05-90_url.txt"
        git --git-dir "${CWD}/.git" rev-parse --verify HEAD > "${MDIS_HISTORY_PATH}/13MD05-90_version.txt"
        git --git-dir "${CWD}/.git" describe --exact-match --tags $(git --git-dir "${CWD}/.git" rev-parse --verify HEAD) &> "${MDIS_HISTORY_PATH}/13MD05-90_tag.txt"

        # Add history for submodules 13*
        for i in ${CWD}/13* ${CWD}/${MDIS_PACKAGE}; do
        if [ -d "$i" ]; then
                local COMMIT_ID
                local CUR_DIR
                CUR_DIR=$(pwd)
                cd $i
                git log > "${MDIS_HISTORY_PATH}/$(basename ${i})_history.txt"
                git remote -v > "${MDIS_HISTORY_PATH}/$(basename ${i})_url.txt"
                COMMIT_ID=$(git rev-parse --verify HEAD)
                echo "${COMMIT_ID}" > "${MDIS_HISTORY_PATH}/$(basename ${i})_version.txt"
                git describe --exact-match --tags ${COMMIT_ID} &> "${MDIS_HISTORY_PATH}/$(basename ${i})_tag.txt"
                cd ${CUR_DIR}
        fi
        done

}

#
# Check if Installation dir exists. Create Installation directory
#
# returns :     0 - success
#       `       1 - error (insufficient rights/others)
create_installation_directory(){
        if [ ! -d "${MENLINUX_ROOT}" ]; then
                # check if /opt writeable
                if [ ! -w "${OPT}" ]; then
                        show_insufficient_rights
                        return 1
                fi
                # create MENLINUX
                echo "Creating directory ${MENLINUX_ROOT}... "
                mkdir -p ${MENLINUX_ROOT}
                result=$?
                if [ ${result} -ne 0 ]; then
                        show_insufficient_rights
                        return 1
                fi
                chmod 777 ${MENLINUX_ROOT}
                result=$?
                if [ ${result} -ne 0 ]; then
                        show_insufficient_rights
                        return 1
                fi
        fi

        # $MENLINUX_ROOT exists, check if it's writeable
        if [ ! -w "${MENLINUX_ROOT}" ]; then
                show_insufficient_rights
                return 1
        fi
}

#
# Function creates/overwrites HISTORY dir when .git is accessible
#
# returns :     0 - HISTORY is available
#       `       1 - HISTORY is unavailable
overwrite_installation_directory(){
        echo "Checking if ${OPT} exists... "
        if [ ! -d "${OPT}" ]; then
                # /opt doesn't exists
                echo >&2 "*** ${OPT} doesn't exists. Please create and then call ${this} again"
                return 1
        fi

        echo "Checking if ${MENLINUX_ROOT} exists... "
        # ask wether to overwrite /opt/menlinux directory
        if [ -d "${MENLINUX_ROOT}" ]; then
                echo
                echo "Directory ${MENLINUX_ROOT} already exists."
                if [ ${force_install} -ne "1" ]; then
                        get_ynq_answer "Overwrite existing files?"
                        case $? in
                        1 | 2) echo "*** Aborted by user."; return 1;;
                        esac
                fi
        fi
}

#
# Copy sources into installation directory,
#
# returns :     0 - copying of sources is succesfull
#       `       1 - error (insufficient rights/others)
copy_sources_into_installation_directory(){
        cd ${MENLINUX_ROOT}
        echo "Copy ${MDIS_PACKAGE}..."
        rsync -ru --exclude=.git  ${CWD}/${MDIS_PACKAGE}/* . 2> /dev/null
        result=$?
        if [ ${result} -ne 0 ]; then
                show_insufficient_rights
                return 1
        fi

        cp -r "${CWD}/HISTORY" "${MENLINUX_ROOT}/HISTORY"
        for i in * ; do
                if [ -d "$i" ]; then
                        echo "$i"
                fi
        done

        # Copy changelog into ${MENLINUX_ROOT}
        cp  "${CWD}/13MD05-90_changelog.md" "${MENLINUX_ROOT}/13MD05-90_changelog.md"

        echo "Copy MDIS drivers..."

        dirToCopyList=$(ls -l ${CWD} | grep '^d' | awk '{ print $9 }' | grep "13.*")
        # additional directories that have to be copied into install directory
        dirToCopyList+=$'\n'"mdis_ll_mt"

        while read -r i; do
                i=${CWD}/${i}
                if [ -d "$i" ]; then
                        for folder_type in BIN BUILD DOXYGENTMPL DRIVERS INCLUDE LIBSRC LICENSES PACKAGE_DESC TOOLS WINDOWS; do
                                if [ -d "$i/${folder_type}" ]; then
                                        mkdir -p ./${folder_type}
                                        folder_recursive $i/${folder_type} ${MENLINUX_ROOT}/${folder_type}
                                        result=$?
                                        if [ ${result} -ne 0 ]; then
                                                return 1
                                        fi
                                fi
                        done
                fi
        done <<< "${dirToCopyList}"

        # set permissions in "BUILD" and "BIN" directory for all users
        cd ${MENLINUX_ROOT}
        echo "Setting permissions..."
        find -type d -exec chmod 777 {} \; 2> /dev/null
        result=$?
        if [ ${result} -ne 0 ]; then
                show_insufficient_rights
                return 1
        fi
        chmod -R 755 BIN/* 2> /dev/null
        result=$?
        if [ ${result} -ne 0 ]; then
                show_insufficient_rights
                return 1
        fi
}

# Function to copy folder structure of low level driver.
# Function is called recursive
#
# input: arg1 = Source folder 
#        arg2 = Destination folder
folder_recursive() {
        local SRC_FOLDER=$1
        local DST_FOLDER=$2

        # check if subfolder already exists
        for subfolder in ${SRC_FOLDER}/* ; do
                # get base name
                #subfolder_base=$(basename ${subfolder})

                if  [ -f "${subfolder}" ] ; then
                        #################
                        # It is a file
                        #################
                        rsync --exclude=.git ${subfolder} ${DST_FOLDER}/
                        continue;
                else
                        #################
                        # It is a folder
                        #################
                        echo "Install ${subfolder} to ${DST_FOLDER}"
                        rsync -ru --exclude=.git ${subfolder} ${DST_FOLDER}/
                        result=$?
                        if [ ${result} -ne 0 ]; then
                                echo "*** Can't sync MDIS sources due to insufficient rights."
                                echo "*** Please login as root and then call ${this} again"
                                return 1
                        fi
                fi;
        done
}

###############################################################################
###############################################################################
############################## START ##########################################
###############################################################################
###############################################################################

# States:
# - Creation of installation directory
# - Copying/Overwritting sources into installation path
# - Creation of modules history logs
# - Check if this script is running on target
# - Scanning the hardware
# - Run make
# - Run make install

echo "                                                                                "
echo "Installing the MEN MDIS for Linux System Package 13MD05-90_02_00                "
echo "(see MDIS User Manual for details)                                              "
echo "________________________________________________________________________________"
echo "                                                                                "

# User Questions
readonly ASK_INSTALL_MDIS_SOURCES="Would you like to proceed, and install MDIS sources into ${MENLINUX_ROOT}?"

readonly ASK_SCAN_TARGET_SYSTEM="Would you like to create an MDIS configuration for your system by scanning your system's hardware?\n
Note: This make only sense at your MEN target system!"

readonly ASK_BUILD_MDIS_MODULES="Would you like to build MDIS modules with the configuration from the system scan?\n
Note: This requires installed Linux kernel sources/headers at the system!"

readonly ASK_INSTALL_MDIS_MODULES="Would you like to install the built MDIS modules at your system?"

linux_kernel_version=$(readlink -f /usr/src/linux)
readonly CALL_MAKE_INSTALL="The kernel modules are compiled for ${linux_kernel_version}\n
If you update your kernel you have to rebuild the modules for the new kernel!"

run=true
state="OverwriteExistingSources"

# Check if this script is running with root priviligies
if [ "$EUID" -ne 0 ]; then
        show_insufficient_rights
        show_end_message
        exit
fi

# If force_install is set, install is done without asking user for action,
# scan, make, make install is not performed
if [ ${force_install} -ne "1" ]; then
    get_ynq_answer "${ASK_INSTALL_MDIS_SOURCES}"
    case $? in
        1 | 2) echo "*** Aborted by user."; run=false;;
    esac
fi

while ${run}; do
        case ${state} in
        OverwriteExistingSources)
                overwrite_installation_directory
                result=$?
                if [ ${result} -ne 0 ]; then
                        state="Break_Failed"
                        break
                fi
                state="CreateInstallationDir";;
        CreateInstallationDir)
                create_installation_directory
                result=$?
                if [ ${result} -ne 0 ]; then
                        state="Break_Failed"
                        break
                fi
                state="CreateHistory";;
        CreateHistory)
                make_history_script
                result=$?
                if [ ${result} -ne 0 ]; then
                        echo "History of modules is not created !!"
                fi
                state="CopySourcesIntoInstallationDir";;
        CopySourcesIntoInstallationDir)
                copy_sources_into_installation_directory
                result=$?
                if [ ${result} -ne 0 ]; then
                        state="Break_Failed"
                        break
                fi
                show_status_message "Installation success"
                state="CheckTargetSystem";;
        CheckTargetSystem)
                if [ ${force_install} -eq "1" ]; then
                        state="Break_Failed"
                        break
                fi
                state="Scan"
                get_ynq_answer "${ASK_SCAN_TARGET_SYSTEM}"
                case $? in
                        1 | 2)  show_manual_steps
                                state="Break_Failed"
                                break
                esac;;
        Scan)
                curr_dir=$(pwd)
                cd ${CWD}
                state="Make"
                run_scan_system ${MENLINUX_ROOT}
                result=$?
                if [ ${result} -ne 0 ]; then
                        state="Break_Failed"
                        break
                fi
                show_status_message "Scan success"
                cd ${curr_dir};;
        Make)
                curr_dir=$(pwd)
                cd ${CWD}
                state="MakeInstall"
                get_ynq_answer "${ASK_BUILD_MDIS_MODULES}"
                case $? in
                        0 )     make
                                result=$?
                                if [ ${result} -ne 0 ]; then
                                        state="Break_Failed"
                                        break
                                fi
                                show_status_message "Build success";;
                        1 | 2)  echo "*** Aborted by user. "
                                state="Break_Failed";;
                esac
                cd ${curr_dir};;
        MakeInstall)
                curr_dir=$(pwd)
                cd ${CWD}
                state="Break_Success"
                get_ynq_answer "${ASK_INSTALL_MDIS_MODULES}"
                case $? in
                        0 )     make install
                                result=$?
                                if [ ${result} -ne 0 ]; then
                                        state="Break_Failed"
                                        break
                                fi
                                show_status_message ${CALL_MAKE_INSTALL};;
                     1 | 2)     echo "*** Aborted by user. "
                                state="Break_Failed";;
                esac
                cd ${curr_dir};;
        Break_Failed)
                run=false;;
        Break_Success)
                run=false;;
        esac
done

show_end_message
