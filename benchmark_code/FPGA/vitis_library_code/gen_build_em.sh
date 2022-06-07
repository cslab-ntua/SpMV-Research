usage(){ echo "Usage : $0 -l <Xilinx Tools License Path> -v <Vitis Tools Path> -x <XRT Path> -p <Xilinx Platforms Path> -d <Target Device>"; exit 1; }

if [ "$#" -ne 10 ]; then
	usage
fi

while getopts l:v:x:p:d: flag; do
	case "${flag}" in
		l) XILINX_LIC=${OPTARG};;
		v) VITIS_PATH=${OPTARG};;
		x) XRT_PATH=${OPTARG};;
		p) XIL_PLATFORMS=${OPTARG};;
		d) XIL_DEVICE=${OPTARG};;
		:) # If expected argument omitted:
			echo "Error: -${OPTARG} requires an argument."
			usage;;
		*) # If unknown (any other) option:
			usage;;
	esac
done

export BUILD_FILE="build_em.sh"
rm -rf ${BUILD_FILE}
echo 'export LC_ALL=en_US.utf8' >> ${BUILD_FILE}
echo 'export XILINXD_LICENSE_FILE='$XILINX_LIC >> ${BUILD_FILE}
echo 'source '$VITIS_PATH'/settings64.sh' >> ${BUILD_FILE}
echo 'source '$XRT_PATH'/setup.sh' >> ${BUILD_FILE}
echo 'make all TARGET=hw PLATFORM_REPO_PATHS='${XIL_PLATFORMS}' DEVICE='${XIL_DEVICE} >> ${BUILD_FILE}
