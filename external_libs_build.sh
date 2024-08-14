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
# @file   external_libs_build.sh
# @author Scott Brauer
# @date   11-15-2021
#

. external_libs_common.sh
LOG_FILE=${LOG_DIR}/external_libs_build.log

echo "External Libs Get" | tee ${LOG_FILE}

# INSTALL_DIR=${HOME}/dev/offcenter/trade/external_lib
# INSTALL_DIR_LIB=${INSTALL_DIR}/lib
# EXTERNAL_SRC_DIR=${HOME}/dev/offcenter/trade/external_src
# #LOG_DIR=${EXTERNAL_SRC_DIR}
# BUILD_DIR=${EXTERNAL_SRC_DIR}/build
# BUILD_DIR_JSON=${BUILD_DIR}/json
# BUILD_DIR_CPPRESTSDK=${BUILD_DIR}/cpprestsdk
# BUILD_DIR_EASYLOGGINGPP=${BUILD_DIR}/easyloggingpp
# BUILD_DIR_GOOGLETEST=${BUILD_DIR}/googletest
# BUILD_DIR_DATE=${BUILD_DIR}/date
# BUILD_DIR_APR=${BUILD_DIR}/apr-1.7.0
# BUILD_DIR_APRICONV=${BUILD_DIR}/apr-iconv
# BUILD_DIR_ACTIVEMQCPP=${BUILD_DIR}/activemq-cpp
# BUILD_DIR_MARIADB_C=${BUILD_DIR}/mariadb_c
# BUILD_DIR_MARIADB_CPP=${BUILD_DIR}/mariadb_cpp
# BUILD_DIR_SOCI=${BUILD_DIR}/soci
# BUILD_DIR_OANDA=${BUILD_DIR}/oanda
# BUILD_DIR_OFFCENTERCOMMON=${BUILD_DIR}/offcenter_common
# BUILD_DIR_OFFCENTERTRADING=${BUILD_DIR}/offcenter_trading
# BUILD_DIR_INTELFPMATH=${EXTERNAL_SRC_DIR}/build/intel_fp_math
# BUILD_DIR_GMP=${EXTERNAL_SRC_DIR}/build/gmp
# SOURCE_DIR_GMP=${EXTERNAL_SRC_DIR}/gmp-6.2.1
# SOURCE_DIR_MPDEC=${EXTERNAL_SRC_DIR}/mpdecimal-2.5.1
# BUILD_DIR_BLOOMBERG_BDE=${BUILD_DIR}/bloomberg_bde
# SOURCE_DIR_BLOOMBERG_BDE_TOOLS=${EXTERNAL_SRC_DIR}/bde-tools
# SOURCE_DIR_BLOOMBERG_BDE=${EXTERNAL_SRC_DIR}/bde
# SOURCE_DIR_BOOST=${EXTERNAL_SRC_DIR}/boost_1_76_0
# SOURCE_DIR_ONETBB=${EXTERNAL_SRC_DIR}/oneTBB
# BUILD_DIR_ONETBB=${BUILD_DIR}/oneTBB

install_dependencies=0
build_boost=1
build_json=1
build_mariadb_cpp=0
setup_mariadb=0
build_soci=0
build_intel_rd_fp_math=0
build_gmp=0
build_mpdec=0
build_bloomberg_bde=0

function build_cmake {
    local build_dir=$1
    local src_dir=$2
 	local lib=$3
	local log_file=${LOG_DIR}/${lib}.log

	echo "Building ${lib} - Log File(${log_file})" | tee -a ${LOG_FILE}
	mkdir -p ${build_dir}
	pushd ${build_dir}
	cmake -G"Unix Makefiles" ${src_dir} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} | tee ${log_file}
	make -j5 install | tee -a ${log_file}
	popd
}


# Create build directories
if ((install_dependencies)); then
	sudo apt-get install zlib1g zlib1g-dev g++ git libwebsocketpp-dev openssl libssl-dev ninja-build
	sudo apt-get install libcppunit-dev
	sudo apt-get install libapr1 libapr1-dev
	sudo apt-get install libgl1-mesa-dev
	#sudo apt-get install libboost-all-dev
	#sudo apt-get install libboost-atomic-dev libboost-thread-dev libboost-system-dev libboost-date-time-dev libboost-regex-dev libboost-filesystem-dev libboost-random-dev libboost-chrono-dev libboost-serialization-dev libboost-multiprecision-dev libboost-iostreams-dev libboost-program-options-dev
	#libboost-interprocess-dev
fi

###############################################################################
# boost 
if ((build_boost)); then
	LOG_FILE_BOOST=${LOG_DIR}/boost.log
	echo "Building Boost - Log File(${LOG_FILE_BOOST})" | tee -a ${LOG_FILE}
	pushd ${SOURCE_DIR_BOOST}
	./bootstrap.sh --prefix=${INSTALL_DIR} 2>&1 | tee ${LOG_FILE_BOOST}
	./b2 install 2>&1 | tee -a ${LOG_FILE_BOOST}
	popd
else
	echo "Skipping Build of Boost" | tee -a ${LOG_FILE}
fi

###############################################################################
# json
if ((build_json)); then
	build_cmake ${BUILD_DIR_JSON} ${SOURCE_DIR_JSON} "json"
	# LOG_FILE_JSON=${LOG_DIR}/json.log
	# echo "Building JSON - Log File(${LOG_FILE_JSON})" | tee -a ${LOG_FILE}
	# mkdir -p ${BUILD_DIR_JSON}
	# pushd ${BUILD_DIR_JSON}
	# cmake -G"Unix Makefiles" ${SOURCE_DIR_JSON} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} | tee ${LOG_FILE_JSON}
	# make -j5 install | tee -a ${LOG_FILE_JSON}
	# popd
else
	echo "Skipping Build of json" | tee -a ${LOG_FILE}
fi

###############################################################################
# cpprestsdk
#mkdir -p ${BUILD_DIR_CPPRESTSDK}
#pushd ${BUILD_DIR_CPPRESTSDK}
#cmake -G"Unix Makefiles" ../../cpprestsdk -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=0 -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
#make -j5 install
#popd

###############################################################################
# easylogging++
#mkdir -p ${BUILD_DIR_EASYLOGGINGPP}
#pushd ${BUILD_DIR_EASYLOGGINGPP}
#cmake -G"Unix Makefiles" ../../easyloggingpp -Dtest=OFF -Dbuild_static_lib=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
#make -j5 install
#popd

###############################################################################
# googletest
#mkdir -p ${BUILD_DIR_GOOGLETEST}
#pushd ${BUILD_DIR_GOOGLETEST}
#cmake -G"Unix Makefiles" ../../googletest -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
#make -j5 install
#popd

###############################################################################
# date
##LOG_FILE=${LOG_DIR}/date.log
##mkdir -p ${BUILD_DIR_DATE} 2>&1 | tee ${LOG_FILE}
##pushd ${BUILD_DIR_DATE}
##cmake -G"Unix Makefiles" ${EXTERNAL_SRC_DIR}/date -DCMAKE_BUILD_TYPE=Release -DBUILD_TZ_LIB=ON -DENABLE_DATE_TESTING=ON -DUSE_TZ_DB_IN_DOT=ON -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} 2>&1 | tee -a ${LOG_FILE}
##make -j5 install 2>&1 | tee -a ${LOG_FILE}
##popd

###############################################################################
# apr
## Installed from libraries
##LOG_FILE=${LOG_DIR}/apr.log
#mkdir -p ${BUILD_DIR_APR} 2>&1 | tee ${LOG_FILE}
##pushd apr-1.7.0
##./configure --prefix=${INSTALL_DIR} 2>&1 | tee -a ${LOG_FILE}
#cmake -G"Unix Makefiles" ../../apr -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
##make -j4 2>&1 | tee -a ${LOG_FILE}
##make -j4 install 2>&1 | tee -a ${LOG_FILE}
##popd

###############################################################################
# oanda
#mkdir -p ${BUILD_DIR_OANDA}
#pushd ${BUILD_DIR_OANDA}
#cmake -G"Unix Makefiles" ../../oanda -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
#make -j5 install
#popd

###############################################################################
# activemq-cpp
##LOG_FILE=${LOG_DIR}/activemq_cpp.log
###mkdir -p ${BUILD_DIR_ACTIVEMQCPP} 2>&1 | tee ${LOG_FILE}
##pushd activemq-cpp/activemq-cpp
##CFLAGS="-I${INSTALL_DIR}/include" CXXFLAGS="-I${INSTALL_DIR}/include" LDFLAGS="-L${INSTALL_DIR}/lib" ./autogen.sh 2>&1 | tee -a ${LOG_FILE}
##CFLAGS="-I${INSTALL_DIR}/include" CXXFLAGS="-I${INSTALL_DIR}/include" LDFLAGS="-L${INSTALL_DIR}/lib" ./configure --prefix=${INSTALL_DIR} 2>&1 | tee -a ${LOG_FILE}
###cmake -G"Unix Makefiles" ../../activemq-cpp -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
##make -j5 2>&1 | tee -a ${LOG_FILE}
##make -j5 install 2>&1 | tee -a ${LOG_FILE}
##popd

###############################################################################
# mariadb_c
##LOG_FILE=${LOG_DIR}/mariadb_c.log
##mkdir -p ${BUILD_DIR_MARIADB_C} 2>&1 | tee ${LOG_FILE}
##pushd ${BUILD_DIR_MARIADB_C}
###CFLAGS="-I${INSTALL_DIR}/include" CXXFLAGS="-I${INSTALL_DIR}/include" LDFLAGS="-L${INSTALL_DIR}/lib" ./autogen.sh
###CFLAGS="-I${INSTALL_DIR}/include" CXXFLAGS="-I${INSTALL_DIR}/include" LDFLAGS="-L${INSTALL_DIR}/lib" ./configure --prefix=${INSTALL_DIR}
### -DMYSQL_LIBRARIES=/home/sbrauer/dev/offcenter/trade/external_lib/lib/mariadb/libmariadbclient.a -DMYSQL_DIR=${INSTALL_DIR}
##make -j4 clean 2>&1 | tee -a ${LOG_FILE}
##cmake -G"Unix Makefiles" ../../mariadb-connector-c -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCONC_WITH_UNIT_TESTS=Off -DWITH_SSL=ON -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} 2>&1 | tee -a ${LOG_FILE}
##cmake --build . --config RelWithDebInfo 2>&1 | tee -a ${LOG_FILE}
##make -j4 install 2>&1 | tee -a ${LOG_FILE}
##popd

###############################################################################
# mariadb_cpp
# if ((build_mariadb_cpp)); then
# 	LOG_FILE=${LOG_DIR}/mariadb_cpp.log
# 	mkdir -p ${BUILD_DIR_MARIADB_CPP} 2>&1 | tee ${LOG_FILE}
# 	pushd ${BUILD_DIR_MARIADB_CPP}
# 	###CFLAGS="-I${INSTALL_DIR}/include" CXXFLAGS="-I${INSTALL_DIR}/include" LDFLAGS="-L${INSTALL_DIR}/lib" ./autogen.sh
# 	###CFLAGS="-I${INSTALL_DIR}/include" CXXFLAGS="-I${INSTALL_DIR}/include" LDFLAGS="-L${INSTALL_DIR}/lib" ./configure --prefix=${INSTALL_DIR}
# 	### -DMYSQL_LIBRARIES=/home/sbrauer/dev/offcenter/trade/external_lib/lib/mariadb/libmariadbclient.a -DMYSQL_DIR=${INSTALL_DIR}
# 	make -j4 clean 2>&1 | tee -a ${LOG_FILE}
# 	cmake -G"Unix Makefiles" ../../mariadb-connector-cpp -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCONC_WITH_UNIT_TESTS=Off -DWITH_SSL=ON -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} 2>&1 | tee -a ${LOG_FILE}
# 	cmake --build . --config RelWithDebInfo 2>&1 | tee -a ${LOG_FILE}
# 	make -j4 install 2>&1 | tee -a ${LOG_FILE}
# 	popd
# fi

# if ((setup_mariadb)); then
# 	pushd ${INSTALL_DIR_LIB} 
# 	ln -s mariadb/libmariadb.so.3 libmysqlclient.so
# 	ln -s mariadb/libmariadbclient.a libmysqlclient.a
# 	ln -s libmysqlclient.so libmariadb.so.3
# 	popd
# fi

###############################################################################
# soci
# if ((build_soci)); then
# 	LOG_FILE=${LOG_DIR}/soci.log
# 	mkdir -p ${BUILD_DIR_SOCI} 2>&1 | tee ${LOG_FILE}
# 	pushd ${BUILD_DIR_SOCI}
# 	###CFLAGS="-I${INSTALL_DIR}/include" CXXFLAGS="-I${INSTALL_DIR}/include" LDFLAGS="-L${INSTALL_DIR}/lib" ./autogen.sh
# 	###CFLAGS="-I${INSTALL_DIR}/include" CXXFLAGS="-I${INSTALL_DIR}/include" LDFLAGS="-L${INSTALL_DIR}/lib" ./configure --prefix=${INSTALL_DIR}
# 	### -DMYSQL_LIBRARIES=/home/sbrauer/dev/offcenter/trade/external_lib/lib/mariadb/libmariadbclient.a -DMYSQL_DIR=${INSTALL_DIR}
# 	cmake -G"Unix Makefiles" ../../soci -DSOCI_CXX11=ON -DSOCI_SHARED=OFF -DSOCI_STATIC=ON -DSOCI_TESTS=ON -DWITH_BOOST=ON -DWITH_MYSQL=ON -DMYSQL_INCLUDE_DIR=${INSTALL_DIR}/include/mariadb -DMYSQL_DIR=${INSTALL_DIR} -DWITH_DB2=OFF -DWITH_FIREBIRD=OFF -DWITH_ODBC=OFF -DWITH_ORACLE=OFF -DWITH_POSTGRESQL=OFF -DWITH_SQLITE3=OFF  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} 2>&1 | tee -a ${LOG_FILE}
# 	make -j4 2>&1 | tee -a ${LOG_FILE}
# 	make -j4 install 2>&1 | tee -a ${LOG_FILE}
# 	popd
# fi


###############################################################################
# oanda
# mkdir -p ${BUILD_DIR_OANDA}
# pushd ${BUILD_DIR_OANDA}
# #OANDA_SRC_DIR=../../oanda
# OANDA_SRC_DIR=${HOME}/dev/offcenter/trade/brokers/oanda
# cmake -G"Unix Makefiles" ${OANDA_SRC_DIR} -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${INSTALL_DIR}/lib/cmake/nlohmann_json;${INSTALL_DIR}/lib/cmake/cpprestsdk;${EXTERNAL_SRC_DIR}/easyloggingpp/cmake" -DEASYLOGGINGPP_ROOT=${INSTALL_DIR} -DEASYLOGGINGPP_USE_STATIC_LIBS=ON -DGTEST_ROOT=${INSTALL_DIR} -DLOG_V20_ACCESS=ON -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
# make -j4 install
# popd

###############################################################################
# offcenter_common
# OFFCENTER_COMMON_DIR=${HOME}/dev/offcenter/offcenter_common
# #OFFCENTER_COMMON_DIR=../../offcenter_common
# mkdir -p ${BUILD_DIR_OFFCENTERCOMMON}
# pushd ${BUILD_DIR_OFFCENTERCOMMON}
# #cmake -G"Unix Makefiles" ${OFFCENTER_COMMON_DIR} -DCMAKE_BUILD_TYPE=Release -DAPR_ALTLOCATION=${INSTALL_DIR} -DCMAKE_MODULE_PATH=${INSTALL_DIR} -DACTIVEMQCPP_ALTLOCATION=${INSTALL_DIR} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
# cmake -G"Unix Makefiles" ${OFFCENTER_COMMON_DIR} -DGTEST_ROOT=${INSTALL_DIR} -DENABLE_UNIT_TESTS=ON -DCMAKE_BUILD_TYPE=Release -DAPR_ALTLOCATION=${INSTALL_DIR} -DACTIVEMQCPP_ALTLOCATION=${INSTALL_DIR} -DCMAKE_MODULE_PATH:PATH=${INSTALL_DIR};${INSTALL_DIR}/cmake -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR};${INSTALL_DIR}/easyloggingpp/cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
# cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON -DAPR_ALTLOCATION:STRING=/home/sbrauer/dev/offcenter/trade/external_lib -DENABLE_CODE_ANALYSIS:STRING=OFF -DBUILD_DOC:STRING=OFF -DGTEST_ROOT:STRING=/home/sbrauer/dev/offcenter/trade/external_lib -DENABLE_UNIT_TESTS:STRING=ON -DACTIVEMQCPP_ALTLOCATION:STRING=/home/sbrauer/dev/offcenter/trade/external_lib -DCMAKE_MODULE_PATH:PATH=/home/sbrauer/dev/offcenter/trade/external_lib;/home/sbrauer/dev/offcenter/trade/external_lib/cmake -DCMAKE_PREFIX_PATH:PATH=/home/sbrauer/dev/offcenter/trade/external_lib;/home/sbrauer/dev/offcenter/trade/external_src/easyloggingpp/cmake -DBUILD_DOC:STRING=ON -DEASYLOGGINGPP_ROOT:STRING=/home/sbrauer/dev/offcenter/trade/external_lib -DEASYLOGGINGPP_USE_STATIC_LIBS:STRING=ON -DMYSQLCLIENT_STATIC_BINDING:BOOL=1 -DCMAKE_INSTALL_PREFIX:STRING=/home/sbrauer/dev/offcenter/trade/external_lib -G "Unix Makefiles" /home/sbrauer/dev/offcenter/offcenter_common 
# make -j4 install
# popd

###############################################################################
# intel RD FP Math Lib
# if ((build_intel_rd_fp_math)); then
# 	LOG_FILE=${LOG_DIR}/intel_rd_fp_math.log
# 	mkdir -p ${BUILD_DIR_INTELFPMATH} 2>&1 | tee ${LOG_FILE}
# 	cp ${EXTERNAL_SRC_DIR}/IntelRDFPMathLib20U2/LIBRARY/.directory_exists ${BUILD_DIR_INTELFPMATH} 2>&1 | tee -a ${LOG_FILE}
# 	pushd ${EXTERNAL_SRC_DIR}/IntelRDFPMathLib20U2/LIBRARY
# 	# #cmake -G"Unix Makefiles" ${HOME}/dev/offcenter/trade/trading -DCMAKE_BUILD_TYPE=Release -DGTEST_ROOT=${INSTALL_DIR} -DENABLE_UNIT_TESTS=ON -DAPR_ALTLOCATION=${INSTALL_DIR} -DACTIVEMQCPP_ALTLOCATION=${INSTALL_DIR} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
# 	# cmake -G"Unix Makefiles" ${HOME}/dev/offcenter/trade/trading -DCMAKE_BUILD_TYPE=Release -DGTEST_ROOT=${INSTALL_DIR} -DENABLE_UNIT_TESTS=ON -DAPR_ALTLOCATION=${INSTALL_DIR} -DACTIVEMQCPP_ALTLOCATION=${INSTALL_DIR} -DCMAKE_PREFIX_PATH="${INSTALL_DIR}/lib/cmake/nlohmann_json;${INSTALL_DIR}/lib/cmake/cpprestsdk;${EXTERNAL_SRC_DIR}/easyloggingpp/cmake" -DEASYLOGGINGPP_ROOT=${INSTALL_DIR} -DEASYLOGGINGPP_USE_STATIC_LIBS=ON -DLOG_V20_ACCESS=ON -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
# 	export LIB_DIR=${BUILD_DIR_INTELFPMATH} 2>&1 | tee -a ${LOG_FILE}
# 	./linuxbuild 2>&1 | tee -a ${LOG_FILE}
# 	popd
# fi

###############################################################################
# gmp 
# if ((build_gmp)); then
#         LOG_FILE=${LOG_DIR}/gmp.log
#         #mkdir -p ${BUILD_DIR_GMP} 2>&1 | tee ${LOG_FILE}
#         pushd ${SOURCE_DIR_GMP}
# 	echo "--- Configure ---"
# 	./configure --prefix=${INSTALL_DIR} 2>&1 | tee ${LOG_FILE}
# 	echo "--- Make ---"
# 	make 2>&1 | tee ${LOG_FILE}
# 	echo "--- Make Check ---"
# 	make check 2>&1 | tee ${LOG_FILE}
# 	echo "--- Make Install ---"
# 	make install 2>&1 | tee ${LOG_FILE}
#         popd
# fi

###############################################################################
#  mpdec
# if ((build_mpdec)); then
#         LOG_FILE=${LOG_DIR}/mpdec.log
#         #mkdir -p ${BUILD_DIR_MPDEC} 2>&1 | tee ${LOG_FILE}
#         pushd ${SOURCE_DIR_MPDEC}
#         echo "--- Configure ---"
#         ./configure --prefix=${INSTALL_DIR} 2>&1 | tee ${LOG_FILE}
#         echo "--- Make ---"
#         make 2>&1 | tee -a ${LOG_FILE}
#         echo "--- Make Check ---"
#         make check 2>&1 | tee -a ${LOG_FILE}
#         echo "--- Make Install ---"
#         make install 2>&1 | tee -a ${LOG_FILE}
#         popd
# fi

###############################################################################
# bloomberg_bde
# if ((build_bloomberg_bde)); then
#         LOG_FILE=${LOG_DIR}/bloomberg_bde.log
#         mkdir -p ${BUILD_DIR_BLOOMBERG_BDE} 2>&1 | tee ${LOG_FILE}
# 	export PATH=${SOURCE_DIR_BLOOMBERG_BDE_TOOLS}/bin:$PATH
# 	pushd ${SOURCE_DIR_BLOOMBERG_BDE}
# 	export BDE_CMAKE_BUILD_DIR=${BUILD_DIR_BLOOMBERG_BDE}

# 	#echo "---"
# 	waf configure 2>&1 | tee -a ${LOG_FILE}
# 	#bde_build_env.py --help  2>&1 | tee -a ${LOG_FILE}
# 	echo "---"
#         bde_build_env.py list 2>&1 | tee -a ${LOG_FILE}
# 	echo "---"
# 	bde_build_env.py --build-type=Release --cpp-std=17 --abi-bits=64 --library-type=static --install-dir=${INSTALL_DIR} --build-dir=${BUILD_DIR_BLOOMBERG_BDE} 2>&1 | tee -a ${LOG_FILE}
# 	echo "---"
# 	##bde_build_env.py set -i ${INSTALL_DIR} -b ${BUILD_DIR_BLOOMBERG_BDE} 2>&1 | tee -a ${LOG_FILE}

# 	export BDE_CMAKE_UPLID=unix-linux-armv8-5.4.0-gcc-9.3.0
# 	export BDE_CMAKE_UFID=opt_exc_mt_64_cpp17
# 	export BDE_CMAKE_BUILD_DIR="/home/sbrauer/dev/offcenter/trade/external_src/build/bloomberg_bde"
# 	export CXX=/usr/bin/g++
# 	export CC=/usr/bin/gcc
# 	export BDE_CMAKE_TOOLCHAIN=toolchains/linux/gcc-default
# 	export BDE_CMAKE_INSTALL_DIR="/home/sbrauer/dev/offcenter/trade/external_lib"

# 	cmake_build.py --help 2>&1 | tee -a ${LOG_FILE}

#        	cmake_build.py configure --build_dir ${BUILD_DIR_BLOOMBERG_BDE} --prefix ${INSTALL_DIR} -G "Unix Makefiles" --install_dir ${INSTALL_DIR} 2>&1 | tee -a ${LOG_FILE}
#         #cmake_build.py build --test build 2>&1 | tee -a ${LOG_FILE}
#         #cmake_build.py build --test run 2>&1 | tee -a ${LOG_FILE}
#         #cmake_build.py --install_dir=${INSTALL_DIR} --prefix=/ install 2>&1 | tee -a ${LOG_FILE}

# 	#bde_build_env.py --build-type=Release --cpp-std=17 2>&1 | tee -a ${LOG_FILE}
# 	#cmake_build.py configure 2>&1 | tee -a ${LOG_FILE}
# 	#cmake_build.py build --test build 2>&1 | tee -a ${LOG_FILE}
# 	#cmake_build.py build --test run 2>&1 | tee -a ${LOG_FILE}
# 	#cmake_build.py --install_dir=${INSTALL_DIR} --prefix=/ install 2>&1 | tee -a ${LOG_FILE}
#         popd
# fi



###############################################################################
# oneTBB
# mkdir -p ${BUILD_DIR_ONETBB}
# pushd ${BUILD_DIR_ONETBB}
# cmake -G"Unix Makefiles" ${SOURCE_DIR_ONETBB} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
# make -j5 install
# popd
