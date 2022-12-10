#!/usr/bin/env -S gawk -f

BEGIN {
    core_team = 0
    ex_team = 0
    first = 1
}

/core_team\[\]/ {
    core_team = 1
}

/ex_team\[\]/ {
    ex_team = 1
}

/^\};/ {
    core_team = 0
    ex_team = 0
}

/\s*\{\s*"/ {
    if (core_team || ex_team) {
        match($0, /.*"(.*)",.*"(.*)".*"(.*)"/, m)
        if (first) {
            printf "Copyright: %s %s\n", m[1], m[2]
            first = 0
        } else {
            printf "           %s %s\n", m[1], m[2]
        }
    }
}

END {
}
