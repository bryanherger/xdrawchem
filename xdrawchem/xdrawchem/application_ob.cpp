#include <qfiledialog.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qstatusbar.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <malloc.h>

using std::cout;
using std::endl;

#include "application.h"
#include "chemdata.h"
#include "molecule.h"
#include "dpoint.h"
#include "bond.h"

#include "ioiface.h"

using namespace std;
using namespace OpenBabel;

// OpenBabel functions.
void ApplicationWindow::OBGetFilters() {
  OBConversion Conv;
  //io_type inFileType = UNDEFINED;
  //int i;
  //OBFileFormat fileFormat;
  QString str1, myext;
  str1 = tr("--Select a filter-- (*)");
  readFilters.append(str1);
  writeFilters.append(str1);
  str1 = tr("CDXML - ChemDraw text/XML format (*)");
  readFilters.append(str1);
  writeFilters.append(str1);
  str1 = tr("CDX - ChemDraw binary format (*)");
  readFilters.append(str1);
  writeFilters.append(str1);
  str1 = tr("XDC - XDrawChem native format (*)");
  readFilters.append(str1);
  writeFilters.append(str1);

  Formatpos pos;
  OBFormat* pFormat;
  const char* str=NULL;
  while(OBConversion::GetNextFormat(pos,str,pFormat))
    {
      //std::cout << "  " << str << std::endl;
      if((pFormat->Flags() & NOTWRITABLE) && (pFormat->Flags() & NOTREADABLE))
	continue;
      str1 = str;
      myext = str1.left( str1.find(" -- ") );
      str1.append("(*.");
      str1.append(myext);
      str1.append(")");
      if ( (pFormat->Flags() & NOTREADABLE) == 0 )
	readFilters.append(str1);
      if ( (pFormat->Flags() & NOTWRITABLE) == 0 )
	writeFilters.append(str1);
    }
  
  filters.sort();
  readFilters.sort();
  writeFilters.sort();
}

void ApplicationWindow::OBImport() {
  QFileDialog fd(QString::null, QString::null, 0, 0, TRUE);
  fd.setCaption( tr("Open file...") );
  fd.setMode(QFileDialog::ExistingFile);
  fd.setFilters(readFilters);  
  if ( fd.exec() == QDialog::Accepted ) {
    OBNewLoad(fd.selectedFile(), fd.selectedFilter());
  }
}

void ApplicationWindow::OBNewLoad(QString infile, QString infilter) {
  //io_type inFileType = UNDEFINED;
  ///OBFileFormat fileFormat;

  OBConversion Conv;
  OBFormat *inFormat = NULL;

  if (infilter.left(2) == "--") {
    inFormat = Conv.FormatFromExt( infile.latin1() );
    if (inFormat == NULL) {
      QMessageBox::warning( 0, tr("Could not determine file type"),
			    tr("Please select a file type from the list.") );
      return;
    }
  }
  filename = infile;
  filefilter = infilter;

  /* handled internally */
  if ( (infilter.left(3) == "XDC") || (infilter.left(3) == "CDX") ) {
    QString realFileName = filename;
      
    QFile f(filename);
    if ( !f.open(IO_ReadOnly) ) {
      QMessageBox::warning(0, tr("Couldn't open file"), tr("Could not open the file: ") + filename);
      statusBar()->message(tr("Unable to load ") + filename);
      return;
    }
    f.close();
    SelectAll();
    Clear();

    if ( !c->load(filename) ) {
      statusBar()->message(tr("Unable to load ") + filename);
      return;
    }
  
    filename = realFileName;
    setCaption( QString(XDC_VERSION) + QString(" - ") + filename );
    statusBar()->message( tr("Loaded document ") + filename );
    c->DeselectAll();
    r->repaint(false);      
    return;
  }
  /* end XDC/CDX code */
  /* rest done by OpenBabel */

  int cutpt = infilter.find(" -- ");
  infilter.truncate(cutpt);
  const char *tmpchar = infilter.latin1();
  char *realchar;
  realchar = (char*)malloc(sizeof(char)*infilter.length());
  strcpy(realchar, tmpchar);
  if (inFormat == NULL)
    inFormat = Conv.FindFormat( realchar );
  OBMol *mol = new OBMol;
  ifstream inFileStream( infile.latin1() );
  if (!inFileStream) {
    QMessageBox::warning( 0, tr("Problem while opening the file"),
			  tr("Cannot open the specified file.") );
    return;
  }
  Conv.SetInAndOutFormats(inFormat,inFormat);
  Conv.Read(mol, &inFileStream);
  SelectAll();
  Clear();
  IOIface converter(c, mol);
  converter.convertToChemData();
  c->ScaleAll();
  c->DeselectAll();
  r->repaint(false);      
  r->setMode_Select();
  setCaption( QString(XDC_VERSION) + QString(" - ") + filename );
  statusBar()->message( tr("Loaded document ") + filename );
}

void ApplicationWindow::OBExport() {
  cout << "OBExport()" << endl;
  //io_type inFileType = UNDEFINED;
  int i;
  //OBFileFormat fileFormat;
  //char *formatOptions;
  QFile ftest;

  QFileDialog fd(QString::null, QString::null, 0, 0, TRUE);
  QString str1;
  fd.setCaption( tr("Save file as...") );
  fd.setMode(QFileDialog::AnyFile);
  fd.setFilters(writeFilters);  
  if ( fd.exec() == QDialog::Accepted ) {
    if (ftest.exists(fd.selectedFile())) {
      i = QMessageBox::warning(this, tr("Overwrite file?"), tr("Overwrite existing file: %1 ?").arg(fd.selectedFile()), QMessageBox::Ok, QMessageBox::Cancel);
      if (i == QMessageBox::Cancel) return;
    }
    str1 = fd.selectedFilter();
    filefilter = str1; // so we know what file type to save as
    filename = fd.selectedFile();
    OBNewSave();
  }
}

void ApplicationWindow::OBNewSave() {
  c->StartUndo();  // needed in case we scale before saving

  OBConversion Conv;
  OBFormat *outFormat = NULL;

  if (filefilter.left(2) == "--") {
    outFormat = Conv.FormatFromExt( filefilter.latin1() );
    if (outFormat == NULL) {
      QMessageBox::warning( 0, tr("Could not determine file type"),
			    tr("Please select a file type from the list.") );
      return;
    }
  }
  
  QString str1;
  //io_type inFileType = UNDEFINED;
  //int i;
  //OBFileFormat fileFormat;
  //char *formatOptions;
  str1 = filefilter;

  if (str1.left(3) == "XDC") {
    if (filename.right(4).lower() != ".xdc")
      filename.append(".xdc");
    if ( !c->save(filename) ) {
      statusBar()->message( tr("Could not write to ") + filename );
      return;
    }
    setCaption( QString(XDC_VERSION) + QString(" - ") + filename );    
    statusBar()->message( tr( "Saved file ") + filename );
    return;
  }
  if (str1.left(3) == "CDX") {
    if (filename.right(6).lower() != ".cdxml")
      filename.append(".cdxml");
    if ( !c->save(filename) ) {
      statusBar()->message( tr("Could not write to ") + filename );
      return;
    }
    setCaption( QString(XDC_VERSION) + QString(" - ") + filename );    
    statusBar()->message( tr( "Saved file ") + filename );
    return;
  }
  int cutpt = str1.find(" -- ");
  str1.truncate(cutpt);
  const char *tmpchar = str1.latin1();
  char *realchar;
  realchar = (char*)malloc(sizeof(char)*str1.length());
  strcpy(realchar, tmpchar);
  if (outFormat == NULL)
    outFormat = Conv.FindFormat( realchar );
  OBMol *mol = new OBMol;
  Conv.SetInAndOutFormats(outFormat,outFormat);

  // invert so save file has correct coordinates
  //r->SelectAll();
  //r->Flip(FLIP_V);
  //r->DeselectAll();

  IOIface converter(c, mol);
  if ( !converter.convertToOBMol() ) {
    QMessageBox::warning( 0, tr("IOIface Data error"),
			  tr("Cannot convert the molecule.\nConversion failed in ioiface.cpp") );
    return;
  }
  ofstream outFileStream( filename.latin1() );
  if (!outFileStream)
  {
  QMessageBox::warning( 0, tr("Problem while writing the file"),
  tr("Cannot write the specified file.") );
  return;
  }
  
  Conv.Write(mol, &outFileStream);
  //fileFormat.WriteMolecule(outFileStream, *mol, "3D", formatOptions);
  setCaption( QString(XDC_VERSION) + QString(" - ") + filename );
  
  statusBar()->message( tr( "Saved file ") + filename );
  //c->Undo();
}

QString ApplicationWindow::OBGuessFileType(QString guessname) {
  QString guesstype = "--Select a filter-- (*)";
  QString guess_lower = guessname.lower();

  if (guess_lower.right(4) == ".xdc") {
    guesstype = "XDC - XDrawChem native format (*)";
    return guesstype;
  }
  if (guess_lower.right(6) == ".cdxml") {
    guesstype = "CDXML - ChemDraw text/XML format (*)";
    return guesstype;
  }
  if (guess_lower.right(4) == ".cdx") {
    guesstype = "CDX - ChemDraw binary format (*)";
    return guesstype;
  }

  QString dos_ext, ext_test;
  for (int cc = guess_lower.length() - 1; cc > 0; cc--) {
    if (guess_lower.at(cc) == '.')
      break;
    dos_ext.prepend(guess_lower.at(cc));
  }

  for ( QStringList::Iterator it = filters.begin(); 
	it != filters.end(); ++it ) {
    ext_test = *it;
    ext_test = ext_test.left(ext_test.find(":"));
    if (dos_ext == ext_test) {
      guesstype = *it;
      break;
    }
  }

  return guesstype;
}
