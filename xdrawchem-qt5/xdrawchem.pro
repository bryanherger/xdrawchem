TEMPLATE = app
TARGET = xdrawchem

# edit to match your OB install
exists(/usr/include/openbabel-2.0/openbabel/mol.h) {
INCLUDEPATH += /usr/include/openbabel-2.0
LIBS += -lopenbabel
}
exists(/usr/local/include/openbabel-2.0/openbabel/mol.h) {
INCLUDEPATH += /usr/local/include/openbabel-2.0
LIBS += -lopenbabel
}
exists(/Developer/openbabel-2.4.1/include/openbabel/mol.h) {
INCLUDEPATH += /Developer/openbabel-2.4.1/include
LIBS += -L/Developer/openbabel-2.4.1/lib/Debug -lopenbabel
}
# exit if openbabel wasn't found
contains( LIBS, -lopenbabel ) {
} else {
error(OpenBabel was not found.  Edit xdrawchem.pro and add valid include and lib values)
# if you get this error, comment the above with #, uncomment the variables below, and set
# the paths to match your OpenBabel install
#INCLUDEPATH += /usr/local/include/openbabel-2.0
#LIBS += -lopenbabel
}
# set PREFIX if not set (it is set when building RPM)
isEmpty(PREFIX) {
PREFIX = /usr/local
}

DESTDIR = bin
target.path = $${PREFIX}/$${DESTDIR}
INSTALLS = target

MOC_DIR = .moc
UI_DIR = .ui
OBJECTS_DIR = .obj

# for includes and libs
DEFINES += UNIX
DEFINES += RINGHOME=\\\"$${PREFIX}/share/xdrawchem\\\"
DEFINES += XDC_SERVER=\\\"http://www.woodsidelabs.com/chemistry\\\"
ringdir.path = $${PREFIX}/share/xdrawchem
ringdir.files = ring/*
doc.path = $${PREFIX}/share/xdrawchem/doc
doc.files = doc/*
INSTALLS += ringdir doc
QMAKE_CXXFLAGS_DEBUG += -O0 -g3 -g -rdynamic

CONFIG += debug
QT += xml network core widgets printsupport network gui

HEADERS = xdrawchem/aa_xpm.h \
          xdrawchem/application.h \
          xdrawchem/arrow.h \
          xdrawchem/arrows.h \
          xdrawchem/atom.h \
          xdrawchem/biotools.h \
          xdrawchem/bondedit.h \
          xdrawchem/bond.h \
          xdrawchem/boshcp.h \
          xdrawchem/bracket.h \
          xdrawchem/brackets.h \
          xdrawchem/CDXConstants.h \
          xdrawchem/cdxml_reader.h \
          xdrawchem/charsel.h \
          xdrawchem/chemdata.h \
          xdrawchem/colorbutton.h \
          xdrawchem/clipboard.h \
          xdrawchem/cml.h \
          xdrawchem/crings_dialog.h \
          xdrawchem/cubicbezier.h \
          xdrawchem/curvearrow.h \
          xdrawchem/defs.h \
          xdrawchem/dpoint.h \
          xdrawchem/drawable.h \
          xdrawchem/dyk.h \
          xdrawchem/fixeddialog.h \
          xdrawchem/gobject.h \
          xdrawchem/graphdata.h \
          xdrawchem/graphdialog.h \
          xdrawchem/graphwidget.h \
          xdrawchem/helpwindow.h \
          xdrawchem/http.h \
          xdrawchem/ioiface.h \
          xdrawchem/justifytools.h \
          xdrawchem/lines.h \
          xdrawchem/magnifytools.h \
          xdrawchem/moldata.h \
          xdrawchem/molecule.h \
          xdrawchem/molecule_sssr.h \
          xdrawchem/molinfodialog.h \
          xdrawchem/myfiledialog.h \
          xdrawchem/na_xpm.h \
          xdrawchem/netaccess.h \
          xdrawchem/netchoosedialog.h \
          xdrawchem/netdialog.h \
          xdrawchem/ngw.h \
          xdrawchem/pagesetupdialog.h \
          xdrawchem/paintable.h \
          xdrawchem/peak.h \
          xdrawchem/peptidebuilder.h \
          xdrawchem/prefs.h \
          xdrawchem/previewwidget.h \
          xdrawchem/render2d.h \
          xdrawchem/renderarea.h \
          xdrawchem/ringdialog.h \
          xdrawchem/ring.h \
          xdrawchem/rings.h \
          xdrawchem/ring_xpm.h \
          xdrawchem/sdg.h \
          xdrawchem/setofrings.h \
          xdrawchem/smilesdialog.h \
          xdrawchem/sorf.h \
          xdrawchem/sugar_xpm.h \
          xdrawchem/symbol.h \
          xdrawchem/symbol_xpm.h \
          xdrawchem/text.h \
          xdrawchem/textshapedialog.h \
          xdrawchem/tool_13c_nmr.h \
          xdrawchem/tool_1h_nmr.h \
          xdrawchem/tool_2d3d.h \
          xdrawchem/tooldialog.h \
          xdrawchem/tool_ir.h \
          xdrawchem/xdc_event.h \
          xdrawchem/xdc_textedit.h \
          xdrawchem/xdc_toolbutton.h \
          xdrawchem/xml_cml.h \
          xdrawchem/xml_reader.h \
          xdrawchem/xruler.h \
          xdrawchem/vector2D.h

SOURCES = xdrawchem/application.cpp \
          xdrawchem/application_ob.cpp \
          xdrawchem/application_ring.cpp \
          xdrawchem/arrow.cpp \
          xdrawchem/biotools.cpp \
          xdrawchem/bond.cpp \
          xdrawchem/bondedit.cpp \
          xdrawchem/boshcp.cpp \
          xdrawchem/bracket.cpp \
          xdrawchem/cdx2cdxml.cpp \
          xdrawchem/cdxml_reader.cpp \
          xdrawchem/charsel.cpp \
          xdrawchem/chemdata_cdx.cpp \
          xdrawchem/chemdata_cdxml.cpp \
          xdrawchem/chemdata_cml.cpp \
          xdrawchem/chemdata.cpp \
          xdrawchem/chemdata_edit.cpp \
          xdrawchem/chemdata_event.cpp \
          xdrawchem/chemdata_mdl.cpp \
          xdrawchem/chemdata_rw.cpp \
          xdrawchem/chemdata_rxn.cpp \
          xdrawchem/chemdata_tools.cpp \
          xdrawchem/chemdata_xdc.cpp \
          xdrawchem/chemdata_xml.cpp \
          xdrawchem/colorbutton.cpp \
          xdrawchem/crings_dialog.cpp \
          xdrawchem/curvearrow.cpp \
          xdrawchem/drawable.cpp \
          xdrawchem/dyk.cpp \
          xdrawchem/fixeddialog.cpp \
          xdrawchem/gobject.cpp \
          xdrawchem/graphdialog.cpp \
          xdrawchem/graphwidget.cpp \
          xdrawchem/helpwindow.cpp \
          xdrawchem/http.cpp \
          xdrawchem/ioiface.cpp \
          xdrawchem/main.cpp \
          xdrawchem/molecule_1h_nmr.cpp \
          xdrawchem/molecule.cpp \
          xdrawchem/molecule_obmol.cpp \
          xdrawchem/molecule_smiles.cpp \
          xdrawchem/molecule_sssr.cpp \
          xdrawchem/molecule_tools_1.cpp \
          xdrawchem/molecule_tools_2.cpp \
          xdrawchem/molinfodialog.cpp \
          xdrawchem/myfiledialog.cpp \
          xdrawchem/netaccess.cpp \
          xdrawchem/netchoosedialog.cpp \
          xdrawchem/netdialog.cpp \
          xdrawchem/ngw.cpp \
          xdrawchem/pagesetupdialog.cpp \
          xdrawchem/peptidebuilder.cpp \
          xdrawchem/previewwidget.cpp \
          xdrawchem/render2d.cpp \
          xdrawchem/render2d_draw.cpp \
          xdrawchem/render2d_event.cpp \
          xdrawchem/render2d_print.cpp \
          xdrawchem/render2d_select.cpp \
          xdrawchem/render2d_text.cpp \
          xdrawchem/renderarea.cpp \
          xdrawchem/retro.cpp \
          xdrawchem/ringdialog.cpp \
          xdrawchem/smilesdialog.cpp \
          xdrawchem/symbol.cpp \
          xdrawchem/text.cpp \
          xdrawchem/textshapedialog.cpp \
          xdrawchem/to3d.cpp \
          xdrawchem/tool_13c_nmr.cpp \
          xdrawchem/tool_1h_nmr.cpp \
          xdrawchem/tool_2d3d.cpp \
          xdrawchem/tooldialog.cpp \
          xdrawchem/tool_ir.cpp \
          xdrawchem/xdc_toolbutton.cpp \
          xdrawchem/xml_cml.cpp \
          xdrawchem/xml_reader.cpp \
          xdrawchem/xruler.cpp \
          xdrawchem/vector2D.cpp

TRANSLATIONS = translation/xdrawchem_da.ts \
	translation/xdrawchem_dk.ts \
	translation/xdrawchem_es.ts \
	translation/xdrawchem_it.ts \
	translation/xdrawchem_nl.ts \
	translation/xdrawchem_pt_BR.ts \
	translation/xdrawchem_de.ts \
	translation/xdrawchem_en.ts \
	translation/xdrawchem_fr.ts \
	translation/xdrawchem_jp.ts \
	translation/xdrawchem_pl.ts \
	translation/xdrawchem_pt.ts

