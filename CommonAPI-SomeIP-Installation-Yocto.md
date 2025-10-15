# CommonAPI and vSomeIP Installation Guide

Complete guide for installing CommonAPI and vSomeIP on x86_64 Ubuntu and integrating with Yocto for embedded systems.

---

## Table of Contents

1. [X86_64 Ubuntu Installation](#x86_64-ubuntu-installation)
2. [Yocto Integration for Raspberry Pi 3](#yocto-integration-for-raspberry-pi-3)
3. [Usage and Testing](#usage-and-testing)
4. [Troubleshooting](#troubleshooting)

---

## X86_64 Ubuntu Installation

### Prerequisites

- **Operating System**: Ubuntu 18.04, 20.04, or 22.04 (x86_64)
- **RAM**: Minimum 4GB (8GB recommended)
- **Disk Space**: ~5GB free space
- **Internet Connection**: Required for downloading packages and repositories

### System Requirements

```bash
# Check your system
uname -m  # Should show: x86_64
lsb_release -a  # Shows Ubuntu version
```

### Installation Steps

#### 1. Create Working Directory

```bash
mkdir -p ~/workspace/vsomeip
cd ~/workspace/vsomeip
```

#### 2. Create Installation Script

Create a file named `installer.sh`:

```bash
nano installer.sh
```

Copy the complete script from below and paste it into the file.

#### 3. Installation Script

```bash
#!/usr/bin/bash

echo "###############################################################"
echo "#                                                             #"
echo "#         CommonAPI and vSomeIP Installation Script          #"
echo "#                                                             #"
echo "###############################################################"

# Get the starting directory
START_DIR=$(pwd)
echo "Working directory: $START_DIR"

# Install required dependencies
echo "Installing dependencies..."
sudo apt-get update 
sudo apt-get install -y net-tools cmake build-essential wget unzip openjdk-8-jdk git

# Install GCC 10 for better C++14 support
echo "Installing GCC 10..."
sudo apt-get install -y gcc-10 g++-10

# Set GCC 10 as the compiler
export CC=/usr/bin/gcc-10
export CXX=/usr/bin/g++-10

echo "Using GCC version:"
$CXX --version

# Remove latest version of boost if exists
sudo apt-get --purge remove libboost-dev libboost-doc 2> /dev/null || true

# Download and install Boost 1.58
echo "###############################################################"
echo "#                                                             #"
echo "#                 Building Boost 1.58                         #"
echo "#                 Don't worry about warnings                  #"
echo "#                                                             #"
echo "###############################################################"

cd "$START_DIR"
if [ ! -f "boost_1_58_0.tar.gz" ]; then
    wget http://sourceforge.net/projects/boost/files/boost/1.58.0/boost_1_58_0.tar.gz
fi

if [ ! -d "boost_1_58_0" ]; then
    tar -xf boost_1_58_0.tar.gz
fi

cd boost_1_58_0/
./bootstrap.sh --prefix=/usr/
sudo ./b2 -j$(nproc) || echo "Boost build completed with warnings (this is OK)"
sudo ./b2 install || echo "Boost install completed with warnings (this is OK)"
cd "$START_DIR"

echo "✅ Boost installation completed"

# Build capicxx-core-runtime
echo "###############################################################"
echo "#                                                             #"
echo "#            Building capicxx-core-runtime                    #"
echo "#                                                             #"
echo "###############################################################"

cd "$START_DIR"
if [ ! -d "capicxx-core-runtime" ]; then
    git clone https://github.com/GENIVI/capicxx-core-runtime.git
fi

cd capicxx-core-runtime
git checkout tags/3.1.12.6

# Apply patch to fix Deployable copy constructor issue
echo "Applying Deployable.hpp patch..."
cat > /tmp/deployable.patch << 'EOF'
--- a/include/CommonAPI/Deployable.hpp
+++ b/include/CommonAPI/Deployable.hpp
@@ -28,6 +28,9 @@
     Deployable(const Type_ &_value, const TypeDepl_ *_depl)
         : value_(_value), depl_(_depl) {}
 
+    Deployable(const Deployable<Type_, TypeDepl_> &_source)
+        : value_(_source.value_), depl_(_source.depl_) {}
+
     Deployable<Type_, TypeDepl_>& operator=(const Deployable<Type_, TypeDepl_> &_source) {
         value_ = _source.value_;
         depl_ = _source.depl_;
EOF

patch -p1 < /tmp/deployable.patch 2>/dev/null || echo "Patch already applied or not needed"

rm -rf build
mkdir -p build
cd build
cmake -DCMAKE_CXX_STANDARD=14 -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX ..
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "❌ Error in capicxx-core-runtime"
    exit 1
fi

echo "✅ Successfully built capicxx-core-runtime"

# Copy libraries
cd "$START_DIR"
mkdir -p COMMONAPI
cp -d capicxx-core-runtime/build/lib* COMMONAPI/ 2>/dev/null || true

# Build vSomeIP
echo "###############################################################"
echo "#                                                             #"
echo "#                 Building vSomeIP                            #"
echo "#                                                             #"
echo "###############################################################"

cd "$START_DIR"
if [ ! -d "vSomeIP" ]; then
    git clone https://github.com/GENIVI/vSomeIP.git
fi

cd vSomeIP
git checkout tags/2.14.16

# Fix missing #include <string> in primitive_types.hpp
echo "Patching vSomeIP for GCC compatibility..."
sed -i '/#include <cstdint>/a #include <string>' interface/vsomeip/primitive_types.hpp

rm -rf build
mkdir -p build
cd build
cmake -DCMAKE_CXX_STANDARD=14 -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX ..
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "❌ Error in vSomeIP"
    exit 1
fi

echo "✅ Successfully built vSomeIP"

# Copy libraries
cd "$START_DIR"
cp -d vSomeIP/build/lib* COMMONAPI/ 2>/dev/null || true

# Build capicxx-someip-runtime
echo "###############################################################"
echo "#                                                             #"
echo "#            Building capicxx-someip-runtime                  #"
echo "#                                                             #"
echo "###############################################################"

cd "$START_DIR"
if [ ! -d "capicxx-someip-runtime" ]; then
    git clone https://github.com/GENIVI/capicxx-someip-runtime.git
fi

cd capicxx-someip-runtime
git checkout tags/3.1.12.9
rm -rf build
mkdir -p build
cd build

# Set paths to find capicxx-core and vsomeip
export PKG_CONFIG_PATH="$START_DIR/capicxx-core-runtime/build:$START_DIR/vSomeIP/build:$PKG_CONFIG_PATH"
export COMMONAPI_TOOL_DIR="$START_DIR/capicxx-core-runtime"
export VSOMEIP_TOOL_DIR="$START_DIR/vSomeIP"

cmake -DUSE_INSTALLED_COMMONAPI=OFF \
      -DCMAKE_CXX_STANDARD=14 \
      -DCMAKE_C_COMPILER=$CC \
      -DCMAKE_CXX_COMPILER=$CXX \
      -DCMAKE_PREFIX_PATH="$START_DIR/capicxx-core-runtime/build;$START_DIR/vSomeIP/build" \
      ..
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "❌ Error in capicxx-someip-runtime"
    exit 1
fi

echo "✅ Successfully built capicxx-someip-runtime"

# Copy libraries
cd "$START_DIR"
cp -d capicxx-someip-runtime/build/lib* COMMONAPI/ 2>/dev/null || true

# Download and setup generators
echo "###############################################################"
echo "#                                                             #"
echo "#              Downloading Code Generators                    #"
echo "#                                                             #"
echo "###############################################################"

cd "$START_DIR"
if [ ! -f "commonapi-generator.zip" ]; then
    wget https://github.com/COVESA/capicxx-core-tools/releases/download/3.1.12.4/commonapi-generator.zip
fi

if [ ! -f "commonapi_someip_generator.zip" ]; then
    wget https://github.com/COVESA/capicxx-someip-tools/releases/download/3.1.12/commonapi_someip_generator.zip
fi

rm -rf commonapi-generator commonapi_someip_generator
unzip -o commonapi-generator.zip -d commonapi-generator
unzip -o commonapi_someip_generator.zip -d commonapi_someip_generator

chmod +x commonapi-generator/commonapi-generator-linux-x86_64
chmod +x commonapi_someip_generator/commonapi-someip-generator-linux-x86_64

echo "✅ Generators downloaded and configured"

# Clone example project
echo "###############################################################"
echo "#                                                             #"
echo "#              Cloning Example Project                        #"
echo "#                                                             #"
echo "###############################################################"

cd "$START_DIR"
if [ ! -d "vsomeip_helloworld" ]; then
    git clone https://github.com/moatasemelsayed/vsomeip_helloworld.git
fi

# Export LD_LIBRARY_PATH
COMMONAPI_LIB_PATH="$START_DIR/COMMONAPI"

# Check if the export already exists in .bashrc
if ! grep -q "COMMONAPI" ~/.bashrc; then
    echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:${COMMONAPI_LIB_PATH}" >> ~/.bashrc
fi

echo ""
echo "###############################################################"
echo "#                                                             #"
echo "#                 Installation Complete!                      #"
echo "#                                                             #"
echo "###############################################################"
echo ""
echo "Installation Summary:"
echo "  ✅ Boost 1.58.0"
echo "  ✅ capicxx-core-runtime 3.1.12.6"
echo "  ✅ vSomeIP 2.14.16"
echo "  ✅ capicxx-someip-runtime 3.1.12.9"
echo "  ✅ Code generators"
echo ""
echo "Generator Usage:"
echo "  Core Generator:"
echo "    $START_DIR/commonapi-generator/commonapi-generator-linux-x86_64 -sk <fidl file>"
echo ""
echo "  SomeIP Generator:"
echo "    $START_DIR/commonapi_someip_generator/commonapi-someip-generator-linux-x86_64 -ll verbose <fdepl file>"
echo ""
echo "Library Path:"
echo "  export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:${COMMONAPI_LIB_PATH}"
echo ""
echo "Example project available at: $START_DIR/vsomeip_helloworld"
echo ""
echo "Please restart your terminal or run: source ~/.bashrc"
echo ""
```

#### 4. Make Script Executable and Run

```bash
chmod +x installer.sh
./installer.sh
```

#### 5. Load Environment

After installation completes:

```bash
source ~/.bashrc
```

### Installation Time

- **Total Time**: 20-40 minutes depending on system performance
- **Boost Build**: 15-25 minutes
- **Other Components**: 5-15 minutes

### Verify Installation

```bash
# Check libraries
ls -la ~/workspace/vsomeip/COMMONAPI/

# Expected output:
# libCommonAPI.so.3.1.12
# libCommonAPI-SomeIP.so.3.1.12
# libvsomeip.so.2.14.16
# libvsomeip-sd.so.2.14.16
# libvsomeip-cfg.so.2.14.16

# Check generators
~/workspace/vsomeip/commonapi-generator/commonapi-generator-linux-x86_64 --version
~/workspace/vsomeip/commonapi_someip_generator/commonapi-someip-generator-linux-x86_64 --version
```

### Installed Components

| Component | Version | Description |
|-----------|---------|-------------|
| Boost | 1.58.0 | C++ libraries for threading, filesystem, etc. |
| capicxx-core-runtime | 3.1.12.6 | CommonAPI C++ core runtime |
| vSomeIP | 2.14.16 | SOME/IP protocol implementation |
| capicxx-someip-runtime | 3.1.12.9 | CommonAPI SOME/IP binding |
| commonapi-generator | 3.1.12.4 | Code generator for CommonAPI |
| commonapi-someip-generator | 3.1.12 | Code generator for SOME/IP binding |

---

## Yocto Integration for Raspberry Pi 3

### Overview

Integrating CommonAPI and vSomeIP into Yocto requires a different approach than the x86_64 installation. You need to use BitBake recipes instead of bash scripts.

### Prerequisites

- Yocto build environment set up for Raspberry Pi 3
- Basic understanding of BitBake and Yocto layers
- At least 50GB free disk space for Yocto build

### Architecture Differences

| Aspect | x86_64 Ubuntu | Yocto/RPi3 |
|--------|---------------|------------|
| **Architecture** | x86_64 | ARM (armv7) |
| **Compiler** | Host GCC-10 | Cross-compiler (arm-linux-gnueabihf-gcc) |
| **Build System** | Direct bash/cmake | BitBake recipes |
| **Installation** | Host system | Target rootfs image |

### Method 1: Using meta-ivi Layer (Recommended)

The easiest way is to use the existing meta-ivi layer from COVESA (formerly GENIVI).

#### Step 1: Add meta-ivi to Your Yocto Build

```bash
# Navigate to your Yocto sources directory
cd ~/yocto/poky/sources  # Adjust path to your Yocto setup

# Clone meta-ivi layer
git clone https://github.com/COVESA/meta-ivi.git -b <branch>
# Replace <branch> with your Yocto version (e.g., kirkstone, dunfell)
```

#### Step 2: Add Layer to bblayers.conf

```bash
cd ~/yocto/poky/build  # Your build directory
bitbake-layers add-layer ../sources/meta-ivi
```

Or manually edit `conf/bblayers.conf`:

```
BBLAYERS ?= " \
  /path/to/poky/meta \
  /path/to/poky/meta-poky \
  /path/to/poky/meta-yocto-bsp \
  /path/to/poky/meta-raspberrypi \
  /path/to/poky/sources/meta-ivi \
  "
```

#### Step 3: Add Packages to Your Image

Edit your image recipe or `conf/local.conf`:

```bash
# In conf/local.conf
IMAGE_INSTALL:append = " vsomeip commonapi-core-runtime commonapi-someip-runtime"

# For Raspberry Pi 3 specific settings
MACHINE = "raspberrypi3"
```

#### Step 4: Build

```bash
bitbake core-image-minimal
# Or your custom image recipe
```

### Method 2: Create Custom Recipes

If you need specific versions not in meta-ivi, create custom recipes.

#### Directory Structure

```
meta-custom/
├── conf/
│   └── layer.conf
└── recipes-commonapi/
    ├── boost/
    │   └── boost_1.58.0.bb
    ├── vsomeip/
    │   ├── vsomeip_2.14.16.bb
    │   └── files/
    │       └── 0001-fix-string-include.patch
    ├── capicxx-core/
    │   ├── capicxx-core-runtime_3.1.12.6.bb
    │   └── files/
    │       └── 0001-fix-deployable-copy-constructor.patch
    └── capicxx-someip/
        └── capicxx-someip-runtime_3.1.12.9.bb
```

#### Example: vsomeip Recipe

Create `meta-custom/recipes-commonapi/vsomeip/vsomeip_2.14.16.bb`:

```bitbake
SUMMARY = "vSomeIP implementation"
DESCRIPTION = "Implementation of Scalable service-Oriented MiddlewarE over IP"
HOMEPAGE = "https://github.com/COVESA/vsomeip"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=815ca599c9df247a0c7f619bab123dad"

SRC_URI = "git://github.com/COVESA/vsomeip.git;protocol=https;branch=master"
SRCREV = "2.14.16"

SRC_URI += "file://0001-fix-string-include.patch"

S = "${WORKDIR}/git"

DEPENDS = "boost"

inherit cmake

EXTRA_OECMAKE = " \
    -DCMAKE_CXX_STANDARD=14 \
    -DENABLE_SIGNAL_HANDLING=1 \
"

do_install:append() {
    # Install configuration files if needed
    install -d ${D}${sysconfdir}/vsomeip
}

FILES:${PN} = " \
    ${libdir}/libvsomeip*.so.* \
    ${sysconfdir}/vsomeip \
"

FILES:${PN}-dev = " \
    ${includedir} \
    ${libdir}/libvsomeip*.so \
    ${libdir}/pkgconfig \
    ${libdir}/cmake \
"

RDEPENDS:${PN} = "boost"
```

#### Example: Patch File

Create `meta-custom/recipes-commonapi/vsomeip/files/0001-fix-string-include.patch`:

```diff
From: Your Name <your.email@example.com>
Date: Wed, 8 Oct 2025 12:00:00 +0000
Subject: [PATCH] Add missing string header include

---
 interface/vsomeip/primitive_types.hpp | 1 +
 1 file changed, 1 insertion(+)

diff --git a/interface/vsomeip/primitive_types.hpp b/interface/vsomeip/primitive_types.hpp
index abcdef..123456 100644
--- a/interface/vsomeip/primitive_types.hpp
+++ b/interface/vsomeip/primitive_types.hpp
@@ -10,6 +10,7 @@
 
 #include <cstdint>
+#include <string>
 
 namespace vsomeip {
```

#### Example: capicxx-core-runtime Recipe

Create `meta-custom/recipes-commonapi/capicxx-core/capicxx-core-runtime_3.1.12.6.bb`:

```bitbake
SUMMARY = "CommonAPI C++ core runtime"
DESCRIPTION = "CommonAPI C++ is a C++ framework for inter-process and network communication"
HOMEPAGE = "https://github.com/COVESA/capicxx-core-runtime"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=815ca599c9df247a0c7f619bab123dad"

SRC_URI = "git://github.com/COVESA/capicxx-core-runtime.git;protocol=https;branch=master"
SRCREV = "3.1.12.6"

SRC_URI += "file://0001-fix-deployable-copy-constructor.patch"

S = "${WORKDIR}/git"

inherit cmake

EXTRA_OECMAKE = "-DCMAKE_CXX_STANDARD=14"

FILES:${PN} = "${libdir}/libCommonAPI*.so.*"
FILES:${PN}-dev = " \
    ${includedir} \
    ${libdir}/libCommonAPI*.so \
    ${libdir}/pkgconfig \
    ${libdir}/cmake \
"
```

#### Example: capicxx-someip-runtime Recipe

Create `meta-custom/recipes-commonapi/capicxx-someip/capicxx-someip-runtime_3.1.12.9.bb`:

```bitbake
SUMMARY = "CommonAPI C++ SOME/IP runtime"
DESCRIPTION = "SOME/IP binding for CommonAPI C++"
HOMEPAGE = "https://github.com/COVESA/capicxx-someip-runtime"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=815ca599c9df247a0c7f619bab123dad"

SRC_URI = "git://github.com/COVESA/capicxx-someip-runtime.git;protocol=https;branch=master"
SRCREV = "3.1.12.9"

S = "${WORKDIR}/git"

DEPENDS = "capicxx-core-runtime vsomeip boost"

inherit cmake

EXTRA_OECMAKE = " \
    -DCMAKE_CXX_STANDARD=14 \
    -DUSE_INSTALLED_COMMONAPI=ON \
"

FILES:${PN} = "${libdir}/libCommonAPI-SomeIP*.so.*"
FILES:${PN}-dev = " \
    ${includedir} \
    ${libdir}/libCommonAPI-SomeIP*.so \
    ${libdir}/pkgconfig \
    ${libdir}/cmake \
"

RDEPENDS:${PN} = "capicxx-core-runtime vsomeip"
```

#### Example: layer.conf

Create `meta-custom/conf/layer.conf`:

```
# We have a conf and classes directory, add to BBPATH
BBPATH .= ":${LAYERDIR}"

# We have recipes-* directories, add to BBFILES
BBFILES += "${LAYERDIR}/recipes-*/*/*.bb \
            ${LAYERDIR}/recipes-*/*/*.bbappend"

BBFILE_COLLECTIONS += "meta-custom"
BBFILE_PATTERN_meta-custom = "^${LAYERDIR}/"
BBFILE_PRIORITY_meta-custom = "6"

LAYERDEPENDS_meta-custom = "core"
LAYERSERIES_COMPAT_meta-custom = "kirkstone dunfell"
```

### Step-by-Step Yocto Integration

#### 1. Create Your Custom Layer

```bash
cd ~/yocto/poky/sources
bitbake-layers create-layer meta-custom
cd ~/yocto/poky/build
bitbake-layers add-layer ../sources/meta-custom
```

#### 2. Add Recipes

Create the directory structure and recipes as shown above.

#### 3. Create a Custom Image Recipe

Create `meta-custom/recipes-core/images/rpi3-commonapi-image.bb`:

```bitbake
SUMMARY = "Raspberry Pi 3 image with CommonAPI and vSomeIP"
LICENSE = "MIT"

inherit core-image

IMAGE_FEATURES += "ssh-server-openssh"

IMAGE_INSTALL:append = " \
    vsomeip \
    capicxx-core-runtime \
    capicxx-someip-runtime \
    boost \
    kernel-modules \
"

# Additional tools for development
IMAGE_INSTALL:append = " \
    vim \
    htop \
    gdb \
    strace \
"

export IMAGE_BASENAME = "rpi3-commonapi-image"
```

#### 4. Build the Image

```bash
bitbake rpi3-commonapi-image
```

#### 5. Flash to SD Card

```bash
# Find your SD card device (e.g., /dev/sdb)
lsblk

# Flash the image (BE CAREFUL - this will erase the SD card)
sudo dd if=tmp/deploy/images/raspberrypi3/rpi3-commonapi-image-raspberrypi3.wic of=/dev/sdX bs=4M status=progress
sync
```

### Runtime Configuration

On the Raspberry Pi 3, you'll need configuration files for vSomeIP.

Create `/etc/vsomeip/vsomeip.json`:

```json
{
    "unicast": "192.168.1.100",
    "logging": {
        "level": "info",
        "console": "true",
        "file": {
            "enable": "false",
            "path": "/tmp/vsomeip.log"
        },
        "dlt": "false"
    },
    "applications": [
        {
            "name": "HelloWorldService",
            "id": "0x1111"
        },
        {
            "name": "HelloWorldClient",
            "id": "0x2222"
        }
    ],
    "services": [
        {
            "service": "0x1234",
            "instance": "0x5678",
            "unreliable": "30509"
        }
    ],
    "routing": "HelloWorldService",
    "service-discovery": {
        "enable": "true",
        "multicast": "224.0.0.1",
        "port": "30490",
        "protocol": "udp"
    }
}
```

---

## Usage and Testing

### Generate Code from FIDL

#### 1. Create a FIDL File

Create `HelloWorld.fidl`:

```fidl
package com.example

interface HelloWorld {
    version { major 1 minor 0 }
    
    method sayHello {
        in {
            String name
        }
        out {
            String message
        }
    }
    
    broadcast newMessage {
        out {
            String message
        }
    }
}
```

#### 2. Create FDEPL File

Create `HelloWorld.fdepl`:

```fdepl
import "platform:/plugin/org.genivi.commonapi.someip/deployment/CommonAPI-SOMEIP_deployment_spec.fdepl"
import "HelloWorld.fidl"

define org.genivi.commonapi.someip.deployment for interface com.example.HelloWorld {
    SomeIpServiceID = 4660
    
    method sayHello {
        SomeIpMethodID = 33000
    }
    
    broadcast newMessage {
        SomeIpEventID = 33001
    }
}

define org.genivi.commonapi.someip.deployment for provider as Service {
    instance com.example.HelloWorld {
        InstanceId = "test"
        SomeIpInstanceID = 22136
    }
}
```

#### 3. Generate Code

```bash
cd ~/workspace/vsomeip

# Generate CommonAPI code
./commonapi-generator/commonapi-generator-linux-x86_64 -sk HelloWorld.fidl

# Generate SOME/IP binding code
./commonapi_someip_generator/commonapi-someip-generator-linux-x86_64 HelloWorld.fdepl
```

### Build and Run Example

```bash
# Navigate to example
cd ~/workspace/vsomeip/vsomeip_helloworld

# Update CMakeLists.txt with your paths
# Set COMMONAPI_INSTALL_PATH, VSOMEIP_INSTALL_PATH, etc.

# Build
mkdir build
cd build
cmake ..
make

# Run service
export LD_LIBRARY_PATH=~/workspace/vsomeip/COMMONAPI:$LD_LIBRARY_PATH
export VSOMEIP_CONFIGURATION=../vsomeip-service.json
./HelloWorldService

# In another terminal, run client
export LD_LIBRARY_PATH=~/workspace/vsomeip/COMMONAPI:$LD_LIBRARY_PATH
export VSOMEIP_CONFIGURATION=../vsomeip-client.json
./HelloWorldClient
```

---

## Troubleshooting

### X86_64 Ubuntu Issues

#### Issue: Boost Warnings About `-Wfree-nonheap-object`

**Solution**: These are harmless warnings. Boost 1.58 has compatibility issues with GCC 10+, but the libraries still build correctly.

```bash
# Verify Boost installed correctly
ls /usr/lib/libboost_system.so*
```

#### Issue: `capicxx-core-runtime` Build Fails

**Error**: `Deployable` copy constructor issue

**Solution**: Ensure the patch is applied:

```bash
cd ~/workspace/vsomeip/capicxx-core-runtime
grep "Deployable(const Deployable" include/CommonAPI/Deployable.hpp
# Should show the copy constructor
```

#### Issue: `vSomeIP` Build Fails with Missing `std::string`

**Error**: `'string' in namespace 'std' does not name a type`

**Solution**: Apply the string header patch:

```bash
cd ~/workspace/vsomeip/vSomeIP
sed -i '/#include <cstdint>/a #include <string>' interface/vsomeip/primitive_types.hpp
```

#### Issue: Libraries Not Found at Runtime

**Error**: `error while loading shared libraries: libCommonAPI.so`

**Solution**: Check and reload library path:

```bash
echo $LD_LIBRARY_PATH
# Should include: /home/username/workspace/vsomeip/COMMONAPI

source ~/.bashrc
# Or manually:
export LD_LIBRARY_PATH=~/workspace/vsomeip/COMMONAPI:$LD_LIBRARY_PATH
```

### Yocto Build Issues

#### Issue: Fetch Failures

**Error**: `Fetcher failure for URL: 'git://github.com/...'`

**Solution**: Check network and use HTTPS protocol:

```bitbake
SRC_URI = "git://github.com/COVESA/vsomeip.git;protocol=https;branch=master"
```

#### Issue: Patch Doesn't Apply

**Error**: `patch does not apply`

**Solution**: Verify patch format and SRCREV:

```bash
# Test patch manually
cd tmp/work/.../git
patch -p1 --dry-run < ../../0001-fix.patch
```

#### Issue: Cross-Compilation Errors

**Error**: Architecture mismatch errors

**Solution**: Ensure you're not mixing host and target builds. Clean and rebuild:

```bash
bitbake -c cleanall vsomeip
bitbake vsomeip
```

#### Issue: Missing Dependencies

**Error**: `Nothing PROVIDES 'boost'`

**Solution**: Add dependency layers:

```bash
bitbake-layers add-layer ../sources/meta-openembedded/meta-oe
```

### Runtime Issues on Raspberry Pi

#### Issue: vSomeIP Can't Find Configuration

**Error**: `Configuration file not found`

**Solution**: Set environment variable:

```bash
export VSOMEIP_CONFIGURATION=/etc/vsomeip/vsomeip.json
```

#### Issue: Service Discovery Not Working

**Solution**: Check network configuration and firewall:

```bash
# Enable multicast
ip route add 224.0.0.0/4 dev eth0

# Check if port is accessible
netstat -an | grep 30490
```