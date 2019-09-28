PKG_NAME        := bash
PKG_VERSION     := 5.0
PKG_URL         := https://ftp.gnu.org/gnu/bash/bash-$(PKG_VERSION).tar.gz
PKG_TARBALL     := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_ARCHIVE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_PREFIX       = /usr/local
PKG_DEPS        := "mlibc"

all:
	[ -f $(PKG_TARBALL)     ] || wget    $(PKG_URL)
	[ -d $(PKG_ARCHIVE_DIR) ] || tar -xf $(PKG_TARBALL)
	cd   $(PKG_ARCHIVE_DIR)   && patch -p1 < ../$(PKG_NAME).patch
	PATH=$(PATH) && export PATH && \
    cd   $(PKG_ARCHIVE_DIR)   && ./configure \
	    --host=i686-phoenux  \
	    --prefix=$(PKG_PREFIX) \
	    --without-bash-malloc \
	    --enable-static-link
	make -C $(PKG_ARCHIVE_DIR) LDFLAGS="-static -L./lib/termcap -O2 -Wl,--allow-multiple-definition"

install:
	make -C $(PKG_ARCHIVE_DIR) DESTDIR=$(DESTDIR) install
