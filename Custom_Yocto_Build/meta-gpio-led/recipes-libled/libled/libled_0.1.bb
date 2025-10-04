SUMMARY = "led library shared"
DESCRIPTION = "A recipe for GPIO LED shared library with gpioled application"
LICENSE = "CLOSED"

SRC_URI = "file://led.cpp \
           file://led.h"

S = "${WORKDIR}"

do_compile() {
    ${CXX} ${CXXFLAGS} ${LDFLAGS} -fPIC -shared \
        -Wl,-soname,libled.so.0 \
        ${WORKDIR}/led.cpp -I${WORKDIR} \
        -o ${B}/libled.so.0.1

    ln -sf libled.so.0.1 ${B}/libled.so.0
    ln -sf libled.so.0 ${B}/libled.so
}

do_install() {
    install -d ${D}${libdir}
    install -m 0755 ${B}/libled.so.0.1 ${D}${libdir}/
    ln -sf libled.so.0.1 ${D}${libdir}/libled.so.0
    ln -sf libled.so.0 ${D}${libdir}/libled.so

    install -d ${D}${includedir}
    install -m 0644 ${WORKDIR}/led.h ${D}${includedir}/
}

FILES:${PN} = "${libdir}/libled.so.*"
FILES:${PN}-dev = "${libdir}/libled.so ${includedir}/led.h"
