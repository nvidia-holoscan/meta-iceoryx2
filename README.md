# meta-iceoryx2

Yocto Layer for Eclipse iceoryx2™

## Manual setup

### Prerequisites

This setup is based on Ubuntu 24.04.

If you are running in a docker environment with a minimal Ubuntu setup,
ensure that a timezone is set before running the other commands. One option
to succeed with this is:

```sh
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata
```

Before we start with the Yocto build, ensure that the following dependencies 
are installed:

```sh
sudo apt install -y \
    chrpath \
    cpio \
    diffstat \
    file \
    g++ \
    gawk \
    gcc \
    git \
    language-pack-en \
    lz4 \
    make \
    python3 \
    python3-pip \
    python3-venv \
    wget \
    xz-utils \
    zstd
```

The complete list for each host distribution is in the Yocto Project
[system requirements](https://docs.yoctoproject.org/dev/ref-manual/system-requirements.html#ubuntu-and-debian).

### Setup the Yocto build

Move to your project directory and clone bitbake, OpenEmbedded-core,
meta-openembedded and this layer at their wrynose branches. The layer declares
`LAYERSERIES_COMPAT_meta-iceoryx2 = "wrynose"` and relies on the `UNPACKDIR`
source layout, so it will not parse against older releases.

```sh
git clone -b 2.18 https://git.openembedded.org/bitbake
git clone -b wrynose https://git.openembedded.org/openembedded-core
git clone -b wrynose https://git.openembedded.org/meta-openembedded
git clone -b wrynose https://github.com/eclipse-iceoryx/meta-iceoryx2.git
```

Now source `oe-init-build-env`. This creates the build directory and changes
into it; pass a path to `oe-init-build-env` to put it somewhere else.

```sh
source openembedded-core/oe-init-build-env
```

Add the layers to `conf/bblayers.conf`. meta-iceoryx2 depends on `meta-oe`,
which also provides gTest for the C++ unit tests:

```sh
bitbake-layers add-layer ../meta-openembedded/meta-oe
bitbake-layers add-layer ../meta-iceoryx2
```

The Rust parts of iceoryx2 are built with the `cargo` and `ptest-cargo` classes
that OpenEmbedded-core ships, so no additional layer is needed for the Rust
toolchain.

Since Yocto can consume quite a lot of disk space, we add
`INHERIT += "rm_work"` to `conf/local.conf`.

```sh
echo 'INHERIT += "rm_work"' >> conf/local.conf
```

To log in to the image as `root` without a password, which the examples below
assume, also add:

```sh
echo 'EXTRA_IMAGE_FEATURES ?= "allow-empty-password empty-root-password allow-root-login"' >> conf/local.conf
```

### Build iceoryx2-image-full

We are now ready to build the `iceoryx2-image-full`, which does not only
contain the libraries and CLI tools but also the examples and tests.

For enable unit tests of iceoryx2 and iceoryx2-cxx, add to local.conf

```sh
DISTRO_FEATURES:append = " ptest"
```

```sh
bitbake iceoryx2-image-full
```

By default, the `qemux86-64` image is build. To select a different image, just
uncomment the desired platform from the `Machine Selection`.

Once the build process completes, the image can be used with the
`runqemu qemux86-64` command.

### Run an iceoryx2 example

The examples are located under `/usr/bin/iceoryx2/examples`, with `rust`, `c` and `cxx` subfolders.

When qemu is used to run the image, the user needs to login with `root` and no
password. Once logged in, the following command can be used to run the
publish-subscribe examples.

```sh
/usr/bin/iceoryx2/examples/rust/publish_subscribe_subscriber &
/usr/bin/iceoryx2/examples/rust/publish_subscribe_publisher
```

On the terminal, the output of the applications should now appear.

When the publisher is stopped, the iceoryx2 CLI can be used to list the
information from the still running subscriber example.

```sh
iox2 node list
iox2 service list
```

These command will list the available information about the iceoryx2 application.
For more details about the iceoryx2 CLI, please visit https://github.com/eclipse-iceoryx/iceoryx2/tree/main/iceoryx2-cli.

## How to build iceoryx2-image-full with kas

```sh
mkdir yocto-iceoryx2
cd yocto-iceoryx2
python3 -m venv .venv
source .venv/bin/activate
pip3 install kas
git clone -b wrynose https://github.com/eclipse-iceoryx/meta-iceoryx2.git
kas build meta-iceoryx2/kas/wrynose.yml
```

This builds for `qemux86-64` against the wrynose branches of bitbake,
openembedded-core and meta-openembedded. To boot the image:

```sh
kas shell meta-iceoryx2/kas/wrynose.yml -c 'runqemu kvm nographic'
```

## Run the ptest suites

The test suites are packaged as ptest and require `ptest` in `DISTRO_FEATURES`.
Boot the image on hardware or under `runqemu`, log in as `root`, then:

```sh
# For running iceoryx2 unit tests
ptest-runner iceoryx2-validation-suite
# For running iceoryx2-cxx unit tests
ptest-runner iceoryx2-cxx
# For running iceoryx2-bb-cxx unit tests
ptest-runner iceoryx2-bb-cxx
```

## Contributing

Contributions are welcome as pull requests against
https://github.com/eclipse-iceoryx/meta-iceoryx2, where the maintainers review
them. Please sign off your commits with `git commit -s`.

Source patches for the iceoryx2 upstream code belong next to the recipe that
applies them, and each one needs an `Upstream-Status:` tag as described in the
[OpenEmbedded patch guidelines](https://docs.yoctoproject.org/contributor-guide/recipe-style-guide.html#patch-upstream-status),
so that the `patch-status` QA check passes.
