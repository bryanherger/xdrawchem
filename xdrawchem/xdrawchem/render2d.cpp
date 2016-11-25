// render2d.cpp - definitions for functions of class Render2D

#include <stdlib.h>
#include <iostream>
#include <math.h>

using std::cout;
using std::endl;

#include <qapplication.h>
#include <qclipboard.h>
#include <qwidget.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qfont.h>
#include <qcolor.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdatetime.h>
#include <qprocess.h>

#include "defs.h"
#include "render2d.h"
#include "chemdata.h"

#include "skullcursor.xpm"
#include "rotatecursor.xpm"

Render2D::Render2D(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  setPalette(QPalette(QColor(255,255,255)));

  // generally, width and height should allow for 0.5" margins and 100 dpi
  renderHeight = 1000;
  renderWidth = 750;
  dbuffer.resize(renderWidth, renderHeight);
  directdraw = false;

  resize(renderWidth, renderHeight);
  outputDevice = OUTPUT_SCREEN;

  mode = MODE_SELECT;
  prev_mode = 0;
  prevpos = 0;
  mouse1down = false;
  mouse3down = false;
  doubleClickFlag = false;
  dragging = false;
  moved = false;

  highlightpoint = 0;
  highlightobject = 0;
  localtext = 0;
  localtexteditor = 0;

  thick = 1;
  savedBondOrder = -1;
  zoomFactor = 1.0;

  currentFont = QFont("Helvetica", 12);
  currentColor = QColor(0,0,0);
  bgcolor = QColor(255,255,255);

  setMouseTracking(true);
  setFocusPolicy(ClickFocus);

  QPixmap sc(skullcursor_xpm);
  sc.setMask(sc.createHeuristicMask());
  skullcursor = QCursor(sc, 1, 1);

  QPixmap rc(rotatecursor_xpm);
  rc.setMask(rc.createHeuristicMask());
  rotatecursor = QCursor(rc, 1, 1);

  font_size_kludge = false;
  hotkeymode = false;
  hkprevmode = 0;
  painter = new QPainter;

  // set printing defaults
  PrintSetup();  // see render2d_print.cpp
}

void Render2D::ReactionAnalysis(int x) {
  c->ReactionAnalysis(x);
}

// this function required for good behavior if user changes tools while
// text is selected.
void Render2D::CleanUpAfterTextTool() {
  /*  OLD CODE
  if (localtext == 0) return;  // don't need to clean up if no text.
  // save text, clean up
  localtext->DeselectAllText();
  emit TextOff();
  if (!text_exists) {
    if (localtext->getText().length() > 0)
      { c->addText(localtext); localtext = 0; }
  } else {
    if (localtext->getText().length() == 0)
      { c->Erase(localtext); localtext = 0; }
  }
  localtext = 0;
  highlightpoint = 0;
  if (highlightobject != 0) {
    highlightobject->Highlight(false);
    highlightobject = 0;
  }
  */

  if (localtexteditor != 0) {
    DrawText_FinishText();
  }
}

void Render2D::setClipboard(Clipboard *clip1) { c->setClipboard(clip1); }
Clipboard * Render2D::getClipboard() { return c->getClipboard(); }

// setMode functions (slots)

void Render2D::setMode_Select() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_SELECT; 
  startpoint = NULL; endpoint = NULL;
  setCursor(arrowCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Select mode: left click on object to move, right click on object to edit") );
  repaint(false);
}

void Render2D::setMode_Lasso() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_SELECT_LASSO; 
  startpoint = NULL; endpoint = NULL;
  setCursor(arrowCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Lasso mode: left click and hold to draw lasso") );
  repaint(false);
}

void Render2D::setMode_DrawLine() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWLINE; 
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Draw Line mode: left click to draw line, right click to edit") );
  repaint(false);
}

void Render2D::setMode_DrawDashLine() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWLINE_DASH; 
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Draw Dashed Line mode: left click to draw line, right click to edit") );
  repaint(false);
}

void Render2D::setMode_DrawChain() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWCHAIN; 
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Draw Chain mode: left click and drag to draw aliphatic chain") );
  repaint(false);
}

void Render2D::setMode_DrawUpLine() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWLINE_UP; 
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Draw Stereo-Up Line mode: left click to draw line, right click to edit") );
  repaint(false);
}

void Render2D::setMode_DrawDownLine() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWLINE_DOWN; 
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Draw Stereo-Down Line mode: left click to draw line, right click to edit") );
  repaint(false);
}

void Render2D::setMode_DrawWavyLine() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWWAVYLINE; 
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Draw Wavy Line mode: left click to draw line, right click to edit") );
  repaint(false);
}

void Render2D::setMode_DrawArrow(QString btype) {
  bracket_type = 0;
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWARROW; 
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  if (btype == "REGULAR") bracket_type = ARROW_REGULAR;
  if (btype == "TOP_HARPOON") bracket_type = ARROW_TOPHARPOON;
  if (btype == "BOTTOM_HARPOON") bracket_type = ARROW_BOTTOMHARPOON;
  if (btype == "MIDDLE") bracket_type = ARROW_MIDDLE;
  if (btype == "DIDNT_WORK") bracket_type = ARROW_DIDNT_WORK;
  if (btype == "DASH") bracket_type = ARROW_DASH;
  if (btype == "BI1") bracket_type = ARROW_BI1;
  if (btype == "BI2") bracket_type = ARROW_BI2;
  if (btype == "RETRO") bracket_type = ARROW_RETRO;
  emit SignalSetStatusBar( tr("Draw Arrow mode: left click to draw arrow") );
  repaint(false);
}

void Render2D::setMode_DrawBracket(QString btype) { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWBRACKET; 
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  if (btype == "SQUARE") bracket_type = BRACKET_SQUARE;
  if (btype == "CURVE") bracket_type = BRACKET_CURVE;
  if (btype == "BRACE") bracket_type = BRACKET_BRACE;
  if (btype == "BOX") bracket_type = BRACKET_BOX;
  if (btype == "ELLIPSE") bracket_type = BRACKET_ELLIPSE;
  if (btype == "CLOSEDSQUARE") bracket_type = BRACKET_CLOSEDSQUARE;
  if (btype == "CIRCLE") bracket_type = BRACKET_CIRCLE;
  emit SignalSetStatusBar( tr("Draw Bracket mode: left click to draw bracket") );
  repaint(false);
}

void Render2D::setMode_Erase() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_ERASE; 
  startpoint = NULL; endpoint = NULL;
  localtext = 0;
  setCursor(skullcursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Erase mode: left click to erase object") );
  repaint(false);
}

void Render2D::setMode_DrawText() { 
  mode = MODE_TEXT; 
  if (mode == MODE_RING) smartplace = 0;
  startpoint = NULL; endpoint = NULL;
  setCursor(ibeamCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Text mode: left click to add or edit text") );
  repaint(false);
}

void Render2D::setMode_DrawSymbol(QString s) { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  cout << s << endl;
  mode = MODE_SYMBOL; 
  startpoint = NULL; endpoint = NULL;
  symbolfile = s;
  setCursor(crossCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Draw Symbol mode: left click to add symbol") );
  repaint(false);
}

void Render2D::setMode_DrawGraphicObject(int g1, int param1) { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  cout << g1 << endl;
  bracket_type = g1;
  if (g1 == TYPE_BEZIER) {
    mode = MODE_DRAWBEZIER; 
    tmp_bezier.resize(4);
    bezier_count = 0;
    bracket_type = param1;  // none, half, full arrowhead
    emit SignalSetStatusBar( tr("Draw graphic object: cubic bezier") );
  }
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  repaint(false);
}

void Render2D::setMode_DrawNewmanAnti() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWNEWMAN_ANTI;
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  repaint(false);
  emit SignalSetStatusBar( tr("Left-click to draw Newman projection template") );
}

void Render2D::setMode_DrawNewmanGauche() { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWNEWMAN_GAUCHE;
  startpoint = NULL; endpoint = NULL;
  setCursor(crossCursor);
  c->DeselectAll();
  repaint(false);
  emit SignalSetStatusBar( tr("Left-click to draw Newman projection template") );
}

void Render2D::setMode_DrawRing(QString s, QString fn, int sp) {
  smartplace = 0;
  smartplace = sp;
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  cout << s << endl;
  mode = MODE_RING; 
  startpoint = NULL; endpoint = NULL;
  symbolfile = s;
  setCursor(crossCursor);
  c->DeselectAll();
  emit SignalSetStatusBar( tr("Draw Ring mode: left click to add %1").arg(fn));
  repaint(false);
}

void Render2D::setMode_DrawCurveArrow(QString s) { 
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = MODE_DRAWCURVEARROW; 
  startpoint = NULL; endpoint = NULL;
  symbolfile = s;
  setCursor(crossCursor);
  c->DeselectAll();
  emit SignalSetStatusBar("Draw Curved Arrow mode: left click to draw arrow");
  repaint(false);
}

void Render2D::Tool(int t) {
  if (mode == MODE_TEXT) CleanUpAfterTextTool();
  if (mode == MODE_RING) smartplace = 0;
  mode = t;
  startpoint = NULL; endpoint = NULL;
  setCursor(arrowCursor);
  switch (mode) {
  case MODE_TOOL_MOLECULE_INFO:
    emit SignalSetStatusBar( tr("Click on a molecule for information") );
    break;
  case MODE_TOOL_CALCMW:
    emit SignalSetStatusBar( tr("Click on a molecule to calculate its molecular weight") );
    break;
  case MODE_TOOL_CALCEF:
    emit SignalSetStatusBar( tr("Click on a molecule to calculate its empirical formula") );
    break;
  case MODE_TOOL_CALCEA:
    emit SignalSetStatusBar( tr("Click on a molecule to calculate its elemental analysis") );
    break;
  case MODE_TOOL_13CNMR:
    emit SignalSetStatusBar( tr("Click on a molecule to calculate its 13C NMR") );
    break;
  case MODE_TOOL_1HNMR:
    emit SignalSetStatusBar( tr("Click on a molecule to calculate its 1H NMR") );
    break;
  case MODE_TOOL_IR:
    emit SignalSetStatusBar( tr("Click on a molecule to calculate its IR") );
    break;
  case MODE_TOOL_PKA:
    emit SignalSetStatusBar( tr("Click on a molecule to calculate its pKa(s)") );
    break;
  case MODE_TOOL_RETRO:
    emit SignalSetStatusBar( tr("Click on a molecule for possible retrosynthesis") );
    break;
  case MODE_TOOL_RETRO_BONDNAME:
    highlightobject = 0;
    emit SignalSetStatusBar( tr("Click on a molecule to display its bond identifier") );
    break;
  case MODE_TOOL_KOW:
    emit SignalSetStatusBar( tr("Click on a molecule to calculate its octanol-water partition") );
    break;
  case MODE_TOOL_2D3D:
    emit SignalSetStatusBar( tr("Click on a molecule to generate 3-D coordinates") );
    break;
  case MODE_TOOL_NAME:
    emit SignalSetStatusBar( tr("Click on a molecule to guess its name") );
    break;
  case MODE_TOOL_TOSMILES:
    emit SignalSetStatusBar( tr("Click on a molecule to determine its SMILES string") );
    break;
  case MODE_TOOL_CLEANUPMOL:
    emit SignalSetStatusBar( tr("Click on a molecule to clean up its structure") );
    break;
  case MODE_TOOL_GROUP_REACTANT:
    emit SignalSetStatusBar( tr("Click on a molecule to assign it as a reactant") );
    break;
  case MODE_TOOL_GROUP_PRODUCT:
    emit SignalSetStatusBar( tr("Click on a molecule to assign it as a product") );
    break;
  case MODE_TOOL_GROUP_CLEAR:
    emit SignalSetStatusBar( tr("Click on a molecule to clear it group assignment") );
    break;
  case MODE_TOOL_CUSTOMRING:
    emit SignalSetStatusBar( tr("Click on a molecule to save it as a custom ring") );
    break;
  case MODE_TOOL_TEST:
    emit SignalSetStatusBar( tr("Tool test mode") );
    break;
  default:
    emit SignalSetStatusBar( tr("Click on a molecule") );
    break;
  }
}

void Render2D::clearAllGroups() {
  c->clearAllGroups();
  repaint(false);
}

void Render2D::AutoLayout() {
  c->DeselectAll();
  c->StartUndo(0, 0);
  c->AutoLayout();
  mode = MODE_SELECT; 
  startpoint = NULL; endpoint = NULL;
  setCursor(arrowCursor);
  emit SignalSetStatusBar( tr("Select mode: left click to move, right click to edit") );
  repaint(false);
}

void Render2D::Cut() {
  c->StartUndo(0, 0);
  c->Cut();
  mode = MODE_SELECT;  // the selection is gone, don't draw selection box
  // the selection is gone, don't reference it anymore
  highlightpoint = 0; highlightobject = 0;
  repaint(false);
}

void Render2D::Copy() {
  c->Copy();
  // copy to system clipboard
  QRect savebox = selectionBox;
  int prevmode = mode;
  mode = MODE_SELECT;
  c->DeselectAll();
  repaint(false);
  QPixmap pm( savebox.size() );
  bitBlt( &pm, 0, 0, this, savebox.left(), savebox.top(), 
	  savebox.width(), savebox.height() );
  c->NewSelectRect(savebox, false);
  selectionBox = c->selectionBox();
  mode = prevmode;
  QApplication::clipboard()->setImage(pm.convertToImage());
  repaint(false);  // do anyways, though Copy() should not alter the drawing.
}

void Render2D::Paste() {
  if (mode == MODE_TEXT) {
    if (localtext != 0) { 
      QClipboard* clip=QApplication::clipboard();
      QString pastext = clip->text(QClipboard::Clipboard);
      localtext->InsertString(pastext);
      repaint(false); 
    }
    return;  // eliminate unpredictable behavior
  }
  c->StartUndo(0, 0);
  if (c->Paste() == false) return;
  c->Move(30,30);
  mode = MODE_SELECT_MULTIPLE_SELECTED;
  selectionBox = c->selectionBox();
  repaint(false);
}

void Render2D::Undo() {
  c->SelectAll();
  c->EraseSelected();
  if (c->Undo())
    emit SignalSetStatusBar( tr("Undo!") );
  else
    emit SignalSetStatusBar( tr("Cannot undo, sorry!") );
  c->DeselectAll();
  //mode = MODE_SELECT;
  repaint(false);
}

void Render2D::SelectAll() {
  mode = MODE_SELECT_MULTIPLE_SELECTED;
  c->SelectAll();
  selectionBox = c->selectionBox();
  repaint(false);
}

void Render2D::DeselectAll() {
  mode = MODE_SELECT;
  c->DeselectAll();
  repaint(false);
}

void Render2D::EraseSelected() {
  if (mode == MODE_TEXT) {
    if (localtext != 0) { localtext->DeleteKeyPressed(); repaint(false); }
    return;  // eliminate unpredictable behavior
  }
  c->StartUndo(0, 0);
  c->EraseSelected();
  // in all likelihood, we just blew away highlightobject.
  // if not (unlikely!), user can reselect it.
  highlightobject = 0;
  if (mode == MODE_SELECT_MULTIPLE_SELECTED) mode = MODE_SELECT;
  repaint(false);
}

void Render2D::Rotate90() {
  c->StartUndo(0, 0);
  rotateOrigin.setX( (selectionBox.right() + selectionBox.left() - 2) / 2);
  rotateOrigin.setY( (selectionBox.bottom() + selectionBox.top()) / 2);
  DPoint *dp1 = new DPoint( rotateOrigin );
  c->Rotate(dp1, -0.5 * M_PI);
  delete dp1;
  selectionBox = c->selectionBox();  
  repaint(false);
}

void Render2D::Rotate180() {
  c->StartUndo(0, 0);
  rotateOrigin.setX( (selectionBox.right() + selectionBox.left() - 2) / 2);
  rotateOrigin.setY( (selectionBox.bottom() + selectionBox.top()) / 2);
  DPoint *dp1 = new DPoint( rotateOrigin );
  c->Rotate(dp1, M_PI);
  delete dp1;
  selectionBox = c->selectionBox();  
  repaint(false);
}

void Render2D::Rotate270() {
  c->StartUndo(0, 0);
  rotateOrigin.setX( (selectionBox.right() + selectionBox.left() - 2) / 2);
  rotateOrigin.setY( (selectionBox.bottom() + selectionBox.top()) / 2);
  DPoint *dp1 = new DPoint( rotateOrigin );
  c->Rotate(dp1, 0.5 * M_PI);
  delete dp1;
  selectionBox = c->selectionBox();  
  repaint(false);
}

void Render2D::Flip(int d) {
  c->StartUndo(0, 0);
  rotateOrigin.setX( (selectionBox.right() + selectionBox.left() - 2) / 2);
  rotateOrigin.setY( (selectionBox.bottom() + selectionBox.top()) / 2);
  DPoint *dp1 = new DPoint( rotateOrigin );
  c->Flip(dp1, d);
  delete dp1;
  selectionBox = c->selectionBox();  
  repaint(false);
}

// Inserted(): highlight and box items inserted by RingDialog, etc.
void Render2D::Inserted() {
  mode = MODE_SELECT_MULTIPLE_SELECTED;
  selectionBox = c->selectionBox();
  repaint(false);
}

// change current font, or change font of active text
void Render2D::SetFont(QFont f) {
  if (localtext != 0) {
    localtext->setFont(f);
    if (localtexteditor != 0)
      localtexteditor->setCurrentFont(f);
    repaint(false);
    return;
  } else {
    currentFont = f;
  }
}

// change current font, or change font of active text
void Render2D::SetColor(QColor c1) {
  if (mode != MODE_SELECT_MULTIPLE_SELECTED) {
    currentColor = c1;
    if (localtext != 0) localtext->SetColor(c1);
    if (localtexteditor != 0)
      localtexteditor->setColor(c1);
  } else {
    c->SetColorIfHighlighted(c1);
  }
}

QFont Render2D::GetFont() { return currentFont; }

QColor Render2D::GetColor() { return currentColor; }

// adjust endpoint to specified fixed length/angle
void Render2D::CorrectEndpoint_arrow() {
  double dx = endpoint->x - startpoint->x;
  double dy = endpoint->y - startpoint->y;
  int mysign = 1;
  if (dx < 0.0) mysign = -1;
  double ang = atan(dy/dx) * MOL_ARAD;
  double newang = ((int)( (ang + (preferences.getArrow_fixedangle()/2.0)) / preferences.getArrow_fixedangle() ) * preferences.getArrow_fixedangle());
  if (fabs(newang) != 90.0) {
    dx = mysign * preferences.getArrow_fixedlength() * cos(newang / MOL_ARAD);
    dy = mysign * preferences.getArrow_fixedlength() * sin(newang / MOL_ARAD);
    endpoint->x = startpoint->x + dx;
    endpoint->y = startpoint->y + dy;
  } else {
    if (dy < 0.0) mysign = -1;
    endpoint->x = startpoint->x;
    endpoint->y = startpoint->y + mysign * preferences.getArrow_fixedlength();
  }
}

// adjust endpoint to specified fixed length/angle
void Render2D::CorrectEndpoint_bond() {
  double dx = endpoint->x - startpoint->x;
  double dy = endpoint->y - startpoint->y;
  int mysign = 1;
  if (dx < 0.0) mysign = -1;
  double ang = atan(dy/dx) * MOL_ARAD;
  double newang = ((int)( (ang + (preferences.getBond_fixedangle()/2.0)) / preferences.getBond_fixedangle() ) * preferences.getBond_fixedangle());
  if (fabs(newang) != 90.0) {
    dx = mysign * preferences.getBond_fixedlength() * cos(newang / MOL_ARAD);
    dy = mysign * preferences.getBond_fixedlength() * sin(newang / MOL_ARAD);
    endpoint->x = startpoint->x + dx;
    endpoint->y = startpoint->y + dy;
  } else {
    if (dy < 0.0) mysign = -1;
    endpoint->x = startpoint->x;
    endpoint->y = startpoint->y + mysign * preferences.getBond_fixedlength();
  }
}

bool Render2D::SaveSVG(QString fn) {
  outputDevice = OUTPUT_SVG;
  output_file.setName(fn);
  // open file and create text stream
  if ( !output_file.open(IO_WriteOnly) ) return false;
  output_ts.setDevice(&output_file);

  QRect r;
  c->SelectAll();
  r = c->selectionBox();
  selectionBox = r;
  c->DeselectAll();
  mode = MODE_SELECT;  // so selection box will not appear
  svg_dx = r.x(); svg_dy = r.y();

  output_ts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  output_ts << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << endl;
  output_ts << "<svg version=\"1.0\" width=\"" << r.width() << 
    "\" height=\"" << r.height() << "\">" << endl;

  repaint(false);

  output_ts << "</svg>" << endl;

  output_file.close();
  // restore screen display
  outputDevice = OUTPUT_SCREEN;
  repaint(false);
  return true;
}

bool Render2D::SaveEPS(QString fn) {
  //
  // perhaps look at printing using QPrinter, then shelling to use ps2epsi?
  //
  // tmp_fn = xdc[procno].ps
  QString n1;
  srand(QTime::currentTime().second());
  n1.setNum(rand());
  QString tmp_fn = QString("/tmp/xdc") + n1 + QString(".ps");
  Print(tmp_fn);
  // shell (ps2epsi tmp_fn fn)
  QProcess *proc1 = new QProcess( this );
  proc1->addArgument( "ps2epsi" );
  proc1->addArgument( tmp_fn );
  proc1->addArgument( fn );
  proc1->start();
  do {
    // nothing!
  } while (proc1->normalExit() == false);
  QProcess *proc2 = new QProcess( this );
  proc2->addArgument( "rm" );
  proc2->addArgument( tmp_fn );
  proc2->start();
  return true;

  /*
  outputDevice = OUTPUT_EPS;
  output_file.setName(fn);
  // open file and create text stream
  if ( !output_file.open(IO_WriteOnly) ) return false;
  output_ts.setDevice(&output_file);
  // get bounding box
  c->SelectAll();
  selectionBox = c->selectionBox();
  c->DeselectAll();
  // shift selection box to origin
  eps_dx = selectionBox.left(); eps_dy = selectionBox.top();
  // write EPS header
  // The following line, while teccnically correct, break most applications.
  //output_ts << "%!PS-Adobe-3.0 EPSF-3.0" << endl;
  // this is the magic cookie most applications check for
  output_ts << "%!PS-Adobe-2.0 EPSF-2.0" << endl;
  double eps_sf = 0.72;
  output_ts << "%%BoundingBox: " << selectionBox.left() - eps_dx  << " ";
  output_ts << selectionBox.top() - eps_dy << " ";
  output_ts << selectionBox.right() - eps_dx << " ";
  output_ts << selectionBox.bottom() - eps_dy << endl;
  output_ts << "%%Creator: XDrawChem" << endl;
  output_ts << "%%Title: " << fn << endl;
  output_ts << "%%EndComments" << endl;
  //output_ts << selectionBox.left() << " " << selectionBox.top() << " translate" << endl;
  //output_ts << eps_sf << " "<< eps_sf << " scale" << endl;
  // write objects
  repaint(false);
  // finish
  output_ts << "showpage" << endl;
  output_ts << "%%EOF" << endl;
  output_file.close();
  // restore screen display
  outputDevice = OUTPUT_SCREEN;
  repaint(false);
  return true;
  */
}

QPoint Render2D::zoomCorrect(QPoint in1) {
  QPoint out1;
  double dx = (double)in1.x();
  double dy = (double)in1.y();

  dx *= zoomFactor;
  dy *= zoomFactor;

  out1.setX( my_round(dx) );
  out1.setY( my_round(dy) );
  return out1;
}

QPoint Render2D::zoomCorrectReverse(QPoint in1) {
  QPoint out1;
  double dx = (double)in1.x();
  double dy = (double)in1.y();

  dx /= zoomFactor;
  dy /= zoomFactor;

  out1.setX( my_round(dx) );
  out1.setY( my_round(dy) );
  return out1;
}
