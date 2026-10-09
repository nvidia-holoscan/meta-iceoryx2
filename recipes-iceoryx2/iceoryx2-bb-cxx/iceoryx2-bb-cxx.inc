SUMMARY = "iceoryx2 BB C++"
DESCRIPTION = "This package contains the iceoryx2 C++ Building Blocks"
HOMEPAGE = "https://iceoryx.io"
BUGTRACKER = "https://github.com/eclipse-iceoryx/iceoryx2/issues"
LICENSE = "Apache-2.0 OR MIT"
LIC_FILES_CHKSUM = "file://LICENSE-APACHE;md5=22a53954e4e0ec258dfce4391e905dac \
                    file://LICENSE-MIT;md5=b377b220f43d747efdec40d69fcaa69d"

inherit cmake ptest

DEPENDS = " \
  iceoryx2-cmake-modules \
  "

DEPENDS += "${@bb.utils.contains('PTEST_ENABLED', '1', 'googletest', '', d)}"

SRC_URI = "git://github.com/eclipse-iceoryx/iceoryx2.git;protocol=https;nobranch=1 \
          file://run-ptest"

S = "${UNPACKDIR}/${BP}/iceoryx2-bb/cxx"


EXTRA_OECMAKE += "-DCMAKE_INSTALL_PREFIX=${D}/${exec_prefix}"
EXTRA_OECMAKE += "${@bb.utils.contains('PTEST_ENABLED', '1', '-DBUILD_TESTING=ON -DUSE_SYSTEM_GTEST=ON', '-DBUILD_TESTING=OFF', d)}"

BBCLASSEXTEND = "native nativesdk"

do_install() {
  cmake --install ${B}
}

do_install_ptest() {
    install -d ${D}/${PTEST_PATH}/tests
    install -m 0755  ${B}/tests/iceoryx2-bb-cxx-tests ${D}/${PTEST_PATH}/tests
}

# NOTE bitbake complains about nobody providing iceoryx2-bb-cxx needed by
#      iceoryx2-bb-cxx-ptest as runtime dependency;
#      since iceoryx2-bb-cxx is a static library
#      we can remove the runtime dependency here:
RDEPENDS:iceoryx2-bb-cxx-ptest:remove = "iceoryx2-bb-cxx"
