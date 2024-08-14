#
# Copyright 2021 Scott Brauer
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an  BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#
#
# @file   external_libs_common.sh
# @author Scott Brauer
# @date   11-15-2021
#

CURRENT_DIR=${PWD}


EXTERNAL_SRC_DIR=${CURRENT_DIR}/../offcenter_external_src
EXTERNAL_LIB_DIR=${CURRENT_DIR}/../offcenter_external_lib
EXTERNAL_BLD_DIR=${CURRENT_DIR}/../offcenter_external_bld
LOG_DIR=${CURRENT_DIR}/../external_log

INSTALL_DIR=${EXTERNAL_LIB_DIR}
INSTALL_DIR_LIB=${INSTALL_DIR}/lib

mkdir -p ${EXTERNAL_SRC_DIR}
mkdir -p ${EXTERNAL_LIB_DIR}
mkdir -p ${EXTERNAL_BLD_DIR}
mkdir -p ${LOG_DIR}

SOURCE_DIR_BOOST=${EXTERNAL_SRC_DIR}/boost_1_76_0

SOURCE_DIR_JSON=${EXTERNAL_SRC_DIR}/json
BUILD_DIR_JSON=${EXTERNAL_BLD_DIR}/json

BUILD_DIR_CPPRESTSDK=${BUILD_DIR}/cpprestsdk
BUILD_DIR_EASYLOGGINGPP=${BUILD_DIR}/easyloggingpp
BUILD_DIR_GOOGLETEST=${BUILD_DIR}/googletest
BUILD_DIR_DATE=${BUILD_DIR}/date
BUILD_DIR_APR=${BUILD_DIR}/apr-1.7.0
BUILD_DIR_APRICONV=${BUILD_DIR}/apr-iconv
BUILD_DIR_ACTIVEMQCPP=${BUILD_DIR}/activemq-cpp
BUILD_DIR_MARIADB_C=${BUILD_DIR}/mariadb_c
BUILD_DIR_MARIADB_CPP=${BUILD_DIR}/mariadb_cpp
BUILD_DIR_SOCI=${BUILD_DIR}/soci
BUILD_DIR_OANDA=${BUILD_DIR}/oanda
BUILD_DIR_OFFCENTERCOMMON=${BUILD_DIR}/offcenter_common
BUILD_DIR_OFFCENTERTRADING=${BUILD_DIR}/offcenter_trading
BUILD_DIR_INTELFPMATH=${EXTERNAL_SRC_DIR}/build/intel_fp_math
BUILD_DIR_GMP=${EXTERNAL_SRC_DIR}/build/gmp
SOURCE_DIR_GMP=${EXTERNAL_SRC_DIR}/gmp-6.2.1
SOURCE_DIR_MPDEC=${EXTERNAL_SRC_DIR}/mpdecimal-2.5.1
BUILD_DIR_BLOOMBERG_BDE=${BUILD_DIR}/bloomberg_bde
SOURCE_DIR_BLOOMBERG_BDE_TOOLS=${EXTERNAL_SRC_DIR}/bde-tools
SOURCE_DIR_BLOOMBERG_BDE=${EXTERNAL_SRC_DIR}/bde
SOURCE_DIR_ONETBB=${EXTERNAL_SRC_DIR}/oneTBB
BUILD_DIR_ONETBB=${BUILD_DIR}/oneTBB
