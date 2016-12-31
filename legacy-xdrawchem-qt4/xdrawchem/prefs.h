// prefs.h -- static Preferences class

#ifndef PREFS_H
#define PREFS_H

#include <QTextStream>
#include <QFont>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QFile>
#include <QDir>

#include "defs.h"

class Preferences
{
public:
    Preferences() { Defaults(); }

    void Defaults()
    {
        fixed_arrow = true;
        arrow_fixedlength = 50.0;
        arrow_fixedangle = 15.0;
        fixed_bond = true;
        bond_fixedlength = 25.0;
        bond_fixedangle = 15.0;
        dbond_offset = 2.0;
        fix_hydrogens = true;
        paper_size = PAGE_LETTER;  // perhaps change for Europe/Japan release :)
        paper_orient = PAGE_PORTRAIT;
        paper_width = 750;
        paper_height = 1000;
        units = PIXELS;
        gridmode = GRID_NONE;
        drawgrid = GRID_NONE;
        snapgrid = GRID_NONE;
        gridspace = 25;
        dyk = true;
        dykcount = 0;
        zoom = 100;
        mainFont = QFont( "Helvetica", 10 );
        rulerFont = QFont( "Courier", 8 );
        bondcenter = false;
    }

    void setFile( QString fn, bool fb )
    {
        fileName = fn;
        fallback = fb;
    }

    void setSaveFile( QString fn )
    {
        saveFileName = fn;
    }

    bool LoadPrefs()
    {
        QFile fin( fileName );

        if ( fin.open(QIODevice::ReadOnly ) == false )
        return false;

        QTextStream tin( &fin );
        QString line;
        do {
            line = tin.readLine();
            //qDebug() << "PREFS:" << line ;
            if (line.toUpper().contains("FIXED_ARROW") > 0) {
                if (line.toUpper().contains("TRUE"))
                    fixed_arrow = true;
                else
                    fixed_arrow = false;
            }
            if (line.toUpper().contains("FIXED_BOND") > 0) {
                if (line.toUpper().contains("TRUE"))
                    fixed_bond = true;
                else
                    fixed_bond = false;
            }
            if (line.toUpper().contains("FIX_HYDROGENS") > 0) {
                if (line.toUpper().contains("TRUE"))
                    fix_hydrogens = true;
                else
                    fix_hydrogens = false;
                }
            if (line.toUpper().contains("DYK_TF") > 0) {
                if (line.toUpper().contains("TRUE"))
                    dyk = true;
                else
                    dyk = false;
            }
            if (line.toUpper().contains("ARROW_LENGTH") > 0) {
                arrow_fixedlength = line.mid(13).toDouble();
            }
            if (line.toUpper().contains("ARROW_ANGLE") > 0) {
                arrow_fixedangle = line.mid(12).toDouble();
            }
            if (line.toUpper().contains("BOND_LENGTH") > 0) {
                bond_fixedlength = line.mid(12).toDouble();
            }
            if (line.toUpper().contains("BOND_ANGLE") > 0) {
                bond_fixedangle = line.mid(11).toDouble();
            }
            if (line.toUpper().contains("DBOND_OFFSET") > 0) {
                dbond_offset = line.mid(13).toDouble();
            }
            if (line.toUpper().contains("PAPER_SIZE") > 0) {
                paper_size = line.mid(11).toInt();
            }
            if (line.toUpper().contains("PAPER_ORIENT") > 0) {
                paper_orient = line.mid(13).toInt();
            }
            if (line.toUpper().contains("PAPER_WIDTH") > 0) {
                paper_width = line.mid(12).toInt();
            }
            if (line.toUpper().contains("PAPER_HEIGHT") > 0) {
                paper_height = line.mid(13).toInt();
            }
            if (line.toUpper().contains("UNITS") > 0) {
                units = line.mid(6).toInt();
            }
            if (line.toUpper().contains("DRAWGRID") > 0) {
                drawgrid = line.mid(9).toInt();
            }
            if (line.toUpper().contains("SNAPGRID") > 0) {
                snapgrid = line.mid(9).toInt();
            }
            if (line.toUpper().contains("DYKCOUNT") > 0) {
                dykcount = line.mid(9).toInt();
            }
            if (line.toUpper().contains("GRIDMODE") > 0) {
                gridmode = line.mid(9).toInt();
            }
            if (line.toUpper().contains("GRIDSPACE") > 0) {
                gridspace = line.mid(10).toInt();
            }
            if (line.toUpper().contains("MAINFONT") > 0) {
                mainFont.fromString( line.mid(9) );
            }
            if (line.toUpper().contains("RULERFONT") > 0) {
                rulerFont.fromString( line.mid(10) );
            }
        } while (!tin.atEnd());

        fin.close();

        return true;
    }

    bool SavePrefs()
    {
        QFile fout(saveFileName);

        if ( fout.open( QIODevice::WriteOnly ) == false )
        return false;

        QTextStream tout(&fout);
        if (fixed_arrow)
            tout << "FIXED_ARROW true" << endl;
        else
            tout << "FIXED_ARROW false" << endl;

        if (fixed_bond)
            tout << "FIXED_BOND true" << endl;
        else
            tout << "FIXED_BOND false" << endl;

        if (fix_hydrogens)
            tout << "FIX_HYDROGENS true" << endl;
        else
            tout << "FIX_HYDROGENS false" << endl;

        if (dyk)
            tout << "DYK_TF true" << endl;
        else
            tout << "DYK_TF false" << endl;

        tout << "DYKCOUNT " << dykcount << endl;
        tout << "ARROW_LENGTH " << arrow_fixedlength << endl;
        tout << "ARROW_ANGLE " << arrow_fixedangle << endl;
        tout << "BOND_LENGTH " << bond_fixedlength << endl;
        tout << "BOND_ANGLE " << bond_fixedangle << endl;
        tout << "DBOND_OFFSET " << dbond_offset << endl;
        tout << "PAPER_SIZE " << paper_size << endl;
        tout << "PAPER_ORIENT " << paper_orient << endl;
        tout << "PAPER_WIDTH " << paper_width << endl;
        tout << "PAPER_HEIGHT " << paper_height << endl;
        tout << "UNITS " << units << endl;
        tout << "DRAWGRID " << drawgrid << endl;
        tout << "SNAPGRID " << snapgrid << endl;
        tout << "GRIDMODE " << gridmode << endl;
        tout << "GRIDSPACE " << gridspace << endl;
        tout << "MAINFONT " << mainFont.toString() << endl;
        tout << "RULERFONT " << rulerFont.toString() << endl;

        fout.close();

        return true;
    }

    bool getArrow_fixed() { return fixed_arrow; }
    double getArrow_fixedlength() { return arrow_fixedlength; }
    double getArrow_fixedangle() { return arrow_fixedangle; }
    bool getBond_fixed() { return fixed_bond; }
    double getBond_fixedlength() { return bond_fixedlength; }
    double getBond_fixedangle() { return bond_fixedangle; }
    bool getAntialiased() { return antialiased; }
    bool getFixHydrogens() { return fix_hydrogens; }
    int getPageSize() { return paper_size; }
    int getPageOrientation() { return paper_orient; }
    void setPageSize( int n1 ) { paper_size = n1; }
    void setPageOrientation( int n1 ) { paper_orient = n1; }
    void setFixHydrogens( bool n1 ) { fix_hydrogens = n1; }
    void setArrow_fixed( bool n1 ) { fixed_arrow = n1; }
    void setArrow_fixedlength( double n1 ) { arrow_fixedlength = n1; }
    void setArrow_fixedangle( double n1 ) { arrow_fixedangle = n1; }
    void setBond_fixed( bool n1 ) { fixed_bond = n1; }
    void setBond_fixedlength( double n1 ) { bond_fixedlength = n1; }
    void setBond_fixedangle( double n1 ) { bond_fixedangle = n1; }
    void setAntialiased( bool n1 ) { antialiased = n1; }
    double getDoubleBondOffset() { return dbond_offset; }
    void setDoubleBondOffset( double n1 ) { dbond_offset = n1; }
    int getPageWidth() { return paper_width; }
    void setPageWidth( int n1 ) { paper_width = n1; }
    int getPageHeight() { return paper_height; }
    void setPageHeight( int n1 ) { paper_height = n1; }
    int getUnits() { return units; }
    void setUnits( int n1 ) { units = n1; }
    int getGridMode() { return gridmode; }
    void setGridMode( int n1 ) { gridmode = n1; }
    int getDrawGrid() { return drawgrid; }
    void setDrawGrid( int n1 ) { drawgrid = n1; }
    int getSnapGrid() { return snapgrid; }
    void setSnapGrid( int n1 ) { snapgrid = n1; }
    int getGridSpacing() { return gridspace; }
    void setGridSpacing( int n1 ) { gridspace = n1; }
    bool getDYK() { return dyk; }
    void setDYK( bool n1 ) { dyk = n1; }
    int getDYKCount() { return dykcount; }
    void setDYKCount( int n1 ) { dykcount = n1; }
    int getZoom() { return zoom; }
    void setZoom( int z1 ) { zoom = z1; }
    QFont getMainFont() { return mainFont; }
    void setMainFont( QFont ff1 ) { mainFont = ff1; }
    QFont getRulerFont() { return rulerFont; }
    void setRulerFont( QFont ff1 ) { rulerFont = ff1; }

    void setCustomRingDir(QString d1)
    {
        QDir dir1(d1);
        if (dir1.exists()) {
            customRingDir = d1;
            return;
        } else {
            dir1.mkdir( d1 );
            return;
        }
    }

    QString getCustomRingDir() { return customRingDir; }
    void setBondCenter( bool bc1 ) { bondcenter = bc1; }
    bool getBondCenter() { return bondcenter; }

private:
    bool fixed_arrow, fixed_bond, fix_hydrogens;
    bool antialiased;
    bool fallback; // true = file from library directory
    bool dyk;  // true = show Did You Know? dialog
    int dykcount;  // which Did You Know? tip to show
    QString fileName;  // prefs file name
    QString saveFileName;  // save file name
    QString customRingDir;  // custom ring directory
    double arrow_fixedlength;
    double arrow_fixedangle;
    double bond_fixedlength;
    double bond_fixedangle;
    double dbond_offset;
    int paper_size, paper_orient, paper_dpi;
    int paper_width, paper_height;  // ALWAYS in pixels - convert as needed.
    int units;  // PIXELS, ENGLISH, METRIC - see defs.h
    int gridmode, gridspace, drawgrid, snapgrid;
    int zoom;  // not saved.
    QFont mainFont, rulerFont;  // added at user request
    bool bondcenter;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
