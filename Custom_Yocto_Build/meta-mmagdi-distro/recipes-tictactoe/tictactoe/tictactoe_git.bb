# Recipe created by recipetool
# This is the basis of a recipe and may need further editing in order to be fully functional.
# (Feel free to remove these comments when editing.)

# WARNING: the following LICENSE and LIC_FILES_CHKSUM values are best guesses - it is
# your responsibility to verify that the values are complete and correct.
#
# The following license files were not able to be identified and are
# represented as "Unknown" below, you will need to check them yourself:
#   LICENSE
LICENSE = "Unknown"
LIC_FILES_CHKSUM = "file://LICENSE;md5=1ebbd3e34237af26da5dc08a4e440464"

SRC_URI = "git://github.com/ma7moud111/qt6-tictactoe.git;protocol=https;branch=main"

#SRC_URI +="file://tictactoe.service"

DEPENDS:append = " qtbase qtquick3d qttools cups qtdeclarative-native qtwayland"
RDEPENDS:${PN} = " qtbase qtquick3d qttools cups qtwayland qtdeclarative"

# Modify these as desired
PV = "1.0+git${SRCPV}"
SRCREV = "f0926e57d2acec6790f061c637f504f75a8b6956"

S = "${WORKDIR}/git"

# NOTE: unable to map the following CMake package dependencies: Qt6
inherit qt6-cmake

# Specify any options you want to pass to cmake using EXTRA_OECMAKE:
EXTRA_OECMAKE = ""


