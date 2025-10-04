SUMMARY = "gpio led switch on/off With shared lib"
DESCRIPTION = "Recipe created for toggling a led using a push button"
LICENSE = "CLOSED"

SRC_URI = "file://main.cpp \
           file://gpioled.service \
           file://start-gpioled.sh "

S = "${WORKDIR}"

inherit systemd
SYSTEMD_AUTO_ENABLE = "enable"

DEPENDS = "libled"
RDEPENDS:${PN}:append = " libled bash"

do_compile(){
        ${CXX} ${CXXFLAGS} ${WORKDIR}/main.cpp \
        -I${STAGING_INCDIR} -L${STAGING_LIBDIR} -lled \
        ${LDFLAGS} -o ${B}/gpioled
}

do_install(){
    install -d ${D}${bindir}
    install -m 0755 ${B}/gpioled ${D}${bindir}/gpioled

    # install start gpio script
    install -m 0755 ${WORKDIR}/start-gpioled.sh ${D}${bindir}

    # install the auto enable service
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/gpioled.service ${D}${systemd_system_unitdir}
}

FILES:${PN} += "${bindir}/gpioled"
FILES:${PN} += "${systemd_system_unitdir}/gpioled.service"
SYSTEMD_SERVICE:${PN} = "gpioled.service"

