# meta-iceoryx2

Yocto Layer for Eclipse iceoryx2™

This layer builds [iceoryx2](https://github.com/eclipse-iceoryx/iceoryx2), the
zero-copy inter-process communication library, version 0.9.3, with
OpenEmbedded.

## Dependencies and supported releases

The layer depends on:

- [openembedded-core](https://git.openembedded.org/openembedded-core)
- `meta-oe` from [meta-openembedded](https://git.openembedded.org/meta-openembedded),
  which also provides gTest for the C++ unit tests

The `wrynose` branch follows the `wrynose` branches of these layers. The
`master` branch provides the same recipes for oe-core master (the `blacksail`
series).

## Provided packages and images

| Package | Contents |
| --- | --- |
| `iceoryx2-c` | C library (`libiceoryx2_ffi_c.so`) |
| `iceoryx2-cxx` | C++ library (`libiceoryx2_cxx.so`) |
| `iceoryx2-cli` | `iox2` command line tools |
| `iceoryx2-examples`, `iceoryx2-c-examples`, `iceoryx2-cxx-examples` | Rust, C and C++ examples in `/usr/bin/iceoryx2/examples` |
| `iceoryx2-benchmarks` | benchmarks in `/usr/bin/iceoryx2/benchmarks` |
| `iceoryx2-c-dev`, `iceoryx2-cxx-dev`, `iceoryx2-bb-cxx-dev`, `iceoryx2-cmake-modules-dev` | headers and CMake package files for building applications |
| `iceoryx2-validation-suite-ptest`, `iceoryx2-cxx-ptest`, `iceoryx2-bb-cxx-ptest` | unit tests, see [Run the ptest suites](#run-the-ptest-suites) |

`iceoryx2-bb-cxx` is header-only and `iceoryx2-cmake-modules` only carries
CMake files, so both exist only as `-dev` packages.

Two example images are included:

- `iceoryx2-image-minimal`: the C and C++ libraries
- `iceoryx2-image-full`: additionally the CLI tools, examples, benchmarks and
  unit tests

## Using iceoryx2 in your own image

Add the packages your application needs to the image, for example in
`conf/local.conf`:

```sh
IMAGE_INSTALL:append = " iceoryx2-cxx iceoryx2-cli"
```

`iceoryx2-cxx` pulls in `iceoryx2-c`. A recipe that builds an application
against iceoryx2 adds `iceoryx2-cxx` (or `iceoryx2-c`) to `DEPENDS` and finds
it with CMake's `find_package(iceoryx2-cxx)`. For an SDK, add the matching
`-dev` packages, e.g. `TOOLCHAIN_TARGET_TASK:append = " iceoryx2-cxx-dev"`.

To pass additional options to the iceoryx2 build, such as cargo features, see
the [example append layer](examples/meta-iceoryx2-append-layer/README.md).

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

`iceoryx2-image-full` includes the unit tests, so it needs `ptest` in
`DISTRO_FEATURES`, which OpenEmbedded-core enables by default. If your distro
disables it, build `iceoryx2-image-minimal` instead.

```sh
bitbake iceoryx2-image-full
```

By default, the image is built for `qemux86-64`. To build for another machine,
set `MACHINE` in `conf/local.conf`.

Once the build process completes, the image can be used with the
`runqemu qemux86-64` command.

### Run an iceoryx2 example

The examples are located under `/usr/bin/iceoryx2/examples`, with `rust`, `c` and `cxx` subfolders.

When qemu is used to run the image, log in as `root` with no password (see the
`EXTRA_IMAGE_FEATURES` step above). Once logged in, the following commands run
the publish-subscribe examples.

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

These commands list the available information about the iceoryx2 application.
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
openembedded-core and meta-openembedded, with `ptest` and the root login
features already enabled. For oe-core master, check out the `master` branch
and use `kas/master.yml` instead.

To boot the image:

```sh
kas shell meta-iceoryx2/kas/wrynose.yml -c 'runqemu kvm nographic'
```

`kvm` needs access to `/dev/kvm`; leave it out to run without hardware
acceleration.

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

## Security

To report a vulnerability, see [SECURITY.md](SECURITY.md).

## License

meta-iceoryx2 is licensed under either the Apache License 2.0 or the MIT
license, at your option. See [LICENSE-APACHE](LICENSE-APACHE) and
[LICENSE-MIT](LICENSE-MIT).
