#!/bin/sh

DEVICE_NAME=CVORG
TRANSFER=/etc/transfer.ref
DRIVER_NAME=cvorg

# Install the cvorg driver specific script

OUTPUT=":"
RUN=""
while getopts hvnD:d:t: o
do	case $o in
	v)	OUTPUT="echo" ;;		# verbose
	n)	RUN=":" ;;			# dry run
	D)	DEVICE_NAME="$OPTARG" ;;
	d)	DRIVER_NAME="$OPTARG" ;;
	t)	TRANSFER="$OPTARG" ;;
	[h?])	echo >&2 "usage: $0 [-?hvnb] [-D device] [-d driver] [-t transfer]"
		exit ;;
	esac
done

$OUTPUT "Installing $DEVICE_NAME driver..."
INSMOD_ARGS=`awk -f cvorg.awk $DEVICE_NAME $TRANSFER`
if [ x"$INSMOD_ARGS" == x"" ] ; then
    echo "No $DEVICE_NAME declared in $TRANSFER, exiting"
    exit 1
fi

INSMOD_CMD="/sbin/insmod $DRIVER_NAME.ko $INSMOD_ARGS"
$OUTPUT installing $DRIVER_NAME by $INSMOD_CMD
sh -c "$RUN $INSMOD_CMD"


MAJOR=`cat /proc/devices | awk '$2 == "'"$DRIVER_NAME"'" {print $1}'`
if [ -z "$MAJOR" ]; then
	echo "driver $DRIVER_NAME not installed!"
	exit 1
fi
MINORS=`awk '/^#\+#/ && $6 == "'"$DEVICE_NAME"'" { printf("%s ", $7) }' $TRANSFER`
$OUTPUT "creating device nodes for driver $DRIVER_NAME, major $MAJOR, minors $MINORS"
for MINOR in $MINORS; do
    NODE=`expr $MINOR + 1`
    sh -c "$RUN rm -f /dev/$DRIVER_NAME.$NODE"
    sh -c "$RUN mknod /dev/$DRIVER_NAME.$NODE c $MAJOR $NODE"
done
