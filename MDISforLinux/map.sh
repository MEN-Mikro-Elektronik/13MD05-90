### @brief Create map
### @param $1 Map name
mapNew() {
    local -r keys="__MAP_KEYS_${1}"
    local -r values="__MAP_VALUES_${1}"

    declare -a "${keys}"
    declare -a "${values}"

    eval "${keys}=()"
    eval "${values}=()"
}

### @brief Delete map
### @param $1 Map name
mapDelete() {
    local -r keys="__MAP_KEYS_${1}"
    local -r values="__MAP_VALUES_${1}"

    eval "unset -v \"${keys}\""
    eval "unset -v \"${values}\""
}

### @brief Clear map
### @param $1 Map name
mapClear() {
    local -r keys="__MAP_KEYS_${1}"
    local -r values="__MAP_VALUES_${1}"

    eval "${keys}=()"
    eval "${values}=()"
}

### @brief Get key value
### @param $1 Map name
### @param $2 Key
### @return Value associated with key is echoed
mapGet() {
    local -r keys="__MAP_KEYS_${1}"
    local -r values="__MAP_VALUES_${1}"
    local -ir size="$(eval "echo \${#${keys}[@]}")"
    local -i i
    local key

    for (( i=0 ; i<size ; i++ )); do
        key="$(eval "echo \${${keys}[${i}]}")"
        if [ "${key}" == "${2}" ]; then
            eval "echo \"\${${values}[${i}]}\""
            break
        fi
    done
}

### @brief Associate value with key
### @param $1 Map name
### @param $2 Key
### @param $3 Value
mapPut() {
    local -r keys="__MAP_KEYS_${1}"
    local -r values="__MAP_VALUES_${1}"
    local -ir size="$(eval "echo \${#${keys}[@]}")"
    local -i i
    local key

    if [ "${size}" == 0 ]; then
        eval "${keys}+=(\"$2\")"
        eval "${values}+=(\"$3\")"
    else
        for (( i=0 ; i<size ; i++ )); do
            key="$(eval "echo \${${keys}[${i}]}")"
            if [ "${key}" == "${2}" ]; then
                eval "${values}[${i}]=\"${3}\""
                break
            fi
        done
        if [ "${i}" == "${size}" ]; then
            eval "${keys}+=(\"$2\")"
            eval "${values}+=(\"$3\")"
        fi
    fi
}

### @brief Remove key (and associated value)
### @param $1 Map name
### @param $2 Key
mapRemove() {
    local -r keys="__MAP_KEYS_${1}"
    local -r values="__MAP_VALUES_${1}"
    local -i size
    local -i i
    local key

    size="$(eval "echo \${#${keys}[@]}")"
    for (( i=0 ; i<size ; i++ )); do
        key="$(eval "echo \${${keys}[${i}]}")"
        if [ "${key}" == "${2}" ]; then
            size="$((size-1))"
            if [ "${size}" != "0" ]; then
                eval "${keys}[${i}]=\"\${${keys}[${size}]}\""
                eval "${values}[${i}]=\"\${${values}[${size}]}\""
            fi
            eval "unset -v \"${keys}[${size}]\""
            eval "unset -v \"${values}[${size}]\""
            break
        fi
    done
}

### @brief Get keys
### @param $1 Map name
### @return All keys are echoed
mapKeys() {
    local -r keys="__MAP_KEYS_${1}"

    eval "echo \${${keys}[@]}"
}

### @brief Get values
### @param $1 Map name
### @return All values are echoed
mapValues() {
    local -r values="__MAP_VALUES_${1}"

    eval "echo \${${values}[@]}"
}

### @brief Check if key is in map
### @param $1 Map name
### @param $2 Key
### @return 0 if key is in map
### @return non-zero otherwise
mapKey() {
    local -r keys="__MAP_KEYS_${1}"
    local -ir size="$(eval "echo \${#${keys}[@]}")"
    local -i i
    local key

    for (( i=0 ; i<size ; i++ )); do
        key="$(eval "echo \${${keys}[${i}]}")"
        if [ "${key}" == "${2}" ]; then
            return "0"
        fi
    done

    return "1"
}

### @brief Check if value is in map
### @param $1 Map name
### @param $2 Value
### @return 0 if value is in map
### @return non-zero otherwise
mapValue() {
    local -r values="__MAP_VALUES_${1}"
    local -ir size="$(eval "echo \${#${values}[@]}")"
    local -i i
    local value

    for (( i=0 ; i<size ; i++ )); do
        value="$(eval "echo \${${values}[${i}]}")"
        if [ "${value}" == "${2}" ]; then
            return "0"
        fi
    done

    return "1"
}

### @brief Get size
### @param $1 Map name
### @return Map size is echoed
mapSize() {
    local -r keys="__MAP_KEYS_${1}"

    eval "echo \${#${keys}[@]}"
}
