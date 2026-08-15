require ${BPN}.inc

SRCREV = "6ef7c55176a8487ae7ca53c2e0649d5df9a1b3b4"


# crate:// SRC_URI entries for this version. Required here rather than from the
# .inc because ${PV} only resolves correctly in the versioned .bb.
require iceoryx2-crates-${PV}.inc
