#! /bin/bash
#
# this little script tries to fixup some phrases found in the source so they
# will (or will not) show up in the doxygen documentation
#
# /* BUG: ... -> /*! \bug ...
# /* FIXME: ... -> /*! \todo FIXME: ...
# /* TODO: ... -> /*! \todo ...
# /* #... */ (remove, commented out preprocessor stuff)
# /* ---... */ (remove)
# #define ... /* ... */ -> ... /*!< ... */ (autobrief)
# /* ... */ -> /*! ... */ (autobrief, convert comments to qt style if they begin 
#                          at start of line and the line ends right after them)
#
cat $1 | \
    sed -s 's/\* BUG:/\*! \\bug /g' |
    sed -s 's/\* FIXME:/\*! \\todo FIXME:/g' |
    sed -s 's/\* TODO:/\*! \\todo /g' |
    sed -s 's:^/\* #.*\*/$::' |
    sed -s 's:^/\*.---.*\*/$::' |
    sed -s 's:\(^#define .* \)\(/\* \)\(.*\*/$\):\1/\*!< \3:' |
    sed -s 's:^/\* \(.*\) \*/$:/\*! \1 \*/:'

