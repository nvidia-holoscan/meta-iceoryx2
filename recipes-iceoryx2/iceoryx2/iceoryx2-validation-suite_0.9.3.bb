require ${BPN}.inc

SRCREV = "6ef7c55176a8487ae7ca53c2e0649d5df9a1b3b4"

# Fixed upstream in v0.10.0. Drop with the next version bump.
SRC_URI += "file://0001-Reset-last-signal-in-SignalHandler-tests.patch"

# crate:// SRC_URI entries for this version. Required here rather than from the
# .inc because ${PV} only resolves correctly in the versioned .bb.
require iceoryx2-crates-${PV}.inc
