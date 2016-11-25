// render2d.h -- class definition for XDrawChem render widget.

#ifndef RENDER2D_H
#define RENDER2D_H

#include <QPainterPath>
#include <QTextCharFormat>
#include <QTextEdit>
#include <QProcess>

#include "drawable.h"
#include "paintable.h"
#include "dpoint.h"
#include "prefs.h"
#include "clipboard.h"
#include "xdc_event.h"

class ChemData;
class Text;
class Molecule;

extern Preferences preferences;

class Render2D : public QWidget
{
Q_OBJECT

public:
    void setChemData(ChemData *c1) { c = c1; }
    // defined in render2d.cpp
    Render2D(QWidget *parent = 0);
    void CorrectEndpoint_arrow();
    void CorrectEndpoint_bond();
    void CleanUpAfterTextTool();
    void setClipboard(Clipboard *);
    Clipboard *getClipboard();

public slots:
    // defined in render2d.cpp
    void ReactionAnalysis(int);
    void setMode_Select();
    void setMode_Lasso();
    void setMode_DrawLine();
    void setMode_DrawDashLine();
    void setMode_DrawChain();
    void setMode_DrawUpLine();
    void setMode_DrawDownLine();
    void setMode_DrawWavyLine();
    void setMode_DrawArrow(regularArrowType);
    void setMode_DrawCurveArrow(curveArrowType);
    void setMode_DrawBracket(bracketType);
    void setMode_Erase();
    void setMode_DrawText();
    void setMode_DrawSymbol( const QString & );
    void setMode_DrawGraphicObject(int, int param1 = 0);
    void setMode_DrawRing(QString, QString, int sp = 0);
    void setMode_DrawNewmanAnti();
    void setMode_DrawNewmanGauche();
    void Tool(int);
    void SelectAll();
    void DeselectAll();
    void EraseSelected();
    void Inserted();
    void SetFont(QFont);
    void SetColor(QColor);
    QFont GetFont();
    QColor GetColor();
    void Cut();
    void Copy();
    void Paste();
    void Undo();
    void Rotate90();
    void Rotate180();
    void Rotate270();
    void AutoLayout();
    void Flip(int);
    bool SaveEPS(QString);
    bool SaveSVG(QString);
    void clearAllGroups();
    QPoint zoomCorrect(QPoint);
    QPoint zoomCorrectReverse(QPoint);
    // defined in render2d_draw.cpp
    double getAngle(QPoint, QPoint);
    void drawBezier(QVector<QPoint>, QColor, bool = false, int = 0);
    void drawLine(QPoint, QPoint, int, QColor, int = 0);
    void drawPolyline(QPainterPath, QColor);
    void drawPolyline(QVector<QPoint> p, QColor c1);
    void drawUpLine(QPoint, QPoint, QColor);
    void drawDownLine(QPoint, QPoint, QColor);
    void drawWavyLine(QPoint, QPoint, QColor);
    void drawArrow(QPoint, QPoint, QColor, int, int);
    void drawBracket(QPoint, QPoint, QColor, int, QColor);
    void drawBox(QPoint, QPoint, QColor);
    void drawFillBox(QPoint, QPoint, QColor);
    void drawFillBox(QPoint, QPoint, QColor, bool, QColor, int);
    void drawCircle(QPoint, int, QColor);
    void drawEllipse(QPoint, QPoint, bool, QColor, bool, QColor);
    void drawText(QChar, QPoint, QColor, QFont);
    void drawString(QString, QPoint, QColor, QFont);
    void drawTextReverse(QChar, QPoint, QColor, QFont);
    void drawPixmap(QPoint, QPixmap);
    void drawCurveArrow(QPoint, QPoint, QColor, QString);
    QPixmap MakePixmap(bool transp = false);
    QPixmap MakeFullPixmap();
    // defined in render2d_text.cpp
    void InsertSymbol();
    void DrawText_returnPressed();
    void DrawText_textChanged();
    void DrawText_FinishText();
    void DrawText_CVAchanged(QTextCharFormat::VerticalAlignment);
    QString EditText(QPoint, QString, int, bool);
    QRect GetTextDimensions(QString, QFont);
    int GetTextHeight(QFont);
    int GetTextFullHeight(QFont);
    int GetCharWidth(QChar, QFont);
    int GetStringWidth(QString, QFont);
    void DrawText_mousePressEvent(QMouseEvent *, QPoint);
    void DrawText_mouseReleaseEvent(QMouseEvent *, QPoint);
    void DrawText_mouseMoveEvent(QMouseEvent *);
    void Superscript();
    void Subscript();
    void Bold();
    void Italic();
    void Underline();
    void JustifyLeft();
    void JustifyCenter();
    void JustifyRight();
    // defined in render2d_select.cpp
    void Select_mouseMoveEvent(QMouseEvent *);
    double GetAngle(DPoint *, DPoint *, DPoint *);
    // defined in render2d_print.cpp
    void PrintSetup();
    void Print(QString epsname = "");
    void UpdatePageGeometry();
    // defined in render2d_event.cpp:
    QPoint GridLock(QPoint);
    void createGrid();
    void zoomEvent();
    void molInfo();
    void molSelect();
    void molCut();
    void molCopy();
    void bondEdit();
    void bondInfo();
    void textShape();
    void bracketFill();
    void bracketFillOff();

    // defined here:
    void setDirectDraw(bool dd1) { directdraw = dd1; }
    double DistanceBetween(QPoint a, QPoint b)
    {
        double dx = (double)a.x() - (double)b.x();
        double dy = (double)a.y() - (double)b.y();
        return sqrt(dx * dx + dy * dy);
    }
    QPoint Midpoint(QPoint a, QPoint b)
    {
        int ax = (a.x() + b.x()) / 2;
        int ay = (a.y() + b.y()) / 2;
        return QPoint(ax, ay);
    }
    int RoundOff(double x)
    {
        int x1 = (int)x;
        double x2 = x - (double)x1;
        if (x2 > 0.5)
            return x1 + 1;
        else
            return x1;
    }
    void SetOutputDevice(int x) { outputDevice = x; }
    void SetThick(int t) { thick = t; }
    void resetTextOrigin()
    {
        if (mode > 127) {
            rotateOrigin.setX(0); rotateOrigin.setY(0);
        }
    }
    // now done in prefs.h
    //int getPageSize() { return page_size; }
    //int getPageOrientation() { return page_orientation; }
    void setPageSizeAndOrientation() {
      UpdatePageGeometry();
    }
    void setBGColor(QColor bc1) {
      QPalette palette;
      palette.setColor(backgroundRole(), bc1);
      setPalette(palette);
    }
    QColor getBGColor() { return bgcolor; }
    void setWaitCursor() { setCursor(Qt::WaitCursor); }
    void setArrowCursor() { setCursor(Qt::ArrowCursor); }
    bool getFontKludge() { return font_size_kludge; }

signals:
    void SignalSetStatusBar(QString);
    void SignalHelpTopic(QString);
    void XDCEventSignal(XDC_Event *);
    void textOn( QFont );
    void TextOff();

protected:
    // defined in render2d_event.cpp
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    // defined in render2d_text.cpp
    void keyPressEvent(QKeyEvent *);

private slots:
    void saveEPSFinished( int, QProcess::ExitStatus );

private:
    // Qpixmap of double buffer
    QPixmap dbuffer;
    QPixmap grid;
    // height and width of the drawing area
    int renderHeight, renderWidth;
    // zoom factor (actually held in prefs.h)
    float zoomFactor;
    // cursors
    QCursor skullcursor, rotatecursor;
    // the ChemData object this Render2D is linked with
    ChemData *c;
    // start and end points of object being drawn, or selection box...
    DPoint *startpoint, *endpoint;
    // current drawing font
    QFont currentFont;
    // current drawing color
    QColor currentColor;
    // current background color
    QColor bgcolor;
    // Positions to remember for multiple selection
    QPoint selectOrigin, selectPrev;
    // Rotate and resize handles (used in MODE_SELECT_MULTIPLE_SELECTED)
    QRect selectionBox;
    QPoint rotateHandle, resizeHandle;
    // rotate origin
    QPoint rotateOrigin;
    // previous angle (for rotating)
    double prevangle;
    // previous position (of mouse, usually)
    DPoint *prevpos;
    // highlighted point, if any
    DPoint *highlightpoint;
    // highlighted object, if any
    Drawable *highlightobject;
    // current input/output/drawing mode
    int mode, prev_mode;
    // temporary Bezier curve
    QVector<QPoint> tmp_bezier;
    int bezier_count;
    // current button state
    bool mouse1down, mouse3down, doubleClickFlag;
    bool moved;  // to check validity of points, see render2d_event.cpp
    // direct drawing flag
    bool directdraw;
    // fixed length/angle (now controlled by Preferences class, see prefs.h)
    //bool fixed_bond, fixed_arrow;
    //double fixedlength_bond, fixedangle_bond;
    //double fixedlength_arrow, fixedangle_arrow;
    // current line thickness and old bond order (for edit)
    int thick, savedBondOrder;
    // chain length
    double chainlength;
    // Output device (screen, printer, image)
    int outputDevice;
    // Output file/textstream
    QFile output_file;
    QTextStream output_ts;
    // SVG position correction
    int svg_dx, svg_dy;
    // Output printer and related classes
    QList<Paintable *> paintqueue;
    QPrinter *printer;
    QPainter *painter;
    // Text object, temp string to use when creating/editing text
    Text *localtext;
    QTextEdit *localtexteditor;
    QString localstring;
    bool text_exists;  // does this text object exist?
    bool text_drag, super_set, sub_set;
    DPoint *start_drag;
    // Symbol object, which file to use (or ring, if MODE_RING)
    QString symbolfile;
    int smartplace;  // able to attach ring to a point (e.g., benzene, Fmoc)
    // Bracket object, which bracket
    int bracket_type;
    // page setup options (NOW HELD IN PREFS...)
    //int page_size;
    //int page_orientation;
    // drag object
    bool dragging;
    // finished rendering?
    bool finishedPainting;
    // set in render2d.cpp::SaveEPS()
    double eps_dx, eps_dy;
    // data for lasso
    QVector<QPoint> tmp_lasso;
    QPoint lasso1, lasso2;
    // QPopupMenu for right clicks
    QMenu *rtclickpopup;
    Molecule *targetmol;
    // current/temporary Molecule
    Molecule *tmp_molecule;
    // kludge for PNG/BMP generation
    bool font_size_kludge;
    // track last known pointer position
    QPoint lastmouse;
    // remember mode if using "hot key"
    QString tmpEpsFile;

    bool hotkeymode;
    int hkprevmode;
    QPoint startPoint, endPoint;
    QVector<QPoint> chainPoints;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
