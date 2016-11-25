project(xdrawchem)

set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

set(CMAKE_VERBOSE_MAKEFILE ON)
set( CMAKE_COLOR_MAKEFILE ON )

SET(QT_USE_QT3SUPPORT TRUE)
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)

find_package( Qt4 REQUIRED )
find_package( Openbabel REQUIRED )

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

add_subdirectory( xdrawchem )
add_subdirectory( ring )
add_subdirectory( translation )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${LIBOPENBABEL_INCLUDE_DIR} ${QT_INCLUDES}  )

########### install files ###############




#original Makefile.am contents follow:

### Makefile.am -- Process this file with automake to produce Makefile.in
#
### SUBDIRS lists the directories that make will descend into.
### Note: this list needs to be synchronized with "configure.in"
#
#dist-hook:
#	-perl automoc
#
#SUBDIRS = @MY_SUBDIRS@
#
### EXTRA_DIST contains the files automake does not know about, that need to go
### in the distribution. Note that scripts are not distributed by default.
#
#EXTRA_DIST =
#
#AUTOMAKE_OPTIONS = foreign
###AUTOMAKE_OPTIONS = foreign dist-bzip2
#
#install-data-local: $(EXTRA_DIST)
#	@$(NORMAL_INSTALL)
#	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
#	@list='$(EXTRA_DIST)'; for p in $$list; do \
#	  dir=`echo "$$p" | sed -e 's,/[^/]*$$,,'`; \
#	  if test "$$dir" != "$$p" && test "$$dir" != "."; then \
#	    $(mkinstalldirs) "$(DESTDIR)$(pkgdatadir)/$$dir"; \
#	  fi; \
#	  f="`echo $$p|sed '$(transform)'`"; \
#	  if test -f $$p; then \
#	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(pkgdatadir)/$$f"; \
#	    $(INSTALL_DATA) $$p $(DESTDIR)$(pkgdatadir)/$$f; \
#	  elif test -f $(srcdir)/$$p; then \
#	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(pkgdatadir)/$$f"; \
#	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(pkgdatadir)/$$f; \
#	  else :; fi; \
#	done
#
#uninstall-local:
#	@$(NORMAL_UNINSTALL)
#	@list='$(EXTRA_DIST)'; for p in $$list; do \
#	  f="`echo $$p|sed '$(transform)'`"; \
#	  echo " rm -f $(DESTDIR)$(pkgdatadir)/$$f"; \
#	  rm -f $(DESTDIR)$(pkgdatadir)/$$f; \
#	done
#
#libtool: $(LIBTOOL_DEPS)
#	$(SHELL) ./config.status --recheck
