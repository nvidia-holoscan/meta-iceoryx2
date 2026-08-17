SUMMARY = "iceoryx2"
DESCRIPTION = "This package is used to build the iceoryx2 benchmarks, examples and tests"
HOMEPAGE = "https://iceoryx.io"
BUGTRACKER = "https://github.com/eclipse-iceoryx/iceoryx2/issues"
LICENSE = "Apache-2.0 | MIT"
LIC_FILES_CHKSUM = "file://LICENSE-APACHE;md5=22a53954e4e0ec258dfce4391e905dac \
                    file://LICENSE-MIT;md5=b377b220f43d747efdec40d69fcaa69d"

DEPENDS = ""

SRC_URI = "git://github.com/eclipse-iceoryx/iceoryx2.git;protocol=https;nobranch=1 \
           file://run-ptest"

inherit ptest-cargo

INSANE_SKIP:${PN} += "already-stripped"
FILES_SOLIBSDEV = ""

# Tests that cannot pass on the target are excluded: the proc-macro crates,
# whose tests are built for the host; iceoryx2-cli, whose tests create an
# executable at run time; the tunnel end-to-end tests, which need two
# processes at once; the no_std tests, which abort on their expected panics.
CARGO_BUILD_FLAGS:append = " --workspace --all-targets \
    --exclude iceoryx2-ffi-python \
    --exclude component-tests_rust \
    --exclude iceoryx2-services-tunnel-end-to-end-tests \
    --exclude iceoryx2-cli \
    --exclude iceoryx2-bb-container-tests-nostd \
    --exclude iceoryx2-bb-lock-free-tests-nostd \
    --exclude iceoryx2-bb-trait-tests-nostd \
    --exclude iceoryx2-conformance-tests-nostd \
    --exclude iceoryx2-ffi-macros \
    --exclude iceoryx2-bb-derive-macros \
    --exclude iceoryx2-bb-testing-macros"

CARGO_BINDIR = "${B}/target/${CARGO_TARGET_SUBDIR}"

BBCLASSEXTEND = "native nativesdk"

PACKAGES =+ "iceoryx2-benchmarks iceoryx2-examples"

do_install() {
    install -d ${D}${bindir}/iceoryx2/benchmarks
    install -m 0755 ${CARGO_BINDIR}/benchmark-event ${D}${bindir}/iceoryx2/benchmarks
    install -m 0755 ${CARGO_BINDIR}/benchmark-publish-subscribe ${D}${bindir}/iceoryx2/benchmarks
    install -m 0755 ${CARGO_BINDIR}/benchmark-queue ${D}${bindir}/iceoryx2/benchmarks
    install -m 0755 ${CARGO_BINDIR}/benchmark-request-response ${D}${bindir}/iceoryx2/benchmarks

    install -d ${D}${bindir}/iceoryx2/examples/rust
    for example in ${CARGO_BINDIR}/examples/*; do
        if [ -f "$example" ] && [ -x "$example" ] && [ "${example##*.}" != "so" ]; then
            install -m 0755 "$example" ${D}${bindir}/iceoryx2/examples/rust
        fi
    done
}

do_install_ptest() {
    install -d ${D}/${PTEST_PATH}
    install -m 0755 ${UNPACKDIR}/run-ptest ${D}/${PTEST_PATH}/
}

SUMMARY:iceoryx2-benchmarks = "The iceoryx2 benchmarks"
DESCRIPTION:iceoryx2-benchmarks = "This package contains the iceoryx2 benchmarks. \
                                They are available in '/usr/bin/iceoryx2/benchmarks'"
HOMEPAGE:iceoryx2-benchmarks = "https://iceoryx.io"
BUGTRACKER:iceoryx2-benchmarks = "https://github.com/eclipse-iceoryx/iceoryx2/issues"
FILES:iceoryx2-benchmarks += "${bindir}/iceoryx2/benchmarks/*"

SUMMARY:iceoryx2-examples = "The iceoryx2 Rust examples"
DESCRIPTION:iceoryx2-examples = "This package contains the iceoryx2 Rust examples. \
                              They are available in '/usr/bin/iceoryx2/examples'"
HOMEPAGE:iceoryx2-examples = "https://iceoryx.io"
BUGTRACKER:iceoryx2-examples = "https://github.com/eclipse-iceoryx/iceoryx2/issues"
FILES:iceoryx2-examples += "${bindir}/iceoryx2/examples/rust/*"

# NOTE bitbake complains about nobody providing iceoryx2-validation-suite needed by
#      iceoryx2-validation-suite-ptest; since iceoryx2-validation-suite is an empty
#      package, it will not be generated -> remove it from the dependencies
RDEPENDS:iceoryx2-validation-suite-ptest:remove = "iceoryx2-validation-suite"
