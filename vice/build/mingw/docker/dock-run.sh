#/bin/bash
# in case you need to rebuild the container image use: 
#   docker build --tag vice-buildcontainer:0.2 .

if test x"$1" = "x" ; then
    echo "Usage: $0 [-i] <vice-checkout-dir>"
    echo "	-i ... start interactive, don't build"
    exit 1
fi

while [ x"$1" != "x" ] ; do
    case $1 in
	-i)
	    cmd=/bin/bash
	    echo "Going interactive, build script is /build-vice.sh"
	    ;;
	-*)
	    echo "Ignoring invalid option $1"
	    ;;
	*)
	    base=$1
	    test x"${cmd}" == "x" && cmd=/build-vice.sh
	    msg="Building here: ${base}"
	    ;;
    esac
    shift
done

# check sanity of checkout tree 
if [ ! -e ${base}/vice/configure ] ; then
    echo "${base} is not a valid vice checkout directory"
    exit 1
fi
echo $msg
    
docker run -a STDOUT -a STDERR -it \
       --env HOME='/tmp' \
       --name vice-build \
       --mount type=bind,source="${base}",target=/vice \
       vice-buildcontainer:0.2 \
       ${cmd}

#       --user `id -u`:`id -g` \

docker rm vice-build
