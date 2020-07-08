#/bin/bash
# docker build --tag vice-buildcontainer:0.2 .

if test x"$1" = "x" ; then
    echo "Usage: $0 <vice-checkout-dir>"
    exit 1
fi

base=$1
if [ ! -e ${base}/vice ] ; then
    echo "${base} is not a valid vice checkout directory"
    exit 1
fi
echo "Building here: ${base}"

docker run -a STDOUT -a STDERR -it \
       --name vice-build \
       --mount type=bind,source="${base}",target=/vice \
       vice-buildcontainer:0.2 \
       /build-vice.sh
