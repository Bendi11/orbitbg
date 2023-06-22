pkgname='orbitbg'
pkgver='0.1.0'
pkgrel='1'

pkgdesc="Dynamic material-style solar system background for X11"
arch=('any')
license=('GPL')

build() {
    cmake -B ../build .. \
        -DCMAKE_BUILD_TYPE='Release' \
        -DCMAKE_INSTALL_PREFIX='/usr' \
        -Wno-dev
    cmake --build ../build
}

package() {
    DESTDIR="$pkgdir" cmake --install ../build
}
