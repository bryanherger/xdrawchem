#include <QList>

#include "tool_2d3d.h"
#include "render2d.h"
#include "helpwindow.h"
#include "defs.h"

#include "ioiface.h"

using namespace OpenBabel;

// defined in main.cpp
//extern QString RingDir;

Tool_2D3D::Tool_2D3D( QWidget *parent )
    : ToolDialog( parent )
{

}

void Tool_2D3D::process()
{
    new3dmol = new Molecule( 0 );
    // deep copy into new3dmol
    QList < Drawable * >objs = this_mol->AllObjects();

    QList < DPoint * >oldPoints;
    QList < DPoint * >newPoints;
    DPoint *n, *tmp_pt;
    Bond *b;
    Drawable *tmp_draw;

    // need to deep copy stuff coming off the Clipboard
    // first, find all unique DPoint's
    foreach ( tmp_draw, objs ) {
        if ( oldPoints.contains( tmp_draw->Start() ) == 0 )
            oldPoints.append( tmp_draw->Start() );
        if ( tmp_draw->End() != 0 ) {
            if ( oldPoints.contains( tmp_draw->End() ) == 0 )
                oldPoints.append( tmp_draw->End() );
        }
    }
    if ( oldPoints.count() == 0 )
        return;

    // make new DPoint's which correspond to old DPoint's
    foreach ( tmp_pt, oldPoints ) {
        n = new DPoint( tmp_pt );
        newPoints.append( n );
    }
    // now add all TYPE_BOND objects back to current
    // no need to copy TEXT or SYMBOL
    Drawable *td1;

    foreach ( td1, objs ) {
        if ( td1->Type() == TYPE_BOND ) {
            b = ( Bond * ) td1;
            new3dmol->addBond( newPoints.at( oldPoints.indexOf( td1->Start() ) ), newPoints.at( oldPoints.indexOf( td1->End() ) ), b->Thick(), b->Order(), td1->GetColor(), true );
        }
    }

    new3dmol->Make3DVersion();
    save3DFile();
}

void Tool_2D3D::save3DFile()
{
    /* obsolete, and not even used, IIRC
       QStringList filters;
       io_type inFileType = UNDEFINED;
       int i;
       OBFileFormat fileFormat;
       QString str1, filefilter, filename;
       char *formatOptions;

       str1 = tr("--Select a filter-- (*)");
       filters.append(str1);
       for (i = 0; i < extab.Count(); i++) {
       str1 = "";
       if (extab.IsReadable(i)) {
       str1.append(extab.GetExtension(i));
       str1.append(": ");
       str1.append(extab.GetDescription(i));
       str1.append(" (*)");
       filters.append(str1);
       }
       }
       filters.sort();

       QFileDialog fd(QString::null, QString::null, 0, 0, TRUE);
       fd.setWindowTitle( tr("Save file as...") );
       fd.setMode(QFileDialog::AnyFile);
       fd.setFilters(filters);
       if ( fd.exec() == QDialog::Accepted ) {
       str1 = fd.selectedFilter();
       if (str1.left(2) == "--") {
       QMessageBox::warning( 0, tr("Could not determine file type"),
       tr("Please select a file type from the list.") );
       return;
       }
       filefilter = str1; // so we know what file type to save as
       filename = fd.selectedFile();

       int cutpt = str1.indexOf(':');
       str1.truncate(cutpt);
       const char *tmpchar = str1.toLatin1();
       char *realchar;
       realchar = (char*)malloc(sizeof(char)*str1.length());
       strcpy(realchar, tmpchar);
       inFileType = extab.FilenameToType( realchar );
       OBMol *mol = new3dmol->convertToOBMol();
       mol->SetInputType(inFileType);
       mol->SetOutputType(inFileType);
       ofstream outFileStream( filename.toLatin1() );
       if (!outFileStream)
       {
       QMessageBox::warning( 0, tr("Problem while writing the file"),
       tr("Cannot write the specified file.") );
       return;
       }
       fileFormat.WriteMolecule(outFileStream, *mol, "3D", formatOptions);
       delete new3dmol;
       }
     */
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
