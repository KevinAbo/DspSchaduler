#!/bin/sh

###########################################################################
#
# Copyright (c) 2014 Avazu, Inc. All Rights Reserved
# $Id: build.sh,v 1.0.0.1 2014/12/01 07:34:57 Dylan Wu Exp $
#
# 这是构建脚本程序
#
##########################################################################/

CUR_DIR=$(cd "$(dirname "${0}")"; pwd)
BUILD_DIR="${CUR_DIR}/.build"
PROJECT=$(basename $CUR_DIR)
PRODUCT_DIR="${CUR_DIR}/product_output"

cd $CUR_DIR

TAG=`export LANG="en_US.UTF-8"; svn info | awk '/^URL/{print $2}' | awk -F'tag/' '{print $2}' | awk -F'/' '{print $NF}' | awk -F'.' '{if(NF>1) print $NF}'`
if [ -z $TAG ]; then
    TAG="unknown"
fi
REVISION=`export LANG="en_US.UTF-8"; svn info | awk '/^Revision/{print $2}'`
if [ -z $REVISION ]; then
    REVISION="unknown"
fi

#安装的路径, 可以根据需要传入不同的安装目录参数
PREFIX="${PRODUCT_DIR}"
if [ -n "$1" ]; then
    if [ "${1#/}" != "${1}" ]; then
        PREFIX="${1}"
    else
        PREFIX="${CUR_DIR}/${1}"
    fi
fi
echo "PRODUCT_DIRECTORY: $PREFIX"

if [ -d $PREFIX ]; then
    echo "Directory[$PREFIX] is exists, confirm overwrite it? [Y/N]"
    read choice
    if [ "$choice" != "y" ] && [ "$choice" != "Y" ]; then
        exit 1
    fi
    rm -rf $PREFIX
fi

#创建build
rm -rf ${BUILD_DIR}
mkdir -p ${BUILD_DIR}

#编译并安装
cd ${BUILD_DIR} && cmake -D PROJECT_TITLE=${PROJECT} -D PREFIX=${PREFIX} -D TAG=${TAG} -D REVISION=${REVISION} .. && make -j4 
if [ $? -ne 0 ]; then
	echo "Build Project ERROR"
    exit 2
fi

# cd $CUR_DIR/unit_test/ && cmake . && make && make test && cd -
# if [ $? -ne 0 ]; then
# 	echo "Unit Test ERROR"
#     exit 2
# fi

make install
baselib_revision='unknown'
if [ -f /opt/baselib/REVISION ]; then
    baselib_revision=`cat /opt/baselib/REVISION`
fi
echo "baselib: ${baselib_revision}" > ${PREFIX}/REVISION
echo "project: ${REVISION}" >> ${PREFIX}/REVISION

#清理临时目录
cd ${CUR_DIR}
rm -rf ${BUILD_DIR}

echo "Build ${PROJECT} complete!!!"
