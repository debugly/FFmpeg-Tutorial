#! /bin/sh
# 运行此脚本将会编译 tutorial01.c ，目标可执行程序 tutorial01.out
# 可以使用 gcc 也可以使用 clang 编译

# CC="/usr/bin/clang -arch x86_64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk -std=gnu11 -O0"
CC="gcc -g" #-v

BD=$(cd `dirname $0`; pwd)
vendor=$(cd "${BD}/../../vendor"; pwd)
source=$(cd "${BD}/../source"; pwd)
Ccode=$(cd "${BD}/../../Ccode"; pwd)
assets=$(cd "${BD}/../../assets"; pwd)
build="${Ccode}/build/tutorial01"
mkdir -p "$build"

# echo "vendor:${vendor}"
# echo "source:${source}"
# echo "build:${build}"

CFLAGS="-I${vendor}/FFmpeg-3.1.11/include" #/usr/local/include
LDFLAGS="-L${vendor}/FFmpeg-3.1.11/lib" #/usr/local/lib

src="${source}/tutorial01.c"
target="${build}/tutorial01.out"


function run_it(){
    $target "${assets}/xp5.mp4"
}

function build_it(){

    cd "$build"
    if [[ `ls $*` ]];then
        rm -rf *
    fi
    
    $CC "$src" \
        -o "$target" \
        ${CFLAGS} \
        ${LDFLAGS} \
        -lbz2 -liconv.2 -llzma -lz.1 \
        -framework CoreFoundation -framework CoreVideo -framework VideoToolbox -framework CoreMedia -framework AudioToolbox -framework VideoDecodeAcceleration -framework Security \
        -lavfilter -lavutil -lavformat -lswscale -lavdevice -lswresample -lavcodec
}

if [ "$*" = 'run' ];then
    echo "run"
    run_it
else
    build_it
fi


