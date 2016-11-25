// render2d_text.cpp - text-editing functions in class Render2D

#include <iostream>
#include <math.h>
#include <qwidget.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qtextstream.h>
#include <qinputdialog.h>

using std::cerr;
using std::endl;

#include "render2d.h"
#include "chemdata.h"
#include "molecule.h"
#include "text.h"
#include "bracket.h"
#include "defs.h"
#include "charsel.h"

// insert symbol, from Application
void Render2D::InsertSymbol() {
  if (localtexteditor == 0) return;

  CharSelDialog cd1(this, "charsel");
  QFont tf1;
  if (cd1.exec()) {
    tf1 = localtexteditor->currentFont();
    localtexteditor->setCurrentFont( cd1.selectedFont() );
    localtexteditor->insert( cd1.selectedChar() );
    localtexteditor->setCurrentFont( tf1 );
  }
}

// events from QTextEdit
void Render2D::DrawText_returnPressed(void) {
  std::cout << "Return pressed inside QTextEdit" << std::endl;
  if ( localtext->Justify() == JUSTIFY_CENTER ) {
    // this appears to cause a seg fault only under this condition,
    // called from signal...
    DrawText_FinishText();
    return;
  }
}

void Render2D::DrawText_textChanged(void) {
  std::cout << "contents: " << localtexteditor->contentsWidth() << ",";
  std::cout << localtexteditor->contentsHeight() << std::endl;
  localtexteditor->resize( localtexteditor->contentsWidth() + 21,
			   localtexteditor->contentsHeight() );
}

void Render2D::DrawText_CVAchanged(QTextEdit::VerticalAlignment cva) {
  std::cout << "New vertical alignment: " << (int)cva << std::endl;
}

void Render2D::DrawText_FinishText() {
  emit TextOff();
  if (hotkeymode) {
    hotkeymode = false;
    mode = hkprevmode;
    std::cout << "hkprevmode = " << mode << std::endl;
  }
  QString lt2 = localtexteditor->text();
  localtexteditor->setTextFormat(Qt::PlainText);
  QString lt1 = localtexteditor->text();
  if (localtext->Justify() == JUSTIFY_CENTER) {
    lt1.remove('\n');
    lt2.remove('\n');
    lt2.remove("<p></p>");
  }

  std::cout << "lt1 = " << lt1 << std::endl;
  std::cout << "lt2 = " << lt2 << std::endl;

  localtext->setText( lt1, lt2 );
  // this is done for us now by text::setText!
  //lt1.fill(' ');
  //localtext->setTextMask( lt1 );
  if (localtext->getText().length() > 0) { 
    if (!text_exists) c->addText(localtext); 
  } else {
    localtext->Start()->element = "C";
    c->Erase(localtext);
  }
  if (localtext->Justify() == JUSTIFY_TOPLEFT) {
    delete localtexteditor;
    localtexteditor = 0;
  } else {
    localtexteditor->hide();
    localstring = "XDC_DELETE_ME";
  }
  localtext = 0;
  highlightpoint = 0;
  if (highlightobject != 0) {
    highlightobject->Highlight(false);
  }      
  highlightobject = 0;
  if (doubleClickFlag) {
    mode = prev_mode;
    doubleClickFlag = false;
  }
  // return focus to Render2D
  setFocus();
  return;
}

// mouse events to handle text functions
// (button states should be set before we get here)
void Render2D::DrawText_mousePressEvent(QMouseEvent *e1, QPoint cqp) {

  // experimental: use QTextEdit to edit text!
  if (localtexteditor != 0) {
    // if clicked off localtexteditor...
    std::cout << "click outside editor" << std::endl;
    DrawText_FinishText();
    return;
  }

  if (localtexteditor == 0) {
    localtexteditor = new QTextEdit(this, "local text editor");
    localtexteditor->setTextFormat(Qt::RichText);
    super_set = false; sub_set = false;
  }
  if (highlightobject != 0) { // edit existing object
    if (highlightobject->Type() == TYPE_BRACKET) {
      delete localtexteditor;
      localtexteditor = 0;
      Bracket *this_bracket = (Bracket *)highlightobject;
      bool ok = false;
      QString btext = QInputDialog::getText(tr( "Enter subscript" ),
	tr( "Please type or edit the subscript for this bracket:" ),
	QLineEdit::Normal, this_bracket->getText(), &ok, this );
      if ( ok )
	this_bracket->setText(btext);
      else
	this_bracket->setText("");
      return;  // a seg fault will occur otherwise!!!
    }

    text_exists = true;
    localtext = (Text *)highlightobject;
    Q_CHECK_PTR(localtext);
  }
  localtexteditor->setText( "" );
  if (localtext != 0) {
    localtexteditor->setCurrentFont( localtext->getFont() );
    localtexteditor->setColor( localtext->GetColor() );
    localtexteditor->setText( localtext->getRichText() );
    localtexteditor->move( localtext->Start()->toQPoint() );
    //localtexteditor->resize( localtext->BoundingBox().width() + 40,
    //		     localtext->BoundingBox().height() + 20 );
    localtexteditor->resize( localtexteditor->contentsWidth() + 21,
			     localtexteditor->contentsHeight() );
  } else {
    text_exists = false;
    localtexteditor->move( e1->pos() );
    localtext = new Text(this);
    localtext->setFont(currentFont);
    localtext->SetColor(currentColor);
    if (highlightpoint) { 
      localtext->setPoint(highlightpoint);
      localtext->setJustify(JUSTIFY_CENTER);
      localtext->setText("");
      localtext->setTextMask("");
    } else {
      localtext->setPoint( new DPoint(cqp) );
      localtext->setJustify(JUSTIFY_TOPLEFT);
      localtext->setText("");
      localtext->setTextMask("");
    }
    localtexteditor->setCurrentFont( localtext->getFont() );
    localtexteditor->setColor( localtext->GetColor() );
  }
  emit TextOn( localtext->getFont() );
  connect( localtexteditor, SIGNAL( returnPressed() ), this,
	   SLOT( DrawText_returnPressed() ) );
  connect( localtexteditor, SIGNAL( textChanged() ), this,
	   SLOT( DrawText_textChanged() ) );
  localtexteditor->show();
  localtexteditor->moveCursor( QTextEdit::MoveEnd, false );
  localtexteditor->setFocus();
  return;
}

void Render2D::DrawText_mouseReleaseEvent(QMouseEvent *e1, QPoint cqp) {
  // short circuit for now!
  return;
  // end QTextEdit experiment
}

void Render2D::DrawText_mouseMoveEvent(QMouseEvent *e1) {
  //bool update;
  DPoint *prevhighlight = highlightpoint;
  Drawable *prevhighlightobject = highlightobject;
  // Create DPoint of current pointer position
  DPoint *e = new DPoint;
  DPoint *np = 0;
  e->x = e1->x(); e->y = e1->y();
  double dist, distobj;
  // Get DPoint of nearest point
  np = c->FindNearestPoint(e, dist);
  // get Drawable of nearest object
  Drawable *no = c->FindNearestObject(e, distobj);

  if (localtext != 0) {  // handle moves when there is a current object
    if (text_drag == true) {
      localtext->Select(start_drag, e);
      repaint(false);
      return;
    }
    //return;
  }

  // look for place to draw if no current text object
  if (no != 0) {
    if ( (no->Type() == TYPE_TEXT) || 
	 ((no->Type() == TYPE_BRACKET) && (distobj < 8.0)) ) { 
      // highlight text if closest, or bracket
      highlightpoint = 0;
      highlightobject = no;
      if (prevhighlightobject != 0) prevhighlightobject->Highlight(false);
      highlightobject->Highlight(true);
      if (prevhighlightobject != highlightobject) repaint(false);
      return;
    } else { // deselect and check for points
      // Clear highlighted object
      highlightobject = 0;
      if (prevhighlightobject != 0) prevhighlightobject->Highlight(false);
      if (prevhighlightobject != highlightobject) repaint(false);
    }
  }
  // clear highlighted object (if any)
  if (prevhighlightobject != 0) {
    prevhighlightobject->Highlight(false);
    highlightobject = 0;
    repaint(false);
  }
  // check points
  if (dist < 6.0) {
    highlightpoint = np;
    if (prevhighlight != highlightpoint) repaint(false);
    return;
  }
  if (dist >= 6.0) {
    // Clear highlighted point
    highlightpoint = 0;
    if (prevhighlight != highlightpoint) repaint(false);
    return;
  }
}

// Handle keypress event here, since it only really applies to text
void Render2D::keyPressEvent(QKeyEvent *k) {
  std::cout << "keypress" << std::endl;

  // new stuff for localtexteditor
  if (localtexteditor != 0) {
    if (k->key() == Key_Escape) {
      DrawText_FinishText();
      return;
    }
    if (k->key() == Key_Delete) {
      std::cerr << "EXCEPTION: Delete key caught." << endl;
      return;
    }
  }

  //if (mode != MODE_TEXT) {
    if (k->key() == Key_Escape) {
      setMode_Select();
      return;
    } else {
      if (highlightobject != 0) std::cout << "hiobj" << std::endl;
      if (highlightpoint != 0) std::cout << "hipt" << std::endl;
      if ( (highlightpoint != 0) || (highlightobject != 0) ) {
	// emulate ChemDraw(R) "hot key" behavior
	QString tmpstr = k->text();
	std::cout << "hot key:" << tmpstr << ":" << std::endl;
	if (tmpstr.length() < 1) return;
	tmpstr = tmpstr.upper();
	hotkeymode = true;
	hkprevmode = mode;
	mode = MODE_TEXT;
	if (localtexteditor != 0) {
	  // if clicked off localtexteditor...
	  std::cout << "click outside editor" << std::endl;
	  DrawText_FinishText();
	  return;
	}
	if (localtexteditor == 0) {
	  localtexteditor = new QTextEdit(this, "local text editor");
	  localtexteditor->setTextFormat(Qt::RichText);
	  super_set = false; sub_set = false;
	}
	if (highlightobject != 0) { // edit existing object
	  if (highlightobject->Type() == TYPE_BRACKET) {
	    delete localtexteditor;
	    localtexteditor = 0;
	    Bracket *this_bracket = (Bracket *)highlightobject;
	    bool ok = false;
	    QString btext = QInputDialog::getText(tr( "Enter subscript" ),
						  tr( "Please type or edit the subscript for this bracket:" ),
						  QLineEdit::Normal, tmpstr, &ok, this );
	    if ( ok )
	      this_bracket->setText(btext);
	    else
	      this_bracket->setText("");
	    hotkeymode = false;
	    mode = hkprevmode;
	    return;  // a seg fault will occur otherwise!!!
	  }

	  if (highlightobject->Type() != TYPE_TEXT) {
	    hotkeymode = false;
	    mode = hkprevmode;
	    return;
	  }

	  text_exists = true;
	  localtext = (Text *)highlightobject;
	  Q_CHECK_PTR(localtext);
	}
	localtexteditor->setText( tmpstr );
	if (localtext != 0) {
	  localtexteditor->setCurrentFont( localtext->getFont() );
	  localtexteditor->setColor( localtext->GetColor() );
	  //localtexteditor->setText( localtext->getRichText() );
	  // in this context, set text to hot key, overwriting old text
	  localtexteditor->setText( tmpstr );
	  localtexteditor->move( localtext->Start()->toQPoint() );
	  //localtexteditor->resize( localtext->BoundingBox().width() + 40,
	  //		     localtext->BoundingBox().height() + 20 );
	  localtexteditor->resize( localtexteditor->contentsWidth() + 21,
				   localtexteditor->contentsHeight() );
	} else {
	  text_exists = false;
	  localtexteditor->move( highlightpoint->toQPoint() );
	  localtext = new Text(this);
	  localtext->setFont(currentFont);
	  localtext->SetColor(currentColor);
	  if (highlightpoint) { 
	    localtext->setPoint(highlightpoint);
	    localtext->setJustify(JUSTIFY_CENTER);
	    localtext->setText("");
	    localtext->setTextMask("");
	  } else {
	    localtext->setPoint( new DPoint(lastmouse) );
	    localtext->setJustify(JUSTIFY_TOPLEFT);
	    localtext->setText("");
	    localtext->setTextMask("");
	  }
	  localtexteditor->setCurrentFont( localtext->getFont() );
	  localtexteditor->setColor( localtext->GetColor() );
	}
	emit TextOn( localtext->getFont() );
	connect( localtexteditor, SIGNAL( returnPressed() ), this,
		 SLOT( DrawText_returnPressed() ) );
	connect( localtexteditor, SIGNAL( textChanged() ), this,
		 SLOT( DrawText_textChanged() ) );
	localtexteditor->show();
	localtexteditor->moveCursor( QTextEdit::MoveEnd, false );
	localtexteditor->setFocus();
	return;
      }  // "hot key"
    } // if key == Escape
    //}  // ...if mode!= MODE_TEXT

  return;
}

// Superscript selected text
void Render2D::Superscript() {
  if (localtext != 0) {
    localtext->DoSuperscript();
    repaint(false);
  }
  if (super_set) {
    localtexteditor->setVerticalAlignment(QTextEdit::AlignNormal);
    super_set = false;
  } else {
    localtexteditor->setVerticalAlignment(QTextEdit::AlignSuperScript);
    super_set = true;
  }
}

// Subscript selected text
void Render2D::Subscript() {
  if (localtext != 0) {
    localtext->DoSubscript();
    repaint(false);
  }
  if (sub_set) {
    localtexteditor->setVerticalAlignment(QTextEdit::AlignNormal);
    sub_set = false;
  } else {
    localtexteditor->setVerticalAlignment(QTextEdit::AlignSubScript);
    sub_set = true;
  }
}

// Left-justify text
void Render2D::JustifyLeft() {
  if (localtext != 0) {
    localtext->ForceAlignment(TEXT_LALIGN);
    repaint(false);
  }
  localtexteditor->selectAll();
  localtexteditor->setAlignment(Qt::AlignLeft);
  localtexteditor->selectAll(false);
}

// Center text
void Render2D::JustifyCenter() {
  if (localtext != 0) {
    localtext->ForceAlignment(TEXT_CALIGN);
    repaint(false);
  }
  localtexteditor->selectAll();
  localtexteditor->setAlignment(Qt::AlignCenter);
  localtexteditor->selectAll(false);
}

// Right-justify text
void Render2D::JustifyRight() {
  if (localtext != 0) {
    localtext->ForceAlignment(TEXT_RALIGN);
    repaint(false);
  }
  localtexteditor->selectAll();
  localtexteditor->setAlignment(Qt::AlignRight);
  localtexteditor->selectAll(false);
}

// Bold selected text
void Render2D::Bold() {
  if (localtext != 0) {
    localtext->DoBold();
    repaint(false);
  }
  if (localtexteditor != 0) {
    localtexteditor->setBold( !localtexteditor->bold() );
  }
}

// Italicize selected text
void Render2D::Italic() {
  if (localtext != 0) {
    localtext->DoItalic();
    repaint(false);
  }
  if (localtexteditor != 0) {
    localtexteditor->setItalic( !localtexteditor->italic() );
  }
}

// Underline selected text
void Render2D::Underline() {
  if (localtext != 0) {
    localtext->DoUnderline();
    repaint(false);
  }
  if (localtexteditor != 0) {
    localtexteditor->setUnderline( !localtexteditor->underline() );
  }
}

// origin = origin point of text
// intext = text to draw (or blank); return if modified or not
// justify = how to arrange text around origin
// oneline = if true (i.e., a label), it's a one-line widget
QString Render2D::EditText(QPoint origin, QString intext, int justify,
			   bool oneline) 
{
  return intext;
}

int Render2D::GetTextHeight(QFont fn) {
  QPainter p(this);
  p.setFont(fn);
  QFontMetrics fm = p.fontMetrics();
  
  return fm.ascent();
}

int Render2D::GetTextFullHeight(QFont fn) {
  QPainter p(this);
  p.setFont(fn);
  QFontMetrics fm = p.fontMetrics();
  
  return fm.height();
}

int Render2D::GetCharWidth(QChar ch, QFont fn) {
  QPainter p(this);
  p.setFont(fn);
  QFontMetrics fm = p.fontMetrics();
  
  return fm.width(ch);
}

int Render2D::GetStringWidth(QString ch, QFont fn) {
  QPainter p(this);
  p.setFont(fn);
  QFontMetrics fm = p.fontMetrics();
  
  return fm.width(ch);
}

QRect Render2D::GetTextDimensions(QString txt, QFont fn) {
  QRect retval;

  // if empty, return box large enough to hold cursor and indicate place.
  if (txt.isNull() || txt.isEmpty()) {
    
  }

  QPainter p(this);
  p.setFont(fn);
  int maxwidth, lwidth, linecount, height;
  QFontMetrics fm = p.fontMetrics();
  QTextStream t(txt, IO_ReadOnly);

  linecount = 1;
  maxwidth = 0;
  QString l;
  // find maximum width
  do {
    l = t.readLine();
    lwidth = fm.width(l);
    if (lwidth > maxwidth) maxwidth = lwidth;
  } while (!t.atEnd());
  // find height
  for (int i = 0; i < txt.length(); i++) {
    if ((int)QChar(txt[i]) == 10) linecount++;
  }
  if (linecount < 1) linecount = 1;
  if (maxwidth < 5) maxwidth = 5;
  height = linecount * fm.height();

  retval.setWidth(maxwidth);
  retval.setHeight(height);

  return retval;
}
