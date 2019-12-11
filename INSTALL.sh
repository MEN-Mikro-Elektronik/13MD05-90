#!/bin/bash
#*****************************************************************************
#
#       Author: Andreas Geissler
#       Revision: 0.1
#
#       Description: Installation script for MDIS5LINUX system package
#
#-----------------------------------------------------------------------------
# (c) Copyright 2018-2019 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
#*****************************************************************************
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
MDIS_HISTORY_PATH="${CWD}/${MENHISTORY}"

# system package git repository name
MDIS_PACKAGE=MDISforLinux

# default install parameter
install_only="0"
assume_yes="0"

### @brief script usage --help
function install_usage {
    echo "INSTALL.sh   script to install MDIS sources into system"
    echo ""
    echo "parameters:"
    echo "     -y, --yes"
    echo "     --assume-yes      install and scan without user interaction."
    echo "                       answer 'yes' for all questions"
    echo "     --install-only    install sources and exit without user interaction"
    echo "     -p, --path=PATH   specify a installation path"
    echo "     -h, --help        print help"
    echo ""
}

# read parameters
while test $# -gt 0 ; do
   case "$1" in 
        -h|--help)
                install_usage
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
        --install-only)
                shift
                install_only="1"
                echo "Install without asking:"
                echo "- install sources into provided path (default /opt/menlinux)"
                echo "- do not scan the hardware"
                ;;
        -y|--yes|--assume-yes)
                shift
                assume_yes="1"
                echo "Automatic yes to prompts; assume \"yes\" as answer"
                echo "to all prompts and run non-interactively"
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
        echo -e "${1}"
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
    if [ ${assume_yes} -eq "1" ]; then
        echo "scanning system. Calling $1/scan_system.sh $1 --assume-yes"
        /$1/scan_system.sh $1 --assume-yes
    else
        echo "scanning system. Calling $1/scan_system.sh $1"
        /$1/scan_system.sh $1
    fi
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
        GIT_VERSION=$(which git 2> /dev/null)

        if [ -d "${CWD}/.git" ] && [ "${GIT_VERSION}" != "" ]; then
                echo "Removing old history entry ${MDIS_HISTORY_PATH}"
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
        local gitRevision=$(git --git-dir "${CWD}/.git" describe --dirty --long --tags --always)
        local gitDate=$(git --git-dir "${CWD}/.git" --no-pager show -s --date=short --format=format:"%cd%n")
        git --git-dir "${CWD}/.git" log > "${MDIS_HISTORY_PATH}/13MD05-90_history.txt"
        git --git-dir "${CWD}/.git" remote -v > "${MDIS_HISTORY_PATH}/13MD05-90_url.txt"
        echo "${gitRevision}_${gitDate}" > "${MDIS_HISTORY_PATH}/13MD05-90_version.txt"
        git --git-dir "${CWD}/.git" describe --exact-match --tags $(git --git-dir \
"${CWD}/.git" rev-parse --verify HEAD) &> "${MDIS_HISTORY_PATH}/13MD05-90_tag.txt"

        # Add history files for all submodules within 13MD05-90 repository.
        submoduleList=$(git --git-dir "${CWD}/.git" config --file "${CWD}/.gitmodules" \
--get-regexp path | sed 's/submodule.//g' | sed 's/.url//g' | awk '{print $2}' | sed 's/\.\.\///g' | sed 's/.git//g')
        submoduleShortUrlList=$(git --git-dir "${CWD}/.git" config --file "${CWD}/.gitmodules" \
--get-regexp url | sed 's/submodule.//g' | sed 's/.url//g' | awk '{print $2}' | sed 's/\.\.\///g' | sed 's/.git//g')

        rm "${MDIS_HISTORY_PATH}/13MD05-90_submodules.txt" 2> /dev/null
        submoduleCnt=$(echo "${submoduleList}" | wc -l)
        for ((i=1;i<=submoduleCnt;i++)); do
           echo $(echo "${submoduleList}" | awk -v i="${i}" NR==i) " " $(echo "${submoduleShortUrlList}" \
| awk -v i="${i}" NR==i) >> "${MDIS_HISTORY_PATH}/13MD05-90_submodules.txt"
        done

        while read -r var1 var2; do
                i=${CWD}/${var1}
                if [ -d "${i}" ]; then
                    cd ${i}
                    git log > "${MDIS_HISTORY_PATH}/${var2}_history.txt"
                    git remote -v > "${MDIS_HISTORY_PATH}/${var2}_url.txt"
                    gitRevision=$(git describe --dirty --long --tags --always)
                    gitDate=$(git --no-pager show -s --date=short --format=format:"%cd%n")
                    echo "${gitRevision}_${gitDate}" > "${MDIS_HISTORY_PATH}/${var2}_version.txt"
                    git describe --exact-match --tags $(git rev-parse --verify HEAD) &> "${MDIS_HISTORY_PATH}/${var2}_tag.txt"
                    cd ${CWD}
                fi
        done < <(cat "${MDIS_HISTORY_PATH}/13MD05-90_submodules.txt")

}

#
# Check if Installation dir exists. Create Installation directory
#
# returns :     0 - success
#       `       1 - error (insufficient rights/others)
create_installation_directory(){
        if [ ! -d "${MENLINUX_ROOT}" ]; then
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
# Function check if MDIS Sources are available in system, and ask user
# to overwrite, proceed or exit
#
# returns :     0 - Overwrite MDIS sources
#               1 - proceed without overwriting files
#               2 - force exit
#
overwrite_installation_directory(){
        echo "Checking if ${MENLINUX_ROOT} exists... "
        # ask to overwrite /opt/menlinux directory
        if [ -d "${MENLINUX_ROOT}" ]; then
                echo "Directory ${MENLINUX_ROOT} already exists."
                if [ ${install_only} -eq "0" ] && [ ${assume_yes} -eq "0" ]; then
                        get_ynq_answer "Overwrite existing files?"
                        case $? in
                        1) echo "Do not overwrite, proceed"; return 1;;
                        2) echo "*** Aborted by user."; return 2;;
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

        echo "Copy History..."
        rsync -ru --exclude=.git  ${CWD}/${MENHISTORY}/* ${MENHISTORY}/ 2> /dev/null
        result=$?
        if [ ${result} -ne 0 ]; then
                show_insufficient_rights
                return 1
        fi

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

        # update *.mak and *.xml files
        echo "Makefiles update ..."
        cd ${CWD}
        update_makefiles
        echo "XML files update ..."
        update_xmlfiles

        # set permissions in "BUILD" and "BIN" directory for all users
        cd ${MENLINUX_ROOT}
        echo "Setting permissions..."
        find -type d -exec chmod 777 {} \; 2> /dev/null
        find -type f -exec chmod a+rw '{}' \; 2> /dev/null
        result=$?
        if [ ${result} -ne 0 ]; then
                show_insufficient_rights
                return 1
        fi
        chmod 777 BIN/fpga_load 2> /dev/null
        chmod 777 BIN/mdiswiz 2> /dev/null
        chmod 777 BIN/mm_ident 2> /dev/null
        result=$?
        if [ ${result} -ne 0 ]; then
                show_insufficient_rights
                return 1
        fi
}

#
# Update STAMPED_REVISION in makefiles
#
#
#
update_makefiles(){
    makFilesToUpdate=$(find . -type f -name "*.mak" -exec grep -Hl "STAMPED_REVISION=" {} \;)
    local makFilesToUpdateMainRepo=${makFilesToUpdate}
    # update files in submodules
    local makFilesModified=0
    while read -r var1 var2; do
        local makFilesInSubmodule
        makFilesInSubmodule=$(echo "${makFilesToUpdate}" | grep "${var1}")
        if [ ! -z "${makFilesInSubmodule}" ]; then
            while read -r makfile; do
                local stampedRevision=$(cat "${MDIS_HISTORY_PATH}/${var2}_version.txt")
                local menlinuxMakPath=$(echo "${makfile}" | awk -F / '{for (i=3; i<NF; i++) printf $i "/"; print $NF}')
                sed -i 's/'"STAMPED_REVISION=.*"'/'"STAMPED_REVISION=${stampedRevision}"'/g' ${MENLINUX_ROOT}/${menlinuxMakPath}
                makFilesModified=$((${makFilesModified}+1))
                makFilesToUpdateMainRepo=$(grep -v "${menlinuxMakPath}" <<< "${makFilesToUpdateMainRepo}")
            done <<< "${makFilesInSubmodule}"
        fi
    done < <(cat "${MDIS_HISTORY_PATH}/13MD05-90_submodules.txt")

    # update files in main repository
    if [ ! -z "${makFilesToUpdateMainRepo}" ]; then
        while read -r makfile; do
            local stampedRevision=$(cat "${MDIS_HISTORY_PATH}/13MD05-90_version.txt")
            local menlinuxMakPath=$(echo "${makfile}" | awk -F / '{for (i=3; i<NF; i++) printf $i "/"; print $NF}')
            sed -i 's/'"STAMPED_REVISION=.*"'/'"STAMPED_REVISION=${stampedRevision}"'/g' ${MENLINUX_ROOT}/${menlinuxMakPath}
            makFilesModified=$((${makFilesModified}+1))
        done <<< "${makFilesToUpdateMainRepo}"
    fi

    if [ -z "${makFilesModified}" ]; then
        echo "... no files ..."
    else
        echo "STAMPED_REVISION has been updated in ${makFilesModified} mak files"
    fi
}

#
# Update <revision> in xml files
# Update <date> in xml files
#
#
update_xmlfiles(){
    # create list of all *.xml files that need to be updated during INSTALL
    # update files in submodules
    xmlFilesToUpdate=$(find . -type f -name "*.xml" -exec grep -Hl "<revision>.*<\/revision>" {} \;)
    xmlFilesToUpdateMainRepo=${xmlFilesToUpdate}
    local xmlFilesModified=0
    while read -r var1 var2; do
        local xmlFilesInSubmodule
        xmlFilesInSubmodule=$(echo "${xmlFilesToUpdate}" | grep "${var1}")
        if [ ! -z "${xmlFilesInSubmodule}" ]; then
            while read -r xmlfile; do
                local revisiondate=""
                local revisionEndIdx=0
                local revision=$(cat "${MDIS_HISTORY_PATH}/${var2}_version.txt")
                local menlinuxXmlPath=$(echo "${xmlfile}" | awk -F"/" '{print $NF}')
                revisionEndIdx=$(echo ${revision} | awk -F"_" '{print length($0)-length($NF)}')
                revisionEndIdx=$((${revisionEndIdx}-1))
                revisiondate=$(echo ${revision} | awk -F"_" '{print $NF}')
                revision=$(echo ${revision} | cut -c1-${revisionEndIdx})
                xmlFilesToUpdateMainRepo=$(grep -v "${menlinuxXmlPath}" <<< "${xmlFilesToUpdateMainRepo}")
                menlinuxXmlPath=$(echo "${MENLINUX_ROOT}/PACKAGE_DESC/${menlinuxXmlPath}")
                sed -i 's|'"<date>.*</date>"'|'"<date>${revisiondate}</date>"'|g' ${menlinuxXmlPath}
                sed -i 's|'"<revision>.*</revision>"'|'"<revision>${revision}</revision>"'|g' ${menlinuxXmlPath}
                xmlFilesModified=$((${xmlFilesModified}+1))
            done <<< "${xmlFilesInSubmodule}"
        fi
    done < <(cat "${MDIS_HISTORY_PATH}/13MD05-90_submodules.txt")

    # update files in main repository
    if [ ! -z "${xmlFilesToUpdateMainRepo}" ]; then
        while read -r xmlfile; do
            local revisiondate=""
            local revisionEndIdx=0
            local revision=$(cat "${MDIS_HISTORY_PATH}/13MD05-90_version.txt")
            local menlinuxXmlPath=$(echo "${xmlfile}" | awk -F"/" '{print $NF}')
            revisionEndIdx=$(echo ${revision} | awk -F"_" '{print length($0)-length($NF)}')
            revisionEndIdx=$((${revisionEndIdx}-1))
            revisiondate=$(echo ${revision} | awk -F"_" '{print $NF}')
            revision=$(echo ${revision} | cut -c1-${revisionEndIdx})
            menlinuxXmlPath=$(echo "${MENLINUX_ROOT}/PACKAGE_DESC/${menlinuxXmlPath}")
            sed -i 's|'"<date>.*</date>"'|'"<date>${revisiondate}</date>"'|g' ${menlinuxXmlPath}
            sed -i 's|'"<revision>.*</revision>"'|'"<revision>${revision}</revision>"'|g' ${menlinuxXmlPath}
            xmlFilesModified=$((${xmlFilesModified}+1))
        done <<< "${xmlFilesToUpdateMainRepo}"
    fi

    if [ -z "${xmlFilesModified}" ]; then
        echo "... no files ..."
    else
        echo "DATE and REVISION has been updated in ${xmlFilesModified} xml files"
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
echo "Installing the MEN MDIS for Linux System Package 13MD05-90_02_01                "
echo "(see MDIS User Manual for details)                                              "
echo "________________________________________________________________________________"
echo "                                                                                "

# User Questions
readonly ASK_INSTALL_MDIS_SOURCES="Would you like to install MDIS sources into ${MENLINUX_ROOT}?"

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
# root priviligies are required while scanning the hardware
if [ ${install_only} -eq "0" ] && [ $EUID -ne "0" ]; then
        show_insufficient_rights
        show_end_message
        exit
fi

# If install_only is set, install is done without asking user for action,
# scan, make, make install is not performed
# If assume_yes is set, install is done without asking user for action,
# scan, make, make install is performed
if [ ${install_only} -eq "0" ] && [ ${assume_yes} -eq "0" ]; then
    get_ynq_answer "${ASK_INSTALL_MDIS_SOURCES}"
    case $? in
        1) echo "proceed to scan..."
           state="CheckTargetSystem";;
        2) echo "*** Aborted by user."
           run=false;;
    esac
fi

while ${run}; do
        case ${state} in
        OverwriteExistingSources)
                overwrite_installation_directory
                case $? in
                    0) state="CreateInstallationDir";;
                    1) state="CheckTargetSystem";;
                    2) state="Break_Failed";;
                esac
                ;;
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
                if [ ${install_only} -eq "1" ]; then
                        state="Break_Failed"
                        break
                fi
                state="Scan"
                if [ ${assume_yes} -ne "1" ]; then
                    get_ynq_answer "${ASK_SCAN_TARGET_SYSTEM}"
                    case $? in
                            1 | 2)  show_manual_steps
                                    state="Break_Failed"
                                    break
                    esac
                fi
                ;;
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
                if [ ${assume_yes} -ne "1" ]; then
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
                else
                    make
                    result=$?
                    if [ ${result} -ne 0 ]; then
                            state="Break_Failed"
                            break
                    fi
                fi
                cd ${curr_dir};;
        MakeInstall)
                curr_dir=$(pwd)
                cd ${CWD}
                state="Break_Success"
                if [ ${assume_yes} -ne "1" ]; then
                    get_ynq_answer "${ASK_INSTALL_MDIS_MODULES}"
                    case $? in
                            0 )     make install
                                    result=$?
                                    if [ ${result} -ne 0 ]; then
                                            state="Break_Failed"
                                            break
                                    fi
                                    show_status_message "${CALL_MAKE_INSTALL}";;
                         1 | 2)     echo "*** Aborted by user. "
                                    state="Break_Failed";;
                    esac
                else
                    make install
                    result=$?
                    if [ ${result} -ne 0 ]; then
                            state="Break_Failed"
                            break
                    fi
                    show_status_message "${CALL_MAKE_INSTALL}"
                fi
                cd ${curr_dir};;
        Break_Failed)
                run=false;;
        Break_Success)
                run=false;;
        esac
done

show_end_message
