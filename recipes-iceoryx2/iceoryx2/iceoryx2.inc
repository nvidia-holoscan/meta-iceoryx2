SUMMARY = "iceoryx2"
DESCRIPTION = "This package is used to build iceoryx2-cli and iceoryx2-ffi-c"
HOMEPAGE = "https://iceoryx.io"
BUGTRACKER = "https://github.com/eclipse-iceoryx/iceoryx2/issues"
LICENSE = "Apache-2.0 | MIT"
LIC_FILES_CHKSUM = "file://LICENSE-APACHE;md5=22a53954e4e0ec258dfce4391e905dac \
                    file://LICENSE-MIT;md5=b377b220f43d747efdec40d69fcaa69d"

DEPENDS = ""

SRC_URI = "git://github.com/eclipse-iceoryx/iceoryx2.git;protocol=https;nobranch=1"

inherit cargo cargo-update-recipe-crates

INSANE_SKIP:${PN} += "already-stripped"
FILES_SOLIBSDEV = ""

CARGO_BUILD_FLAGS:append = " --package iceoryx2-ffi-c --package iceoryx2-cli"

CARGO_BINDIR = "${B}/target/${CARGO_TARGET_SUBDIR}"

# integrations/ is a separate cargo workspace that neither recipe builds, so
# keep only the crates from the root Cargo.lock in the generated list.
do_update_crates:append() {
    nativepython3 - "${THISDIR}/${BPN}-crates.inc" <<'EOF'
import re, sys

with open(sys.argv[1]) as f:
    header, *blocks = re.split(r"(?m)^(?=# from )", f.read())

with open(sys.argv[1], "w") as f:
    f.write(header + "".join(b for b in blocks if b.startswith("# from Cargo.lock\n")))
EOF
}

BBCLASSEXTEND = "native nativesdk"

PACKAGES =+ "${PN}-cli"

# iceoryx2-c installs and packages libiceoryx2_ffi_c and its header, so they
# are only staged here, in the layout RUST_BUILD_ARTIFACT_PATH expects. Going
# through the sysroot keeps them in sstate and separate per MACHINE and per
# native/nativesdk variant.
IOX2_FFI_C_DIR = "${datadir}/iceoryx2/ffi-c"

sysroot_stage_all:append() {
    install -d ${SYSROOT_DESTDIR}${IOX2_FFI_C_DIR}/iceoryx2-ffi-c-cbindgen/include/iox2
    install -m 0644 ${CARGO_BINDIR}/libiceoryx2_ffi_c.a ${SYSROOT_DESTDIR}${IOX2_FFI_C_DIR}
    install -m 0755 ${CARGO_BINDIR}/libiceoryx2_ffi_c.so ${SYSROOT_DESTDIR}${IOX2_FFI_C_DIR}
    install -m 0644 ${CARGO_BINDIR}/iceoryx2-ffi-c-cbindgen/include/iox2/iceoryx2.h \
        ${SYSROOT_DESTDIR}${IOX2_FFI_C_DIR}/iceoryx2-ffi-c-cbindgen/include/iox2
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${CARGO_BINDIR}/iox2 ${D}${bindir}
    install -m 0755 ${CARGO_BINDIR}/iox2-config ${D}${bindir}
    install -m 0755 ${CARGO_BINDIR}/iox2-node ${D}${bindir}
    install -m 0755 ${CARGO_BINDIR}/iox2-service ${D}${bindir}
    install -m 0755 ${CARGO_BINDIR}/iox2-tunnel ${D}${bindir}
}

SUMMARY:${PN}-cli = "The iceoryx2 command line tools"
DESCRIPTION:${PN}-cli = "This package contains the iceoryx2 command line tools. \
                         Use 'iox2 --list' to show a list of all available commands."
HOMEPAGE:${PN}-cli = "https://iceoryx.io"
BUGTRACKER:${PN}-cli = "https://github.com/eclipse-iceoryx/iceoryx2/issues"
FILES:${PN}-cli += "${bindir}/iox2"
FILES:${PN}-cli += "${bindir}/iox2-config"
FILES:${PN}-cli += "${bindir}/iox2-node"
FILES:${PN}-cli += "${bindir}/iox2-service"
FILES:${PN}-cli += "${bindir}/iox2-tunnel"
