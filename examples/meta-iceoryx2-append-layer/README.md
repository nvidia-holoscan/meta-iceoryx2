# Customize the iceoryx2 yocto recipes

In order to customize the iceoryx2 build, a `meta-iceoryx2-append-layer` can be
created next to `meta-iceoryx2` and added to `conf/bblayers.conf`.

The `recipes/iceoryx2*_%.bbappend` files apply to whichever iceoryx2 version the
layer provides and can be used to pass additional flags to cargo, e.g. to enable
a feature:

```
CARGO_BUILD_FLAGS:append = " --features iceoryx2/dev_permissions"
```
