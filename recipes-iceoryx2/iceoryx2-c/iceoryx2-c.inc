SUMMARY = "iceoryx2 C bindings"
DESCRIPTION = "This package contains the iceoryx2 C bindings"
HOMEPAGE = "https://iceoryx.io"
BUGTRACKER = "https://github.com/eclipse-iceoryx/iceoryx2/issues"
LICENSE = "Apache-2.0 OR MIT"
LIC_FILES_CHKSUM = "file://LICENSE-APACHE;md5=22a53954e4e0ec258dfce4391e905dac \
                    file://LICENSE-MIT;md5=b377b220f43d747efdec40d69fcaa69d"

inherit cmake

DEPENDS = "iceoryx2 iceoryx2-cmake-modules"

SRC_URI = "git://github.com/eclipse-iceoryx/iceoryx2.git;protocol=https;nobranch=1"

S = "${UNPACKDIR}/${BP}/iceoryx2-c"

INSANE_SKIP:${PN} += "already-stripped"
FILES_SOLIBSDEV = ""

# Staged by iceoryx2, see IOX2_FFI_C_DIR there
EXTRA_OECMAKE += "-DRUST_BUILD_ARTIFACT_PATH=${STAGING_DATADIR}/iceoryx2/ffi-c"
EXTRA_OECMAKE += "-DCMAKE_INSTALL_PREFIX=${D}/${exec_prefix}"

FILES:${PN}-staticdev += "${libdir}/libiceoryx2_ffi_c.a"
FILES:${PN} += "${libdir}/libiceoryx2_ffi_c.so"
RDEPENDS:${PN}-dev += "${PN}-staticdev"
BBCLASSEXTEND = "native nativesdk"

do_install() {
  cmake --install ${B}
}
