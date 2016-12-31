// text.cpp - Text's implementation of functions

// (obsolete note!)
// NOTE ABOUT TEXTMASK: + marks superscript, - marks subscript
// B marks bold, I marks italic, U marks underline

#include <qobject.h>
#include <iostream>
#include <math.h>
#include <qrect.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qtextstream.h>

using std::cout;
using std::endl;

#include "render2d.h"
#include "drawable.h"
#include "text.h"
#include "defs.h"

// lastflag settings

Text::Text(Render2D *r1, QObject *parent, const char *name)
  : Drawable(parent, name)
{
  r = r1;
  font = QFont("Helvetica", 12);  // CHANGE BACK TO 12
  highlighted = false;
  shiftdown = false;
  cursor = 0;
  selectMin = -1; selectMax = -1;
  bfont = font;
  bfont.setWeight(QFont::Bold);
  ifont = font;
  ifont.setItalic(true);
  ufont = font;
  ufont.setUnderline(true);
  molecule = 0;
  tjustify = TEXT_AUTO;
  whichside = 1;
  DataType = TEXT_DATA_NORMAL;
  shape = 0;
  shapewidth = -1; shapeheight = -1;
  oshapewidth = -1; oshapeheight = -1;
  drawBorder = false; drawFill = false;
  borderColor = QColor(0,0,0); fillColor = QColor(0,255,255);
}

QString Text::ToXML(QString xml_id) {
  QString s, n1;
  int fsize;
  fsize = font.pointSize();
  if (fsize < 1) fsize = font.pixelSize();

  // begin text
  s.append("<text id=");
  s.append(xml_id);
  s.append(">\n");

  // write Start
  s.append("<Start>");
  n1.setNum(start->x);
  s.append(n1);
  s.append(" ");
  n1.setNum(start->y);
  s.append(n1);
  s.append("</Start>\n");

  // write text
  s.append("<textstring>");
  s.append(start->element);
  s.append("</textstring>\n");

  // write textmask
  s.append("<richstring>");
  s.append(displayText);
  s.append("</richstring>\n");

  // shape info
  s.append("<shape>");
  n1.setNum(shape);
  s.append(n1);
  s.append("</shape>\n");

  // border info
  s.append("<border>");
  if (drawBorder == true)
    s.append("true");
  else
    s.append("false");
  s.append("</border>\n<bordercolor>");
  n1.setNum(borderColor.red());
  s.append(n1);
  s.append(" ");
  n1.setNum(borderColor.green());
  s.append(n1);
  s.append(" ");
  n1.setNum(borderColor.blue());
  s.append(n1);
  s.append("</bordercolor>\n");

  // fill info
  s.append("<fill>");
  if (drawFill == true)
    s.append("true");
  else
    s.append("false");
  s.append("</fill>\n<fillcolor>");
  n1.setNum(fillColor.red());
  s.append(n1);
  s.append(" ");
  n1.setNum(fillColor.green());
  s.append(n1);
  s.append(" ");
  n1.setNum(fillColor.blue());
  s.append(n1);
  s.append("</fillcolor>\n");

  // shape dimensions
  s.append("<shapegeometry>");
  n1.setNum(shapewidth);
  s.append(n1);
  s.append(" ");
  n1.setNum(shapeheight);
  s.append(n1);
  s.append(" ");
  n1.setNum(oshapewidth);
  s.append(n1);
  s.append(" ");
  n1.setNum(oshapeheight);
  s.append(n1);
  s.append("</shapegeometry>\n");

  // end text
  s.append("</text>\n");

  return s;
}

QString Text::ToCDXML(QString xml_id) {
  QString s, n1;

  // begin text
  s.append("<t id=\"");
  s.append(xml_id);
  s.append("\" p=\"");
  n1.setNum(start->x);
  s.append(n1);
  s.append(" ");
  n1.setNum(start->y);
  s.append(n1);
  s.append("\"><s font=\"21\" size=\"10\" face=\"96\">");
  s.append(start->element);
  s.append("</s></t>\n");

  return s;
}

void Text::FromXML(QString xml_tag) {
  int i1, i2;

  i1 = xml_tag.find("<Start>");
  i2 = xml_tag.find("</Start>") + 8;
  SetStartFromXML(xml_tag.mid(i1, i2 - i1));
  i1 = xml_tag.find("<color>");
  if (i1 >= 0) {
    i2 = xml_tag.find("</color>") + 8;
    SetColorFromXML(xml_tag.mid(i1, i2 - i1));
  }
  i1 = xml_tag.find("<font>");
  if (i1 >= 0) {
    i2 = xml_tag.find("</font>") + 7;
    SetFontFromXML(xml_tag.mid(i1, i2 - i1));
  }
  i1 = xml_tag.find("<textstring>");
  i2 = xml_tag.find("</textstring>") + 13;
  SetTextstringFromXML(xml_tag.mid(i1, i2 - i1));
  i1 = xml_tag.find("<richstring>");
  i2 = xml_tag.find("</richstring>") + 13;
  SetRichstringFromXML(xml_tag.mid(i1, i2 - i1));
  i1 = xml_tag.find("<textmask>");
  i2 = xml_tag.find("</textmask>") + 11;
  SetTextmaskFromXML(xml_tag.mid(i1, i2 - i1));
  i1 = xml_tag.find("<shape>");
  if (i1 > 0) {
    i2 = xml_tag.find("</shape>");
    shape = xml_tag.mid(i1 + 7, i2 - i1 - 7).toInt();
    std::cout << "shape:" << shape << std::endl;
  }
  i1 = xml_tag.find("<border>");
  if (i1 > 0) {
    i2 = xml_tag.find("</border>");
    std::cout << "border:" << xml_tag.mid(i1 + 8, 4) << std::endl;
    if (xml_tag.mid(i1 + 8, 4) == "true")
      drawBorder = true;
    else
      drawBorder = false;
  }
  i1 = xml_tag.find("<fill>");
  if (i1 > 0) {
    i2 = xml_tag.find("</fill>");
    std::cout << "fill:" << xml_tag.mid(i1 + 6, 4) << std::endl;
    if (xml_tag.mid(i1 + 6, 4) == "true")
      drawFill = true;
    else
      drawFill = false;
  }
  i1 = xml_tag.find("<bordercolor>");
  if (i1 >= 0) {
    i2 = xml_tag.find("</bordercolor>") + 14;
    SetBorderColorFromXML(xml_tag.mid(i1, i2 - i1));
  }
  i1 = xml_tag.find("<fillcolor>");
  if (i1 >= 0) {
    i2 = xml_tag.find("</fillcolor>") + 12;
    SetFillColorFromXML(xml_tag.mid(i1, i2 - i1));
  }
  i1 = xml_tag.find("<shapegeometry>");
  if (i1 >= 0) {
    i2 = xml_tag.find("</shapegeometry>") + 16;
    SetShapeGeometryFromXML(xml_tag.mid(i1, i2 - i1));
  }
}

void Text::SetBorderColorFromXML(QString xml_tag) {
  cout << "SetBColorFromXML:" << xml_tag << endl;
  int i1, i2;
  int d1, d2, d3;

  i1 = xml_tag.find("<bordercolor>");
  i2 = xml_tag.find("</bordercolor>");
  xml_tag.remove(i2, 999);
  xml_tag.remove(i1, 13);

  QTextStream ts(&xml_tag, IO_ReadOnly);

  ts >> d1 >> d2 >> d3;

  borderColor = QColor(d1, d2, d3);
}

void Text::SetFillColorFromXML(QString xml_tag) {
  cout << "SetFColorFromXML:" << xml_tag << endl;
  int i1, i2;
  int d1, d2, d3;

  i1 = xml_tag.find("<fillcolor>");
  i2 = xml_tag.find("</fillcolor>");
  xml_tag.remove(i2, 999);
  xml_tag.remove(i1, 11);

  QTextStream ts(&xml_tag, IO_ReadOnly);

  ts >> d1 >> d2 >> d3;

  fillColor = QColor(d1, d2, d3);
}

void Text::SetShapeGeometryFromXML(QString xml_tag) {
  cout << "SetSGFromXML:" << xml_tag << endl;
  int i1, i2;
  int d1, d2, d3, d4;

  i1 = xml_tag.find("<shapegeometry>");
  i2 = xml_tag.find("</shapegeometry>");
  xml_tag.remove(i2, 999);
  xml_tag.remove(i1, 15);

  QTextStream ts(&xml_tag, IO_ReadOnly);

  ts >> d1 >> d2 >> d3 >> d4;

  shapewidth = d1;
  shapeheight = d2;
  oshapewidth = d3;
  oshapeheight = d4;
}

// convert XML <font> tag to QFont and set current
void Text::SetFontFromXML(QString xml_tag) {
  cout << "SetFontFromXML:" << xml_tag << endl;
  int i1, i2;
  //int d1, d2, d3;

  i1 = xml_tag.find("<font>");
  i2 = xml_tag.find("</font>");
  xml_tag.remove(i2, 999);
  xml_tag.remove(i1, 6);
  i1 = xml_tag.find("#");
  cout << xml_tag.mid(0, i1) << "*" << xml_tag.mid(i1 + 1) << endl;
  font = QFont(xml_tag.mid(0, i1), xml_tag.mid(i1 + 1).toInt());
}

// get Text::text from <textstring> tag
void Text::SetTextstringFromXML(QString xml_tag) {
  cout << "SetTextstringFromXML:" << xml_tag << endl;
  int i1, i2;
  //int d1, d2, d3;

  i1 = xml_tag.find("<textstring>");
  i2 = xml_tag.find("</textstring>");
  xml_tag.remove(i2, 999);
  xml_tag.remove(i1, 12);
  if (start != 0)
    start->element = xml_tag;
}

// get Text::displayText from <richstring> tag
void Text::SetRichstringFromXML(QString xml_tag) {
  cout << "SetRichstringFromXML:" << xml_tag << endl;
  int i1, i2;
  //int d1, d2, d3;

  i1 = xml_tag.find("<richstring>");
  i2 = xml_tag.find("</richstring>");
  xml_tag.remove(i2, 999);
  xml_tag.remove(i1, 12);
  if (start != 0)
    displayText = xml_tag;
}

// get Text::textmask from <textmask> tag
void Text::SetTextmaskFromXML(QString xml_tag) {
  // not used!!!  either <richstring> or <elementmask> tag used to set display

  //cout << "SetTextmaskFromXML:" << xml_tag << endl;

  // obsolete tag!  silently update...
  // note that user may need to reformat text
  //displayText = start->element;
  //displayText.replace("\n", "</p>");

  /*
  int i1, i2;
  //int d1, d2, d3;

  i1 = xml_tag.find("<textmask>");
  i2 = xml_tag.find("</textmask>");
  xml_tag.remove(i2, 999);
  xml_tag.remove(i1, 10);
  if (start != 0)
    start->elementmask = xml_tag;
  */
}

bool Text::WithinRect(QRect n, bool shiftdown) {
  if ( DPointInRect(start, n) )
    highlighted = true;
  else
    highlighted = false;
  return highlighted;
}

void Text::Render(void) {
  //std::cout << "displayText.length():" << displayText.length() << std::endl;

  bool boldFlag = false, italicFlag = false, underlineFlag = false,
    superFlag = false, subFlag = false;
  QFont currentFont = font;
  QColor currentColor = color;

  if (r->getFontKludge()) {
    currentFont.setPointSize( currentFont.pointSize() * 2 );
    std::cout << "fontkludge (top)" << std::endl;    
  }

  QStringList parseList, renderList, lineWidths;

  int lineheight = 0, linewidth = 0, textheight = 0, textwidth = 0;;
  int cp1, lp1, lp2;
  QString n1, currentElement = "";
  bool flag1 = false;
  for (cp1 = 0; cp1 < displayText.length() ; cp1++ ) {
    flag1 = true;
    if (displayText[cp1] == '<') {
      parseList.append(currentElement);
      currentElement = "<";
      flag1 = false;      
    }
    if (displayText[cp1] == '>') {
      currentElement.append(displayText[cp1]);
      parseList.append(currentElement);
      currentElement = "";
      flag1 = false;
    }
    if (flag1) {
      currentElement.append(displayText[cp1]);
    }
  }

  // just in case there were no tags...
  if (currentElement.length() > 0) parseList.append(currentElement);

  // mock render to determine width and height
  QRect b1;
  int maxfont = 0, thisfont;
  for ( QStringList::Iterator it = parseList.begin(); it != parseList.end(); 
	++it ) {
    currentElement = *it;
    if (currentElement == "") continue;
    if (currentElement == " ") continue;
    if (currentElement == "\n") continue;
    if (currentElement == "<p>") continue;
    if (currentElement == "<html>") continue;
    if (currentElement == "</html>") continue;
    if (currentElement == "<head>") continue;
    if (currentElement == "</head>") continue;
    if (currentElement == "</body>") continue;
    if (currentElement.left(3) == "<p ") continue;
    if (currentElement.left(6) == "<body ") continue;
    if (currentElement.left(6) == "<meta ") continue;
    if (currentElement == "</span>") {
      // this likely undoes a super/sub flag
      if (superFlag == true) superFlag = false;
      if (subFlag == true) subFlag = false;
      renderList.append(currentElement);
      continue;
    }
    currentElement.replace("&lt;", "<");
    currentElement.replace("&gt;", ">");
    renderList.append(currentElement);
    if (currentElement == "</p>") {
      n1.setNum(linewidth);
      lineWidths.append(n1);
      if (linewidth > textwidth) textwidth = linewidth;
      textheight += lineheight;
      linewidth = 0;
      lineheight = 0;
      continue;
    }
    if (currentElement.left(6) == "<span ") {
      // set font
      //std::cout << "span: " << currentElement << std::endl;
      boldFlag = false, italicFlag = false, underlineFlag = false,
	superFlag = false, subFlag = false;      
      if (currentElement.contains("font-family") > 0) {
	lp2 = currentElement.find("font-family");
	lp1 = currentElement.find(":", lp2);
	lp2 = currentElement.find(";", lp1);
	currentFont.setFamily( currentElement.mid(lp1 + 1, lp2 - lp1 - 1) );
      }
      if (currentElement.contains("font-size") > 0) {
	lp2 = currentElement.find("font-size");
	lp1 = currentElement.find(":", lp2);
	lp2 = currentElement.find("pt", lp1);
	thisfont = currentElement.mid(lp1 + 1, lp2 - lp1 - 1).toInt();
	currentFont.setPointSize( thisfont );
	if (thisfont > maxfont) maxfont = thisfont;
	if ( r->getFontKludge() == true ) {
	  currentFont.setPointSize( currentFont.pointSize() * 2 );
	  std::cout << "fontkludge" << std::endl;
	}
	std::cout << "points: " << currentFont.pointSize() << std::endl;
      }
      currentFont.setBold(false);
      currentFont.setItalic(false);
      currentFont.setUnderline(false);
      if (currentElement.contains("font-weight") > 0) {
	boldFlag = true;
	currentFont.setBold(true);
      }
      if (currentElement.contains("italic") > 0) {
	italicFlag = true;
	currentFont.setItalic(true);
      }
      if (currentElement.contains("underline") > 0) {
	underlineFlag = true;
	currentFont.setUnderline(true);
      }
      if (currentElement.contains("super") > 0) {
	superFlag = true;
      }
      if (currentElement.contains("sub") > 0) {
	subFlag = true;
      }
      continue;
    }
    b1 = r->GetTextDimensions(currentElement, currentFont);
    if (b1.height() > lineheight) lineheight = b1.height();
    linewidth += b1.width();
  }

  if (lineheight > textheight) textheight = lineheight;
  if (linewidth > textwidth) textwidth = linewidth;

  /*
  std::cout << displayText << std::endl;
  std::cout << "Text dimensions = " << textwidth << "," << textheight 
	    << std::endl;  
  std::cout << "---" << std::endl;
  */

  int supersub, subx;
  QFont supersubfont;
  QColor drawcolor;
  if (highlighted)
    drawcolor = QColor(255,0,0);
  else
    drawcolor = color;
  QRect b(0, 0, textwidth, textheight);
  QPoint t = GetTopLeftPoint();
  b.moveBy( t.x(), t.y() );
  // this guarantees that the text will not overlap other objects
  r->drawFillBox(b.topLeft(), b.bottomRight(), r->getBGColor(), false,
		 QColor(0,0,0), 1);

  // draw shape, if applicable
  QPoint shapetl, shapebr, textcenter;
  oshapewidth = b.width() + maxfont; oshapeheight = b.height() + maxfont;
  textcenter.setX( b.left() + (b.width() / 2) );
  textcenter.setY( b.top() + (b.height() / 2) );
  int lsw, lsh;
  if (shapewidth < 0) {
    lsw = oshapewidth; lsh = oshapeheight;
  } else {
    lsw = shapewidth; lsh = shapeheight;
  }
  if (shape > 0) {
    shapetl.setX( textcenter.x() - (lsw / 2) );
    shapebr.setX( textcenter.x() + (lsw / 2) );
    shapetl.setY( textcenter.y() - (lsh / 2) );
    shapebr.setY( textcenter.y() + (lsh / 2) );
    if (shape == TEXT_SHAPE_RECTANGLE) {
      if (drawFill) {
	r->drawFillBox(shapetl, shapebr, fillColor, drawBorder, borderColor,0);
      } else {
	r->drawBox(shapetl, shapebr, borderColor);
      }
    }
    if (shape == TEXT_SHAPE_SQUARE) {
      if (drawFill) {
	r->drawFillBox(shapetl, shapebr, fillColor, drawBorder, borderColor,0);
      } else {
	r->drawBox(shapetl, shapebr, borderColor);
      }
    }
    if (shape == TEXT_SHAPE_ELLIPSE) {
      r->drawEllipse(shapetl, shapebr, drawBorder, borderColor, drawFill,
		     fillColor);
    }
    if (shape == TEXT_SHAPE_CIRCLE) {
      r->drawEllipse(shapetl, shapebr, drawBorder, borderColor, drawFill,
		     fillColor);
    }
  }

  // right-align behavior is different for stand-alone text
  QTextStream dts(&displayTextMask, IO_ReadOnly);
  QString displayLine;
  int in1 = 0;
  //displayLine = dts.readLine();
  //in1 = r->GetStringWidth(displayLine, font);
  //cout << b.width() << ":" << in1 << endl;
  in1 = lineWidths[0].toInt();
  in1 = b.width() - in1;
  //bool switchpos = false;
  QPoint curpos(t.x(), t.y() + r->GetTextHeight(font));
  if ( (tjustify == TEXT_RALIGN) && (justify == JUSTIFY_TOPLEFT) )
    curpos.setX(curpos.x() + in1 + 2);
  if ( (tjustify == TEXT_CALIGN) && (justify == JUSTIFY_TOPLEFT) )
    curpos.setX( curpos.x() + (in1 / 2) + 2);
  bool smaller_font = false;
  QPoint startpos = curpos;

  // actual rendering section

  int lw1 = 1;
  r->resetTextOrigin();
  for ( QStringList::Iterator it = renderList.begin(); it != renderList.end(); 
	++it ) {
    currentElement = *it;
    if (currentElement == "</span>") {
      boldFlag = false, italicFlag = false, underlineFlag = false,
	superFlag = false, subFlag = false;      
      continue;
    }
    if ( currentElement == "</p>" ) {
      if (justify == JUSTIFY_CENTER) continue;
      r->resetTextOrigin();
      //displayLine = dts.readLine();
      //in1 = r->GetStringWidth(displayLine, font);
      //cout << b.width() << ":" << in1 << endl;
      in1 = lineWidths[lw1].toInt();
      lw1++;
      in1 = b.width() - in1;
      curpos.setX(t.x());
      if ( (tjustify == TEXT_RALIGN) && (justify == JUSTIFY_TOPLEFT) )
	curpos.setX(curpos.x() + in1 + 2);
      if ( (tjustify == TEXT_CALIGN) && (justify == JUSTIFY_TOPLEFT) )
	curpos.setX( curpos.x() + (in1 / 2) + 2);
      curpos.setY(curpos.y() + r->GetTextFullHeight(font));
      continue;
    }

    if (currentElement.left(6) == "<span ") {
      // set font
      //std::cout << "span: " << currentElement << std::endl;
      boldFlag = false, italicFlag = false, underlineFlag = false,
	superFlag = false, subFlag = false;      
      if (currentElement.contains("font-family") > 0) {
	lp2 = currentElement.find("font-family");
	lp1 = currentElement.find(":", lp2);
	lp2 = currentElement.find(";", lp1);
	currentFont.setFamily( currentElement.mid(lp1 + 1, lp2 - lp1 - 1) );
      }
      if (currentElement.contains("font-size") > 0) {
	lp2 = currentElement.find("font-size");
	lp1 = currentElement.find(":", lp2);
	lp2 = currentElement.find("pt", lp1);
	currentFont.setPointSize( currentElement.mid(lp1 + 1, lp2 - lp1 - 1).toInt() );
	if ( r->getFontKludge() == true ) {
	  currentFont.setPointSize( currentFont.pointSize() * 2 );
	  std::cout << "fontkludge" << std::endl;
	}
      }
      currentFont.setBold(false);
      currentFont.setItalic(false);
      currentFont.setUnderline(false);
      if (currentElement.contains("font-weight") > 0) {
	boldFlag = true;
	currentFont.setBold(true);
      }
      if (currentElement.contains("italic") > 0) {
	italicFlag = true;
	currentFont.setItalic(true);
      }
      if (currentElement.contains("underline") > 0) {
	underlineFlag = true;
	currentFont.setUnderline(true);
      }
      if (currentElement.contains("super") > 0) {
	superFlag = true;
      }
      if (currentElement.contains("sub") > 0) {
	subFlag = true;
      }
      currentColor = QColor(0,0,0);
      if (currentElement.contains("color") > 0) {
	lp2 = currentElement.find("color");
	lp1 = currentElement.find(":", lp2);
	currentColor.setNamedColor( currentElement.mid(lp1 + 1, 7) );
      }
      continue;
    }

    supersub = r->GetTextHeight(currentFont) / 2;
    supersubfont = subscriptFont(currentFont);
    subx = r->GetTextHeight(supersubfont) / 2;

    if (highlighted) currentColor = QColor(255,0,0);

    if (superFlag == true) {
      r->drawString(currentElement, QPoint(curpos.x(), curpos.y()-supersub), 
		    currentColor, supersubfont);
      smaller_font = true;
    }
    if (subFlag == true) {
      r->drawString(currentElement, QPoint(curpos.x(), curpos.y()+subx), 
		    currentColor, supersubfont);
      smaller_font = true;
    }
    if (smaller_font == true) {
      curpos.setX(curpos.x() + r->GetStringWidth(currentElement,supersubfont));
    } else {
      r->drawString(currentElement, curpos, currentColor, currentFont);
      curpos.setX(curpos.x() + r->GetStringWidth(currentElement,currentFont));
    }
    smaller_font = false;
  }
}

int Text::Type(void)
{
  return TYPE_TEXT;
}

bool Text::Find(DPoint *target) {
  //if (start == target) return true;
  return false;
}

DPoint * Text::FindNearestPoint(DPoint *target, double &dist) {
  dist = 99999.0;
  return 0;
}

Drawable * Text::FindNearestObject(DPoint *target, double &dist) {
  if (WithinBounds(target))
    dist = 0.01;
  else
    dist = 99999.0;
  return this;
}

void Text::setPoint(DPoint *s) {
  start = s;
}

bool Text::WithinBounds(DPoint *target) {
  QRect b = r->GetTextDimensions(start->element, font);
  QPoint t = GetTopLeftPoint();
  b.moveBy( t.x(), t.y() );
  if ( (target->x > b.left()) && (target->x < b.right()) &&
       (target->y > b.top()) && (target->y < b.bottom()) )
    return true;
  else
    return false;
}

QRect Text::BoundingBox() {
  if (highlighted == false)
    return QRect( QPoint(999,999), QPoint(0,0) ); 
  QRect b = r->GetTextDimensions(start->element, font);
  QPoint t = GetTopLeftPoint();
  b.moveBy( t.x(), t.y() );
  return b;
}

// return nearest center point to m (see ChemData::AutoLayout)
QPoint Text::NearestCenter(QPoint m, int di, int &ns) {
  QRect b = r->GetTextDimensions(start->element, font);
  QPoint t = GetTopLeftPoint();
  b.moveBy( t.x(), t.y() );
  QPoint c1, cmin;
  double dist = 9999.0, ndist;
  if (di == ARROW_VERTICAL) {
    c1.setX(b.left());
    c1.setY(b.center().y());
    ndist = DistanceBetween(m, c1);
    if (ndist < dist) { cmin = c1; dist = ndist; ns = TEXT_LEFT; }
    c1.setX(b.right());
    c1.setY(b.center().y());
    ndist = DistanceBetween(m, c1);
    if (ndist < dist) { cmin = c1; dist = ndist; ns = TEXT_RIGHT; }
  } else { // ARROW_HORIZONTAL
    c1.setX(b.center().x());
    c1.setY(b.top());
    ndist = DistanceBetween(m, c1);
    if (ndist < dist) { cmin = c1; dist = ndist; ns = TEXT_TOP; }
    c1.setX(b.center().x());
    c1.setY(b.bottom());
    ndist = DistanceBetween(m, c1);
    if (ndist < dist) { cmin = c1; dist = ndist; ns = TEXT_BOTTOM; }
  }
  return cmin;
}

void Text::DeleteKeyPressed() {
  std::cerr << "Delete key" << endl;
  start->element.remove(cursor, 1);
  start->elementmask.remove(cursor, 1);
}

void Text::InsertCharacter(QKeyEvent *k1) {
  // if shift pressed, start new selection
  if (k1->state() == ShiftButton) {
    if (shiftdown == false) {
      shiftdown = true;
      selectMin = cursor;
      selectMax = cursor - 1;
      cout << cursor << "-" << selectMin << "-" << selectMax << endl;
    } else {
      shiftdown = true;
    }
  } else {
    shiftdown = false;
  }
  
  // if return pressed, add newline only if JUSTIFY_TOPLEFT (not a label)
  if ( (k1->key() == Key_Return) && (justify == JUSTIFY_CENTER) ) return;

  if (k1->key() == Key_Return) {
    cout << "Return" << endl;
    start->element.insert(cursor, (char)10);
    //start->element = text;
    start->elementmask.insert(cursor, (char)10);
    cursor++;
    return;
  }
  // if left or right arrow pressed with shift, make or extend new selection
  if ( (k1->key() == Key_Left) && shiftdown) {
    cout << cursor << "-" << selectMin << "-" << selectMax << endl;
    if (selectMin > 0) {
      if (cursor > selectMin)
	selectMax--;
      else
	selectMin--;
    }
    if (cursor > 0) cursor--;
    return;
  }
  if ( (k1->key() == Key_Right) && shiftdown) {
    cout << cursor << "-" << selectMin << "-" << selectMax << endl;
    if (selectMax < start->element.length()) {
      if (cursor <= selectMax)
	selectMin++;
      else
	selectMax++;
    }
    if (cursor < start->element.length()) cursor++;
    return;
  }
  // if Home or End pressed with shift, make or extend new selection
  if ( (k1->key() == Key_Home) && shiftdown) {
    cout << cursor << "-" << selectMin << "-" << selectMax << endl;
    if (selectMin > 0) {
      if (cursor > selectMin)
	selectMax--;
      else
	selectMin = 0;
    }
    if (cursor > 0) cursor = 0;
    return;
  }
  if ( (k1->key() == Key_End) && shiftdown) {
    cout << cursor << "-" << selectMin << "-" << selectMax << endl;
    if (selectMax < start->element.length()) {
      if (cursor <= selectMax)
	selectMin++;
      else
	selectMax = start->element.length();
    }
    if (cursor < start->element.length()) cursor = start->element.length();
    return;
  }
  // if left or right arrow pressed w/o shift, clear selection
  if (k1->key() == Key_Left) {
    if (cursor > 0) cursor--;
    selectMin = -1; selectMax = -1;
    return;
  }
  if (k1->key() == Key_Right) {
    if (cursor < start->element.length()) cursor++;
    selectMin = -1; selectMax = -1;
    return;
  }
  // if Home or End was pressed, go home or end
  if (k1->key() == Key_Home) {
    if (cursor > 0) cursor = 0;
    selectMin = -1; selectMax = -1;
    return;
  }
  if (k1->key() == Key_End) {
    if (cursor < start->element.length()) cursor = start->element.length();
    selectMin = -1; selectMax = -1;
    return;
  }
  if (k1->key() == Key_Backspace) {
    if (cursor == 0) return;
    if ( (selectMin >= 0) && (selectMax >= 1) ) { // something selected
      start->element.remove(selectMin, selectMax - selectMin + 1);
      start->elementmask.remove(selectMin, selectMax - selectMin + 1);
      //start->element = text;
      cursor = selectMin;
      selectMin = -1, selectMax = -1;
      return;
    }
    start->element.remove(cursor - 1, 1);
    //start->element = text;
    start->elementmask.remove(cursor - 1, 1);
    cursor--;
    return;
  }

  // if key > 96 and not already handled, ignore (letters/numbers/space < 96)
  // note that this breaks international layouts and is probably not needed
  //if (k1->key() > 96) return;
  //
  // this is a more useful test in a Unicode world
  //std::cout << "Key text:" << k1->text() << ":" << std::endl;
  QChar kc1 = k1->text().at(0);
  if (kc1.isPrint() == false) return;
  //std::cout << kc1 << "(key accepted)" << std::endl;

  // regular letter/number pressed
  start->element.insert(cursor, k1->text());
  //start->element = text;
  start->elementmask.insert(cursor, QChar(' '));
  cursor++;
}

void Text::InsertString(QString pt) {
  start->element.insert(cursor, pt);
  pt.fill(' ');
  start->elementmask.insert(cursor, pt);
  cursor += pt.length();
}

// Superscript selected text
void Text::DoSuperscript() {
  if (selectMin < 0) return;
  for (int i = selectMin; i < selectMax+1; i++) {
    if (start->elementmask[i] == ' ') {
      start->elementmask[i] = '+';
      continue;
    }
    if (start->elementmask[i] == '+') {
      start->elementmask[i] = ' ';
      continue;
    }
    if (start->elementmask[i] == '-') {
      start->elementmask[i] = '+';
      continue;
    }
  }
}

// Subscript selected text
void Text::DoSubscript() {
  if (selectMin < 0) return;
  for (int i = selectMin; i < selectMax+1; i++) {
    if (start->elementmask[i] == ' ') {
      start->elementmask[i] = '-';
      continue;
    }
    if (start->elementmask[i] == '+') {
      start->elementmask[i] = '-';
      continue;
    }
    if (start->elementmask[i] == '-') {
      start->elementmask[i] = ' ';
      continue;
    }
  }
}

// Bold selected text
void Text::DoBold() {
  if (selectMin < 0) return;
  for (int i = selectMin; i < selectMax+1; i++) {
    if (start->elementmask[i] == ' ') {
      start->elementmask[i] = 'B';
      continue;
    }
    if (start->elementmask[i] == 'B') {
      start->elementmask[i] = ' ';
      continue;
    }
  }
}

// Italicize selected text
void Text::DoItalic() {
  if (selectMin < 0) return;
  for (int i = selectMin; i < selectMax+1; i++) {
    if (start->elementmask[i] == ' ') {
      start->elementmask[i] = 'I';
      continue;
    }
    if (start->elementmask[i] == 'I') {
      start->elementmask[i] = ' ';
      continue;
    }
  }
}

// Underline selected text
void Text::DoUnderline() {
  if (selectMin < 0) return;
  for (int i = selectMin; i < selectMax+1; i++) {
    if (start->elementmask[i] == ' ') {
      start->elementmask[i] = 'U';
      continue;
    }
    if (start->elementmask[i] == 'U') {
      start->elementmask[i] = ' ';
      continue;
    }
  }
}

// move cursor to target
void Text::MoveCursor(DPoint *target) {
  selectMin = -1; selectMax = -1;
  double mindist = 99999.0, ldist;
  int newcur;
  DPoint *e = new DPoint;
  cout << "Move" << endl;
  if (WithinBounds(target) == false) return;
  QPoint t = GetTopLeftPoint();
  int lx = t.x(), ly = t.y() + (r->GetTextFullHeight(font) / 2);
  for (int i = 0; i < start->element.length(); i++) {
    e->x = lx; e->y = ly;
    ldist = e->distanceTo(target);
    if (ldist < mindist) { mindist = ldist; newcur = i; }
    if ((int)QChar(start->element[i]) == 10) {
      lx = t.x();
      ly = ly + r->GetTextFullHeight(font);
    } else {
      lx = lx + r->GetCharWidth(start->element[i], font);
    }
  }
  cursor = newcur;
  delete e;
}

// Select text between endpoints e1 and e2
void Text::Select(DPoint *e1, DPoint *e2) {
  if (WithinBounds(e1) == false) return;
  if (WithinBounds(e2) == false) return;
  cout << "select" << endl;
  double mindist = 99999.0, ldist;
  int newcur;
  int cr1 = 0, cr2 = 0;
  QPoint t;
  int i, lx, ly;
  DPoint *e = new DPoint;
  t = GetTopLeftPoint();
  lx = t.x(); ly = t.y() + (r->GetTextFullHeight(font) / 2);
  for (i = 0; i < start->element.length(); i++) {
    e->x = lx; e->y = ly;
    ldist = e->distanceTo(e1);
    if (ldist < mindist) { mindist = ldist; newcur = i; }
    if ((int)QChar(start->element[i]) == 10) {
      cr1++;
      lx = t.x();
      ly = ly + r->GetTextFullHeight(font);
    } else {
      lx = lx + r->GetCharWidth(start->element[i], font);
    }
  }
  selectMin = newcur;
  mindist = 99999.0;
  t = GetTopLeftPoint();
  lx = t.x(); ly = t.y() + (r->GetTextFullHeight(font) / 2);
  for (i = 0; i < start->element.length(); i++) {
    e->x = lx; e->y = ly;
    ldist = e->distanceTo(e2);
    if (ldist < mindist) { mindist = ldist; newcur = i; }
    if ((int)QChar(start->element[i]) == 10) {
      cr2++;
      lx = t.x();
      ly = ly + r->GetTextFullHeight(font);
    } else {
      lx = lx + r->GetCharWidth(start->element[i], font);
    }
  }
  selectMax = newcur;
  if (selectMin > selectMax) { 
    int swp = selectMin;
    selectMin = selectMax;
    selectMax = swp - 1;
  }
  cout << selectMin << " " << selectMax << endl;
  delete e;
}

QPoint Text::GetTopLeftPoint(void) {
  bool leftcenter = true;
  if(justify == JUSTIFY_TOPLEFT) {
    return start->toQPoint();
  } else {
    QPoint a;
    QRect b = r->GetTextDimensions(start->element, font);
    // center on first or last character.  Try to guess what user intended
    // (a dangerous idea at best :)
    if ( (start->element.left(1) == "H") && 
	 (start->element.length() > 1) ) leftcenter = false;
    if ( (start->element.left(1) == "+") && 
	 (start->element.length() > 1) ) leftcenter = false;
    if ( (start->element.left(1) == "-") && 
	 (start->element.length() > 1) ) leftcenter = false;
    if ( start->element.at(0).isNumber() == true) leftcenter = false;
    if (whichside == 2) leftcenter = false;
    if (tjustify == TEXT_LALIGN) leftcenter = true;
    if (tjustify == TEXT_CALIGN) leftcenter = true;
    if (tjustify == TEXT_RALIGN) leftcenter = false;
    if (leftcenter) {
      int lc = r->GetCharWidth(start->element.at(0), font);
      a.setX( my_round(start->x - lc / 2.0) );
    } else {
      int rc = r->GetCharWidth(start->element.at(start->element.length() - 1), font);
      a.setX( my_round(start->x - b.width() + rc / 2.0) );
    }
    a.setY( my_round(start->y - b.height() / 2.0) );
    return a;
  }
}

// position atom labels according to hindrance
void Text::CheckAlignment(int alignside) {
  // 1 = left, 2 = right
  whichside = alignside;
  std::cout << "whichside = " << whichside << std::endl;
}

void Text::ForceAlignment(int alignside) {
  tjustify = alignside;
}

// find next smaller font for subscripts
QFont Text::subscriptFont(QFont f_in) {
  QFont f_out = f_in;

  if (f_out.pointSize() > 1) 
    f_out.setPointSize(f_out.pointSize() - 2);
  else
    f_out.setPixelSize(f_out.pixelSize() - 2);

  return f_out;
}

Drawable * Text::DeepCopy() {
  Text *np = new Text(r);
  DPoint *s1 = new DPoint(start);
  np->setPoint(s1);
  np->setText(getText());
  np->setTextMask(getTextMask());
  np->SetColor(GetColor());
  np->setFont(getFont());
  np->setJustify(Justify());
  np->ForceAlignment(tjustify);
  if (highlighted) np->Highlight(true);
  return np;
}

void Text::UpdateDisplayText() {
  return;
  // this function doesn't do anything now that we're using RichText

  // old code follows

  // use user string if justify chosen
  if (tjustify != TEXT_AUTO) {
    displayText = start->element;
    displayTextMask = start->elementmask;
    return;
  }
  
  // validate saved text, make sure format is 'appropriate'
  if (start->element == "MeO")
    start->element = "OMe";

  // decide how to display the text string
  if (whichside == 1) {
    //cerr << "left hindered (normal) version" << endl;
    displayText = start->element;
    displayTextMask = start->elementmask;
    return;
  }
  // create right-aligned version.
  //cerr << "right hindered version" << endl;
  if (start->element == "OMe") {
    displayText = "MeO";
    displayTextMask = "   ";
    return;
  }
  displayText = start->element;
  displayTextMask = start->elementmask;
}
