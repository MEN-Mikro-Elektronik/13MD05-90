#!/bin/bash
#-----------------------------------------------------------------------------
# Copyright 2020, MEN Mikro Elektronik GmbH
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

### @brief M-Module ID to XML file map
declare -A mmodFileList
### @brief MDIS driver packages to xml file map
declare -A mdisDriverFileList

### @brief Parse XML file
### @details SAX parser.
### Callback function is called on events like tag start (startElement), tag end
### (endElement) and tag content (characters).
### On tag start event callback function is called with following arguments:
### "callbackArgument" "startElement" "/current/path" "tagName" "[attr]="value""
### On tag end event callback function is called with following arguments:
### "callbackArgument" "endElement" "/current/path" "tagName"
### On tag content event callback function is called with following arguments:
### "callbackArgument" "characters" "/current/path" "tag content"
### Known limitations: only valid XML syntax allowed; no tag embedded in comment
### allowed; no CDATA recognized
### @param $1 Input XML file
### @param $2 Name of callback function
### @param $3 Callback function argument
xmlParseXml() {
    local -r EV_START="startElement"
    local -r EV_END="endElement"
    local -r EV_CHARS="characters"
    local -r RE_XML="^(/?)([[:alnum:]]+)(([[:space:]]+[:[:alpha:]]+=\"[^\"]+\")*)(/?)$"
    local -r RE_ATTR="s/([:[:alnum:]]+)=\"([^\"]+)\"/[\1]=\"\2\"/g"
    local inFile
    local cbFunc
    local cbArg
    local IFS
    local xmlPath
    local strLen
    local escArg
    local escPath
    local escTag
    local escAttr
    local escChars

    inFile="${1}"
    cbFunc="${2}"
    cbArg="${3}"
    xmlPath=""

    if [ ! -f "${inFile}" ]; then
        return "1"
    fi

    IFS=">"
    while read -rd "<" "xmlEntity" "xmlContent"; do
        IFS=$' \t\n'
        if [ "${cbFunc}" != "" ] && \
            [ "${xmlEntity}" != "" ] && \
            [[ "${xmlEntity}" =~ ${RE_XML} ]]; then
            if [ "${BASH_REMATCH[1]}" == "" ]; then
                escArg="$(printf '%q' "${cbArg}")"
                escPath="$(printf '%q' "${xmlPath}")"
                escTag="$(printf '%q' "${BASH_REMATCH[2]}")"
                escAttr="$(sed -E "${RE_ATTR}" <<< "${BASH_REMATCH[3]}")"
                escAttr="$(printf '%q' "${escAttr}")"
                eval "${cbFunc}" "${escArg}" "${EV_START}" "${escPath}" "${escTag}" "${escAttr}"
                xmlPath+="/${BASH_REMATCH[2]}"
                if [ "${xmlContent}" != "" ]; then
                    escArg="$(printf '%q' "${cbArg}")"
                    escPath="$(printf '%q' "${xmlPath}")"
                    escChars="$(printf '%q' "${xmlContent}")"
                    eval "${cbFunc}" "${escArg}" "${EV_CHARS}" "${escPath}" "${escChars}"
                fi
            fi
            if [ "${BASH_REMATCH[1]}" == "/" ] || \
                [ "${BASH_REMATCH[5]}" == "/" ]; then
                escArg="$(printf '%q' "${cbArg}")"
                escPath="$(printf '%q' "${xmlPath}")"
                escTag="$(printf '%q' "${BASH_REMATCH[2]}")"
                eval "${cbFunc}" "${escArg}" "${EV_END}" "${escPath}" "${escTag}"
                strLen="${#xmlPath}"
                strLen="$((strLen-${#BASH_REMATCH[2]}))"
                strLen="$((strLen-1))"
                xmlPath="${xmlPath:0:${strLen}}"
            fi
        fi
        IFS=">"
    done < "${inFile}"
}

### @brief Create M-Module to XML file mapping associative array
makeMmodFileMap() {
    local xFiles
    local xFile

    if [ "${#mmodFileList[@]}" != "0" ]; then
        return "0"
    fi

    echo -n "Building M-Module database..."
    xFiles=($(ls "${MEN_LIN_DIR}/PACKAGE_DESC/"13m*.xml 2> "/dev/null"))
    for xFile in "${xFiles[@]}"; do
        echo -n "."
        xmlParseXml "${xFile}" "makeMmodFileMapCallback" "${xFile##*/}"
    done
    echo "done!"
}

### @brief xmlParseXml() callback for makeMmodFileMap()
### @param $1 Callback argument
### @param $2 Event reason
### @param $3 Current xPath
### @param $4 Event specific data#1
### @param $5 Event specific data#2
makeMmodFileMapCallback() {
    if [ "${2}" == "characters" ] && \
        [ "${3}" == "/package/modellist/model/autoid/mmoduleid" ] && \
        [ "${mmodFileList["${4}"]}" == "" ]; then
        mmodFileList+=(["${4}"]="${1}")
    fi
}

### @brief Create M-Module output data
### @param $1 M-Module ID
makeMmodOutputData() {
    local xId
    local xFile
    local -A xModel
    local -A xModuleSettings
    local -A xModule

    xId="${1}"

    mmodSpecList=()

    xFile="${mmodFileList["${xId}"]}"
    if [ "${xFile}" == "" ]; then
        return "1"
    fi

    xmlParseXml "${MEN_LIN_DIR}/PACKAGE_DESC/${xFile}" "makeMmodOutputDataCallback" "${xId}"
}

### @brief xmlParseXml() callback for makeMmodOutputData()
### @param $1 Callback argument
### @param $2 Event reason
### @param $3 Current xPath
### @param $4 Event specific data#1
### @param $5 Event specific data#2
makeMmodOutputDataCallback() {
    local xKey

    if [ "${2}" == "startElement" ]; then
        if [ "${3}" == "/package/modellist" ] && \
            [ "${4}" == "model" ]; then
            xModel=()
        elif [[ ("${3}" == "/package/modellist/model/swmodulelist" || \
            "${3}" == "/package/swmodulelist") && \
            "${4}" == "swmodule" ]]; then
            xModule=()
            if [ "${5}" != "" ]; then
                eval "xModule+=(${5})"
            fi
        elif [ "${3}" == "/package/settinglist" ] && \
              [ "${4}" == "setting" ]; then
            xModuleSettings=()
        fi
    elif [ "${2}" == "characters" ]; then
        if [ "${3}" == "/package/modellist/model/hwname" ]; then
            xModel+=(["hwname"]="${4}")
        elif [ "${3}" == "/package/modellist/model/modelname" ]; then
            xModel+=(["modelname"]="${4}")
        elif [ "${3}" == "/package/modellist/model/autoid/mmoduleid" ]; then
            xModel+=(["mmoduleid"]="${4}")
        elif [ "${3}" == "/package/modellist/model/subdevofftbl" ]; then
            xModel+=(["subdevofftbl"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/name" ] || \
            [ "${3}" == "/package/swmodulelist/swmodule/name" ]; then
            xModule+=(["name"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/type" ] || \
            [ "${3}" == "/package/swmodulelist/swmodule/type" ]; then
            xModule+=(["type"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/makefilepath" ] || \
            [ "${3}" == "/package/swmodulelist/swmodule/makefilepath" ]; then
            xModule+=(["makefilepath"]="${4}")
        elif [ "${3}" == "/package/settinglist/setting/name" ]; then
            xModuleSettings+=(["settingname"]="${4}")
        elif [ "${3}" == "/package/settinglist/setting/type" ]; then
            xModuleSettings+=(["settingtype"]="${4}")
        elif [ "${3}" == "/package/settinglist/setting/value" ]; then
            xModuleSettings+=(["settingvalue"]="${4}")
        elif [ "${3}" == "/package/settinglist/setting/defaultvalue" ]; then
            xModuleSettings+=(["settingdefaultvalue"]="${4}")
        fi
    elif [ "${2}" == "endElement" ]; then
        if [ "${3}" == "/package/modellist/model" ] && \
            [ "${4}" == "model" ]; then
            if [ "${1}" == "${xModel["mmoduleid"]}" ] && \
                [ "${mmodSpecList["mmoduleid"]}" == "" ]; then
                for xKey in "${!xModel[@]}"; do
                    mmodSpecList+=(["${xKey}"]="${xModel["${xKey}"]}")
                done
            fi
            xModel=()
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule" ] && \
            [ "${4}" == "swmodule" ]; then
            if [[ "${1}" == "${xModel["mmoduleid"]}" && \
                ( "${xModel["internal"]}" != "true" ||
                "${INTERNAL_SWMODULES}" != "0" ) ]]; then
                if [ "${xModel["name"]}" == "" ]; then
                    xModel+=(["name"]="${xModule["name"]}")
                fi
                if [ "${xModule["type"]}" != "" ] && \
                    [ "${xModule["makefilepath"]}" != "" ]; then
                    if [ "${xModel["${xModule["type"]}"]}" != "" ]; then
                        xModel["${xModule["type"]}"]+=" "
                    fi
                    xModel["${xModule["type"]}"]+="${xModule["makefilepath"]}"
                fi
            fi
            xModule=()
        elif [ "${3}" == "/package/swmodulelist/swmodule" ] && \
            [ "${4}" == "swmodule" ]; then
            if [[ "${xModel["internal"]}" != "true" ||
                "${INTERNAL_SWMODULES}" != "0" ]]; then
                if [ "${mmodSpecList["name"]}" == "" ]; then
                    mmodSpecList+=(["name"]="${xModule["name"]}")
                fi
                if [ "${xModule["type"]}" != "" ] && \
                    [ "${xModule["makefilepath"]}" != "" ]; then
                    if [ "${mmodSpecList["${xModule["type"]}"]}" != "" ]; then
                        mmodSpecList["${xModule["type"]}"]+=" "
                    fi
                    mmodSpecList["${xModule["type"]}"]+="${xModule["makefilepath"]}"
                fi
            fi
            xModule=()
        elif [ "${3}" == "/package/settinglist/setting" ] && \
             [ "${4}" == "setting" ]; then
                if [ "${xModuleSettings["settingname"]}" != "" ] && \
                    [ "${xModuleSettings["settingtype"]}" != "" ] && \
                    [ "${xModuleSettings["settingvalue"]}" != "" ] && \
                    [ "${xModuleSettings["settingdefaultvalue"]}" != "" ]; then
                      if [ "${xModuleSettings["settingvalue"]}" != "${xModuleSettings["settingdefaultvalue"]}" ]; then
                          local settingNo=0
                          if [ "${mmodSpecList["settingno"]}" != "" ]; then
                              settingNo="${mmodSpecList["settingno"]}"
                          fi
                          settingNo=$((settingNo+1))
                          mmodSpecList["settingno"]=${settingNo}
                          mmodSpecList["settingname${settingNo}"]="${xModuleSettings["settingname"]}"
                          mmodSpecList["settingtype${settingNo}"]="${xModuleSettings["settingtype"]}"
                          mmodSpecList["settingvalue${settingNo}"]="${xModuleSettings["settingvalue"]}"
                      fi
                fi
            xModuleSettings=()
        fi
    fi
}

### @brief Get M-Module ID
### @param $1 M-Module address
### @return M-Module ID is echoed
### @return Empty string is echoed on error
getMmodId() {
    local mmAddress
    local mmId
    local mmMagic
    local mmIdent

    mmAddress="${1}"

    mmMagic="$("${MEN_LIN_DIR}"/BIN/"${MM_IDENT}" "${mmAddress}" | grep "MAGIC:" | head -n 1)"
    if [[ "${mmMagic}" =~ ^MAGIC:[[:space:]]0x([[:xdigit:]]{4})$ ]]; then
        mmMagic="${BASH_REMATCH[1]}"
        mmIdent="$("${MEN_LIN_DIR}"/BIN/"${MM_IDENT}" "${mmAddress}" | grep "ID:" | head -n 1)"
        if [[ "${mmIdent}" =~ [[:space:]]ID:[[:space:]]0x([[:xdigit:]]{4}), ]]; then
            mmIdent="${BASH_REMATCH[1]}"
            mmId="0x${mmMagic}${mmIdent}"
        fi
    fi

    echo "${mmId}"
}

### @brief Create IP core to XML file mapping associative array
makeIpCoreFileMap() {
    local xFiles
    local xFile

    if [ "${#ipcoreFileList[@]}" != "0" ]; then
        return "0"
    fi

    echo -n "Building IP core database..."
    xFiles=($(ls "${MEN_LIN_DIR}/PACKAGE_DESC/"13z*.xml 2> "/dev/null"))
    for xFile in "${xFiles[@]}"; do
        echo -n "."
        xmlParseXml "${xFile}" "makeIpCoreFileMapCallback" "${xFile##*/}"
    done
    echo "done!"
}

### @brief xmlParseXml() callback for makeIpCoreFileMap()
### @param $1 Callback argument
### @param $2 Event reason
### @param $3 Current xPath
### @param $4 Event specific data#1
### @param $5 Event specific data#2
makeIpCoreFileMapCallback() {
    if [ "${2}" == "characters" ] && \
        [ "${3}" == "/package/modellist/model/autoid/chamv2id" ] && \
        [ "${ipcoreFileList["${4}"]}" == "" ]; then
        ipcoreFileList+=(["${4}"]="${1}")
    fi
}

### @brief Create IP core output data
### @param $1 IP core ID
makeIpCoreOutputData() {
    local xId
    local xFile
    local -A xModel
    local -A xModule
    local -A xSetting
    local xName

    xId="${1}"

    ipcoreSpecList=()

    xFile="${ipcoreFileList["${xId}"]}"
    if [ "${xFile}" == "" ]; then
        return "1"
    fi

    xmlParseXml "${MEN_LIN_DIR}/PACKAGE_DESC/${xFile}" "makeIpCoreOutputDataCallback" "${xId}"
}

### @brief xmlParseXml() callback for makeIpCoreOutputData()
### @param $1 Callback argument
### @param $2 Event reason
### @param $3 Current xPath
### @param $4 Event specific data#1
### @param $5 Event specific data#2
makeIpCoreOutputDataCallback() {
    local xKey
    local xVal
    local xSize
    local xMap
    local xIdx

    if [ "${2}" == "startElement" ]; then
        if [ "${3}" == "/package/modellist" ] && \
            [ "${4}" == "model" ]; then
            xModel=()
            xSize="${#ipcoreSpecList[@]}"
            xName="ipcoreSwModule${xSize}"
            mapNew "${xName}"
        elif [[ ( "${3}" == "/package/modellist/model/swmodulelist" || \
            "${3}" == "/package/swmodulelist" ) && \
            "${4}" == "swmodule" ]]; then
            xModule=()
            if [ "${5}" != "" ]; then
                eval "xModule+=(${5})"
            fi
        elif [ "${3}" == "/package/modellist/model/settinglist" ] && \
            [ "${4}" == "setting" ]; then
            xSetting=()
        fi
    elif [ "${2}" == "characters" ]; then
        if [ "${3}" == "/package/modellist/model/hwname" ]; then
            xModel+=(["hwname"]="${4}")
        elif [ "${3}" == "/package/modellist/model/modelname" ]; then
            xModel+=(["modelname"]="${4}")
        elif [ "${3}" == "/package/modellist/model/description" ]; then
            xModel+=(["description"]="${4}")
        elif [ "${3}" == "/package/modellist/model/autoid/chamv2id" ]; then
            xModel+=(["chamv2id"]="${4}")
        elif [ "${3}" == "/package/modellist/model/devnameprefix" ]; then
            xModel+=(["devnameprefix"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/name" ]; then
            xModule+=(["name"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/type" ] || \
            [ "${3}" == "/package/swmodulelist/swmodule/type" ]; then
            xModule+=(["type"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/makefilepath" ] || \
            [ "${3}" == "/package/swmodulelist/swmodule/makefilepath" ]; then
            xModule+=(["makefilepath"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/os" ] || \
            [ "${3}" == "/package/swmodulelist/swmodule/os" ]; then
            xModule+=(["os"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/notos" ] || \
            [ "${3}" == "/package/swmodulelist/swmodule/notos" ]; then
            if [ "${4}" == "Linux" ]; then
                xModule+=(["notos"]="${4}")
            fi
        elif [ "${3}" == "/package/swmodulelist/swmodule/name" ]; then
            if [ "${xModule["swname"]}" == "" ]; then
                xModule+=(["swname"]="${4}")
            fi
        elif [ "${3}" == "/package/modellist/model/settinglist/setting/name" ]; then
            xSetting+=(["name"]="${4}")
        elif [ "${3}" == "/package/modellist/model/settinglist/setting/type" ]; then
            xSetting+=(["type"]="${4}")
        elif [ "${3}" == "/package/modellist/model/settinglist/setting/value" ]; then
            xSetting+=(["value"]="${4}")
        fi
    elif [ "${2}" == "endElement" ]; then
        if [ "${3}" == "/package/modellist/model" ] && \
            [ "${4}" == "model" ]; then
            if [ "${1}" == "${xModel["chamv2id"]}" ] && \
                [ "${xModel["name"]: -3}" != "_io" ]; then
                ipcoreSpecList+=("${xName}")
                for xKey in "${!xModel[@]}"; do
                    mapPut "${xName}" "${xKey// /_}" "${xModel["${xKey}"]}"
                done
            else
                mapDelete "${xName}"
            fi
            xModel=()
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule" ] && \
            [ "${4}" == "swmodule" ]; then
            if [[ "${1}" == "${xModel["chamv2id"]}" && \
                ( "${xModel["internal"]}" != "true" || \
                "${INTERNAL_SWMODULES}" != "0" ) ]]; then
                if [ "${xModel["name"]}" == "" ]; then
                    xModel+=(["name"]="${xModule["name"]}")
                fi
                if [ "${xModule["type"]}" != "" ] && \
                    [ "${xModule["makefilepath"]}" != "" ]; then
                    if [ "${xModel["${xModule["type"]}"]}" != "" ]; then
                        xModel["${xModule["type"]}"]+=" "
                    fi
                    xModel["${xModule["type"]}"]+="${xModule["makefilepath"]}"
                fi
            fi
            xModule=()
        elif [ "${3}" == "/package/swmodulelist/swmodule" ] && \
            [ "${4}" == "swmodule" ]; then
            if [[ "${xModule["internal"]}" != "true" || \
                "${INTERNAL_SWMODULES}" != "0" ]]; then
                if [[ "${xModule["type"]}" != "" && \
                    "${xModule["makefilepath"]}" != "" && \
                    ( ( "${xModule["os"]}" == "" || \
                    "${xModule["os"]}" == "Linux" ) && \
                    "${xModule["notos"]}" != "Linux" ) ]]; then
                    xSize="${#ipcoreSpecList[@]}"
                    for (( xIdx=0; xIdx<xSize; xIdx++ )); do
                        xMap="${ipcoreSpecList[${xIdx}]}"
                        xKey="${xModule["type"]}"
                        xKey="${xKey// /_}"
                        xVal="$(mapGet "${xMap}" "${xKey}")"
                        if [ "${xVal}" == "" ]; then
                            xVal="${xModule["makefilepath"]}"
                        else
                            xVal+=" ${xModule["makefilepath"]}"
                        fi
                        mapPut "${xMap}" "${xKey}" "${xVal}"
                        xKey="swname"
                        xVal="${xModule[${xKey}]}"
                        if [ "${xVal}" != "" ]; then
                            if ! mapKey "${xMap}" "${xKey}"; then
                                mapPut "${xMap}" "${xKey}" "${xVal}"
                            fi
                        fi
                    done
                fi
            fi
            xModule=()
        elif [ "${3}" == "/package/modellist/model/settinglist/setting" ] && \
            [ "${4}" == "setting" ]; then
            if [ "${xSetting["value"]}" != "" ]; then
                xSet="${xSetting["name"]} = ${xSetting["type"]} ${xSetting["value"]}"
                if [ "${xModel["settings"]}" == "" ]; then
                    xModel+=(["settings"]="${xSet}")
                else
                    xModel["settings"]+=$'\n'
                    xModel["settings"]+="    ${xSet}"
                fi
            fi
            xSetting=()
        fi
    fi
}

### @brief Create MDIS Drivers XML file mapping associative array
makeMdisDriversFileMap() {
    local xFiles
    local xFile

    if [ "${#mdisDriverFileList[@]}" != "0" ]; then
        return "0"
    fi

    echo -n "Building MDIS driver database..."
    xFiles=($(ls "${MEN_LIN_DIR}/PACKAGE_DESC/"13y*.xml 2> "/dev/null"))
    xFiles=("${xFiles[@]}" $(ls "${MEN_LIN_DIR}/PACKAGE_DESC/"13xm*.xml 2> "/dev/null"))
    xFiles=("${xFiles[@]}" $(echo "${MEN_LIN_DIR}/PACKAGE_DESC/13pp0406.xml"))
    xFiles=("${xFiles[@]}" $(echo "${MEN_LIN_DIR}/PACKAGE_DESC/13p7c906.xml"))
    for xFile in "${xFiles[@]}"; do
        echo -n "."
        xmlParseXml "${xFile}" "makeMdisDriverFileMapCallback" "${xFile##*/}"
    done
    echo "done!"
}

### @brief xmlParseXml() callback for makeMdisDriversFileMap()
### @param $1 Callback argument
### @param $2 Event reason
### @param $3 Current xPath
### @param $4 Event specific data#1
makeMdisDriverFileMapCallback() {
    if [ "${2}" == "characters" ] && \
        [ "${3}" == "/package/modellist/model/hwname" ] && \
        [ "${mdisDriverFileList["${4}"]}" == "" ]; then
        mdisDriverFileList+=(["${4}"]="${1}")
    fi
}

### @brief Create IP core output data
### @param $1 IP core ID
makeMdisDriverOutputData() {
    local xId
    local xFile
    local -A xModel
    local -A xModule

    xId="${1}"

    mdisDriverSpecList=()

    xFile="${mdisDriverFileList["${xId}"]}"
    if [ "${xFile}" == "" ]; then
        return "1"
    fi

    xmlParseXml "${MEN_LIN_DIR}/PACKAGE_DESC/${xFile}" "makeMdisDriverOutputDataCallback" "${xId}"
}

### @brief xmlParseXml() callback for makeMdisDriverOutputData()
### @param $1 Callback argument
### @param $2 Event reason
### @param $3 Current xPath
### @param $4 Event specific data#1
### @param $5 Event specific data#2
makeMdisDriverOutputDataCallback() {
    local xKey

    if [ "${2}" == "startElement" ]; then
        if [ "${3}" == "/package/modellist" ] && \
            [ "${4}" == "model" ]; then
            xModel=()
        elif [[ ("${3}" == "/package/modellist/model/swmodulelist" || \
            "${3}" == "/package/swmodulelist") && \
            "${4}" == "swmodule" ]]; then
            xModule=()
            if [ "${5}" != "" ]; then
                eval "xModule+=(${5})"
            fi
        fi
    elif [ "${2}" == "characters" ]; then
        if [ "${3}" == "/package/modellist/model/hwname" ]; then
            xModel+=(["hwname"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/name" ] || \
            [ "${3}" == "/package/swmodulelist/swmodule/name" ]; then
            xModule+=(["name"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/type" ] || \
            [ "${3}" == "/package/swmodulelist/swmodule/type" ]; then
            xModule+=(["type"]="${4}")
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule/makefilepath" ] || \
            [ "${3}" == "/package/swmodulelist/swmodule/makefilepath" ]; then
            xModule+=(["makefilepath"]="${4}")
        fi
    elif [ "${2}" == "endElement" ]; then
        if [ "${3}" == "/package/swmodulelist/swmodule" ] && \
            [ "${4}" == "swmodule" ]; then
            if [[ "${xModule["internal"]}" != "true" ||
                "${INTERNAL_SWMODULES}" != "0" ]]; then
                if [ "${mdisDriverSpecList["name"]}" == "" ]; then
                    mdisDriverSpecList+=(["name"]="${xModule["name"]}")
                fi
                if [ "${xModule["type"]}" != "" ] && \
                    [ "${xModule["makefilepath"]}" != "" ]; then
                    if [ "${mdisDriverSpecList["${xModule["type"]}"]}" != "" ]; then
                        mdisDriverSpecList["${xModule["type"]}"]+=" "
                    fi
                    mdisDriverSpecList["${xModule["type"]}"]+="${xModule["makefilepath"]}"
                fi
            fi
        xModule=()
        elif [ "${3}" == "/package/modellist/model/swmodulelist/swmodule" ] && \
              [ "${4}" == "swmodule" ]; then
            if [[ "${xModule["internal"]}" != "true" ||
                "${INTERNAL_SWMODULES}" != "0" ]]; then
                if [ "${mdisDriverSpecList["name"]}" == "" ]; then
                    mdisDriverSpecList+=(["name"]="${xModule["name"]}")
                fi
                if [ "${xModule["type"]}" != "" ] && \
                    [ "${xModule["makefilepath"]}" != "" ]; then
                    if [ "${mdisDriverSpecList["${xModule["type"]}"]}" != "" ]; then
                        mdisDriverSpecList["${xModule["type"]}"]+=" "
                    fi
                    mdisDriverSpecList["${xModule["type"]}"]+="${xModule["makefilepath"]}"
                fi
            fi
        xModule=()
        fi
    fi
}
