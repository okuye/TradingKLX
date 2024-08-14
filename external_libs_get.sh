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
# @file   external_libs_get.sh
# @author Scott Brauer
# @date   11-15-2021
#

. external_libs_common.sh
LOG_FILE=${LOG_DIR}/external_libs_get.log

echo "External Libs Get" | tee ${LOG_FILE}

function git_get {
    local dir_name=$1
    local url=$2
    local tag=$3
    local branch=$4

    echo "" | tee -a ${LOG_FILE}
    echo "----------------------------------------" | tee -a ${LOG_FILE}
    if [ -d "${dir_name}" ]; then
        pushd "${dir_name}"

        echo "Pulling ${url} into ${PWD}" | tee -a ${LOG_FILE}
        git pull origin ${branch} | tee -a ${LOG_FILE}

        echo "  Updating submodules" | tee -a ${LOG_FILE}
        git submodule update --recursive --remote | tee -a ${LOG_FILE}

        echo "  Checkout: ${tag}" | tee -a ${LOG_FILE}
        git checkout ${tag} | tee -a ${LOG_FILE}

        popd
    else
        echo "Cloning ${url} into ${PWD}" | tee -a ${LOG_FILE}
        git clone --recurse-submodules "${url}" | tee -a ${LOG_FILE}

        pushd "${dir_name}"

        echo "  Checkout: ${tag}" | tee -a ${LOG_FILE}
        git checkout ${tag} | tee -a ${LOG_FILE}

        popd
    fi
}

pushd ${EXTERNAL_SRC_DIR}

git_get json https://github.com/nlohmann/json.git "tags/v3.9.1" "master"
git_get cpprestsdk https://github.com/microsoft/cpprestsdk.git "tags/v2.10.16" "master"
git_get easyloggingpp https://github.com/amrayn/easyloggingpp.git "tags/v9.97.0" "master"
git_get googletest https://github.com/google/googletest.git "tags/release-1.10.0" "master"
git_get date https://github.com/HowardHinnant/date.git "tags/v3.0.0" "master"

git_get apr https://github.com/apache/apr.git "tags/1.7.0" "master"
git_get apr-iconv https://github.com/apache/apr-iconv.git "tags/1.2.2" "master"
#wget -N https://mirror.jframeworks.com/apache//apr/apr-1.7.0.tar.gz
#tar -zxvf apr-1.7.0.tar.gz

git_get activemq-cpp https://gitbox.apache.org/repos/asf/activemq-cpp.git "tags/activemq-cpp-3.9.5" "master"
git_get mariadb-connector-c https://github.com/mariadb-corporation/mariadb-connector-c.git "tags/v3.2.3" "master"
git_get mariadb-connector-cpp https://github.com/mariadb-corporation/mariadb-connector-cpp.git
#"tags/1.0.1"
git_get soci https://github.com/SOCI/soci.git "tags/v4.0.1" "master"

#wget -N https://registrationcenter-download.intel.com/akdlm/irc_nas/17378/l_tbb_oneapi_p_2021.1.1.119_offline.sh
#chmod +x l_tbb_oneapi*

#wget -N http://www.netlib.org/misc/intel/IntelRDFPMathLib20U2.tar.gz
#tar -zxvf IntelRDFPMathLib20U2.tar.gz

#wget -N https://gmplib.org/download/gmp/gmp-6.2.1.tar.xz
#tar -xvf gmp-6.2.1.tar.xz

#wget -N https://www.bytereef.org/software/mpdecimal/releases/mpdecimal-2.5.1.tar.gz
#tar -zxvf mpdecimal-2.5.1.tar.gz

wget -N https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz
tar -zxvf boost_1_76_0.tar.gz

#git_get bde-tools https://github.com/bloomberg/bde-tools.git
#git_get bde https://github.com/bloomberg/bde.git

popd