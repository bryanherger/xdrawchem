INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${LIBOPENBABEL_INCLUDE_DIR} ${QT_INCLUDES}  )


add_definitions( ${QT_DEFINITIONS} -DQT3_SUPPORT -DQT_NO_STL -D_REENTRANT -DQT3_SUPPORT_WARNINGS -DRINGHOME='"/usr/local"' -DUNIX )

########### next target ###############

QT4_GENERATE_MOC(application.h application.moc)
QT4_GENERATE_MOC(arrow.h arrow.moc)
QT4_GENERATE_MOC(bondedit.h bondedit.moc)
QT4_GENERATE_MOC(charsel.h charsel.moc)
QT4_GENERATE_MOC(chemdata.h chemdata.moc)
QT4_GENERATE_MOC(crings_dialog.h crings_dialog.moc)
QT4_GENERATE_MOC(drawable.h drawable.moc)
QT4_GENERATE_MOC(dyk.h dyk.moc)
QT4_GENERATE_MOC(fixeddialog.h fixeddialog.moc)
QT4_GENERATE_MOC(graphdialog.h graphdialog.moc)
QT4_GENERATE_MOC(graphwidget.h graphwidget.moc)
QT4_GENERATE_MOC(helpwindow.h helpwindow.moc)
QT4_GENERATE_MOC(molinfodialog.h molinfodialog.moc)
QT4_GENERATE_MOC(netaccess.h netaccess.moc)
QT4_GENERATE_MOC(netchoosedialog.h netchoosedialog.moc)
QT4_GENERATE_MOC(ngw.h ngw.moc)
QT4_GENERATE_MOC(pagesetupdialog.h pagesetupdialog.moc)
QT4_GENERATE_MOC(peptidebuilder.h peptidebuilder.moc)
QT4_GENERATE_MOC(render2d.h render2d.moc)
QT4_GENERATE_MOC(renderarea.h renderarea.moc)
QT4_GENERATE_MOC(ringdialog.h ringdialog.moc)
QT4_GENERATE_MOC(textshapedialog.h textshapedialog.moc)
QT4_GENERATE_MOC(tooldialog.h tooldialog.moc)
QT4_GENERATE_MOC(xruler.h xruler.moc)

SET(xdrawchem_SRCS
application.cpp
application_ring.cpp
arrow.cpp
biotools.cpp
bond.cpp
bondedit.cpp
boshcp.cpp
bracket.cpp
cdx2cdxml.cpp
cdxml_reader.cpp
charsel.cpp
chemdata.cpp
chemdata_cdx.cpp
chemdata_cdxml.cpp
chemdata_cml.cpp
chemdata_edit.cpp
chemdata_mdl.cpp
chemdata_rw.cpp
chemdata_rxn.cpp
chemdata_tools.cpp
chemdata_xdc.cpp
chemdata_xml.cpp
crings_dialog.cpp
curvearrow.cpp
drawable.cpp
fixeddialog.cpp
graphdialog.cpp
graphwidget.cpp
helpwindow.cpp
main.cpp
molecule.cpp
molecule_1h_nmr.cpp
molecule_smiles.cpp
molecule_sssr.cpp
molecule_tools_1.cpp
molecule_tools_2.cpp
molinfodialog.cpp
myfiledialog.cpp
netaccess.cpp
netchoosedialog.cpp
netdialog.cpp
pagesetupdialog.cpp
peptidebuilder.cpp
previewwidget.cpp
render2d.cpp
render2d_draw.cpp
render2d_event.cpp
render2d_print.cpp
render2d_select.cpp
render2d_text.cpp
renderarea.cpp
ringdialog.cpp
smilesdialog.cpp
symbol.cpp
text.cpp
textshapedialog.cpp
to3d.cpp
xml_cml.cpp
xml_reader.cpp
ioiface.cpp
application_ob.cpp
chemdata_event.cpp
molecule_obmol.cpp
tooldialog.cpp
ngw.cpp
tool_1h_nmr.cpp
tool_13c_nmr.cpp
tool_ir.cpp
gobject.cpp
tool_2d3d.cpp
xruler.cpp
dyk.cpp
retro.cpp
)

#KDE4_AUTOMOC(${xdrawchem_SRCS})

ADD_EXECUTABLE(xdrawchem ${xdrawchem_SRCS})

TARGET_LINK_LIBRARIES(xdrawchem
${QT_QTCORE_LIBRARY}
 ${QT_QTGUI_LIBRARY}
 ${QT_QTNETWORK_LIBRARY}
 ${QT_QT3SUPPORT_LIBRARY}
${QT_QTXML_LIBRARY}
${LIBOPENBABEL_LIBRARIES}
)

INSTALL_TARGETS(/bin xdrawchem )


########### install files ###############
