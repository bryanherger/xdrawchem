// CDXML_Reader.cpp - function definitions for class CDXML_Reader

#include <iostream>
#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qcolor.h>
#ifdef UNIX
#include <unistd.h>
#endif

using std::cout;
using std::endl;

#include "cdxml_reader.h"
#include "chemdata.h"
#include "dpoint.h"
#include "drawable.h"
#include "text.h"
#include "defs.h"

using std::cout;

extern Preferences preferences;

CDXML_Reader::CDXML_Reader(ChemData *c1) { 
  nodedepth = 0; fragdepth = 0;
  c = c1; 
  tmp_color = new ColorTableEntry;
  tmp_color->index = 0;
  tmp_color->color = QColor(0,0,0);
  colors.append(tmp_color);
  tmp_color = new ColorTableEntry;
  tmp_color->index = 1;
  tmp_color->color = QColor(255,255,255);
  colors.append(tmp_color);
}

bool CDXML_Reader::ReadFile(QString fn) {
  cout << endl << "New and improved CDXML parser" << endl << endl;
  int i1 = fn.find("<CDXML");
  int i2 = fn.find("</CDXML>") + 8;
  ParseDocument(fn.mid(i1, i2 - i1));
  Build();
  cout << endl << "Done parsing!" << endl << endl;
  return false;
}

void CDXML_Reader::ParseDocument(QString dtag) {
  int i1, i2;
  // find and parse color table
  i1 = dtag.find("<colortable>");
  i2 = dtag.find("</colortable>") + 13;
  if (i1 >= 0) {
    ParseColorTable(dtag.mid(i1, i2 - i1));
    dtag.remove(i1, i2 - i1);
  }
  // if no color table, set default foreground and background
  if (colors.count() < 3) {
    tmp_color = new ColorTableEntry;
    tmp_color->index = 2;  // background
    tmp_color->color = QColor(255,255,255);
    colors.append(tmp_color);
    tmp_color = new ColorTableEntry;
    tmp_color->index = 3; // foreground
    tmp_color->color = QColor(0,0,0);
    colors.append(tmp_color);
  }
  // find and parse font table
  i1 = dtag.find("<fonttable>");
  i2 = dtag.find("</fonttable>") + 12;
  if (i1 >= 0) {
    ParseFontTable(dtag.mid(i1, i2 - i1));
    dtag.remove(i1, i2 - i1);
  }
  // find and parse page(s)
  do {
    i1 = dtag.find("<page");
    i2 = dtag.find("</page>") + 7;
    if (i1 < 0) break;
    ParsePage(dtag.mid(i1, i2 - i1));
    dtag.remove(i1, i2 - i1);
  } while (1);

}

void CDXML_Reader::ParseColorTable(QString ctable) {
  //cout << ctable << endl;
  QString nexttag;
  int ptr = 0, idx = 2;
  do {
    nexttag = ReadTag(ctable, ptr);
    if (nexttag.left(11) == "<colortable") continue;
    if (nexttag == "</colortable>") break;
    if (SelfContainedTag(nexttag) == false) {
      //cout << "Problem color tag: " << nexttag << endl;
      continue;
    }
    tmp_color = new ColorTableEntry;
    tmp_color->index = idx;
    idx++;
    tmp_color->color = ParseColor(nexttag);
    colors.append(tmp_color);
  } while (1);  // yeah, I know, looks bad  :)
}

QColor CDXML_Reader::ParseColor(QString ctag) {
  QStringList attr;
  QString a1, v1;
  QColor retval;
  int re,gr,bl;
  attr = ReadAttr(ctag);
  for (int cc = 0; cc < 5; cc = cc + 2) {
    a1 = attr[cc]; v1 = attr[cc + 1];
    if (a1 == "r") 
      re = (int)(v1.toDouble() * 255.0);
    if (a1 == "g") 
      gr = (int)(v1.toDouble() * 255.0);
    if (a1 == "b") 
      bl = (int)(v1.toDouble() * 255.0);
  }
  retval.setRgb(re,gr,bl);
  return retval;
}

void CDXML_Reader::ParseFontTable(QString ftable) {
  //cout << ftable << endl;
  QString nexttag;
  int ptr = 0;
  do {
    nexttag = ReadTag(ftable, ptr);
    if (nexttag.left(10) == "<fonttable") continue;
    if (nexttag == "</fonttable>") break;
    if (SelfContainedTag(nexttag) == false) {
      //cout << "Problem font tag: " << nexttag << endl;
      continue;
    }
    fonts.append(ParseFont(nexttag));
  } while (1);  // yeah, I know, looks bad  :)
}

FontTableEntry * CDXML_Reader::ParseFont(QString ftag) {
  QStringList attr;
  QString a1, v1;
  attr = ReadAttr(ftag);
  tmp_font = new FontTableEntry;
  for (int cc = 0; cc < attr.count(); cc = cc + 2) {
    a1 = attr[cc]; v1 = attr[cc + 1];
    if (a1 == "name") tmp_font->fam = v1;
    if (a1 == "id") tmp_font->id = v1;
  }
  return tmp_font;
}

void CDXML_Reader::ParsePage(QString ptag) {
  // parse out known subobjects
  int i1, i2;
  bool flag;
  do {
    flag = false;
    // find fragments
    i1 = ptag.find("<fragment");
    if (i1 >= 0) {
      i2 = i1 + PositionOfEndTag(ptag.mid(i1), "fragment");
      ParseFragment(ptag.mid(i1, i2 - i1));
      ptag.remove(i1, i2 - i1);
      flag = true;
    }
    // find text
    //i1 = ptag.find("<t");
    //if (i1 >= 0) {
    //  i2 = ptag.find("</t>") + 4;
    //  ParseText(ptag.mid(i1, i2 - i1));
    //  ptag.remove(i1, i2 - i1);
    //  flag = true;
    //}
  } while (flag);  // repeat as necessary
  // find text and graphics not contained in fragments
  do {
    flag = false;
    i1 = ptag.find("<t");
    if (i1 >= 0) {
      i2 = i1 + PositionOfEndTag(ptag.mid(i1), "t");
      cout << "toplevel" << endl;
      QString rs = ParseText(ptag.mid(i1, i2 - i1));
      tmp_node = new DPoint;
      tmp_node->x = globalx;
      tmp_node->y = globaly;
      tmp_node->element = rs;
      nodelist.append(tmp_node);
      ptag.remove(i1, i2 - i1);
      flag = true;
    }
    i1 = ptag.find("<graphic");
    if (i1 >= 0) {
      i2 = ptag.find(">", i1) + 1;
      if (!SelfContainedTag(ptag.mid(i1, i2 - i1))) 
	cout << "Malformed <graphic>" << endl;
      cout << "toplevel" << endl;
      ParseGraphic(ptag.mid(i1, i2 - i1));
      ptag.remove(i1, i2 - i1);
      flag = true;
    }
  } while (flag);
}

// warning, fragments can contain nodes, which in turn can contain fragments.
// make sure you get the level right
void CDXML_Reader::ParseFragment(QString ftag) {
  fragdepth++;
  cout << endl << "<--frag-->" << endl;
  cout << ftag << endl << endl << endl;

  int i1, i2;
  bool flag;
  do {
    flag = false;
    // find nodes
    i1 = ftag.find("<n");
    i2 = ftag.find(">", i1) + 1;
    if (i1 >= 0) {
      if (!SelfContainedTag(ftag.mid(i1, i2 - i1))) 
	i2 = i1 + PositionOfEndTag(ftag.mid(i1), "n");
      ParseNode(ftag.mid(i1, i2 - i1));
      ftag.remove(i1, i2 - i1);
      flag = true;
    }
  } while (flag);
  do {
    flag = false;
    // find bonds (typically self-contained)
    i1 = ftag.find("<b");
    i2 = ftag.find(">", i1) + 1;
    if (i1 >= 0) {
      if (!SelfContainedTag(ftag.mid(i1, i2 - i1))) 
	cout << "Malformed <b>" << endl;
      ParseBond(ftag.mid(i1, i2 - i1));
      ftag.remove(i1, i2 - i1);
      flag = true;
    }
    // find graphics
    i1 = ftag.find("<graphic");
    if (i1 >= 0) {
      i2 = ftag.find(">", i1) + 1;
      if (!SelfContainedTag(ftag.mid(i1, i2 - i1))) 
	cout << "Malformed <graphic>" << endl;
      ParseGraphic(ftag.mid(i1, i2 - i1));
      ftag.remove(i1, i2 - i1);
      flag = true;
    }
  } while (flag);
  fragdepth--;
}

// note this reads <t> and the contained <s>
QString CDXML_Reader::ParseText(QString ttag) {
  cout << endl << "<--text-->" << endl;
  cout << ttag << endl << endl;

  int i1, i2, i3;
  QString a1, v1;
  // tokenize the <t> tag
  QStringList t_tokens;
  i1 = ttag.find(">");
  t_tokens = ReadAttr(ttag.left(i1 + 1));
  cout << "<--t_tokens-->" << endl;
  for (int c1 = 0; c1 < t_tokens.count(); c1 += 2) {
    a1 = *(t_tokens.at(c1));
    v1 = *(t_tokens.at(c1 + 1));
    cout << "-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
    // compare attribute a1 with list of relevant attr's
    if (a1.upper() == QString("P")) {
      i1 = v1.find(QString(" "));
      globalx = v1.mid(0, i1).toDouble();
      globaly = v1.mid(i1 + 1).toDouble();
    }
  }
  // tokenize the <s> tag
  QString actualtext;
  //QStringList s_tokens;
  //i1 = ttag.find("<s ");
  //i2 = ttag.find(">", i1) + 1;
  //s_tokens = ReadAttr(ttag.mid(i1, i2 - i1));
  //cout << "<--s_tokens-->" << endl;
  //for (int c1 = 0; c1 < s_tokens.count(); c1 += 2) {
  //  a1 = *(s_tokens.at(c1));
  //  v1 = *(s_tokens.at(c1 + 1));
  //  cout << "-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
    // compare attribute a1 with list of relevant attr's
  //}
  // read actual text
  //i1 = i2;
  //i2 = ttag.find("<", i1);
  //QString actualtext = ttag.mid(i1, i2 - i1);
  while ( (i1 = ttag.find("<s ")) > 0) {
    i2 = ttag.find(">", i1);
    i3 = ttag.find("</s>", i2);
    actualtext.append(ttag.mid(i2 + 1, i3 - i2 - 1));
    ttag.remove(i1, i3 - i1 + 4);
    cout << ttag << endl;
  };
  cout << "string = " << actualtext << endl;
  return actualtext;
}

void CDXML_Reader::ParseGraphic(QString gtag) {
  tmp_obj = new CDXML_Object;
  tmp_obj->type = TYPE_DRAWABLE;

  // tokenize the <graphic> tag
  QStringList tokens;
  QString a1, v1;
  int i1;
  DPoint *s1, *e1;

  i1 = gtag.find(">");
  tokens = ReadAttr(gtag.left(i1 + 1));
  cout << "<--graphictokens-->" << endl;
  for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
    a1 = *(tokens.at(c1));
    v1 = *(tokens.at(c1 + 1));
    cout << "-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
    // compare attribute a1 with list of relevant attr's
    if (a1.upper() == "BOUNDINGBOX") {
      s1 = new DPoint;
      e1 = new DPoint;
      QTextStream coords(&v1, IO_ReadOnly);
      coords >> s1->x >> s1->y >> e1->x >> e1->y;
      //cout << s1->x << " ";
      //cout << s1->y << " ";
      //cout << e1->x << " ";
      //cout << e1->y << endl;
      tmp_obj->start = s1;
      tmp_obj->end = e1;
    }
    // don't let GraphicType override special types...
    if ( (a1.upper() == "GRAPHICTYPE") && (tmp_obj->type == TYPE_DRAWABLE) ) {
      if (v1.upper() == "BRACKET") tmp_obj->type = TYPE_BRACKET;
    }
    if (a1.upper() == "ARROWTYPE") {
      tmp_obj->type = TYPE_ARROW;
      tmp_obj->idata2 = ARROW_REGULAR;
      if (v1.upper() == "RESONANCE") tmp_obj->idata2 = ARROW_BI1;
      if (v1.upper() == "EQUILIBRIUM") tmp_obj->idata2 = ARROW_BI2;
      if (v1.upper() == "RETROSYNTHETIC") tmp_obj->idata2 = ARROW_RETRO;
    }
    if (a1.upper() == "BRACKETTYPE") {
      tmp_obj->idata2 = BRACKET_SQUARE;
      if (v1.upper() == "SQUAREPAIR") tmp_obj->type = TYPE_BRACKET;
      if (v1.upper() == "ROUNDPAIR") {
	tmp_obj->type = TYPE_BRACKET;
	tmp_obj->idata2 = BRACKET_CURVE;
      }
      if (v1.upper() == "CURLYPAIR") {
	tmp_obj->type = TYPE_BRACKET;
	tmp_obj->idata2 = BRACKET_BRACE;
      }
    }
  }

  objectlist.append(tmp_obj);
}

// warning, nodes can contain fragments, which in turn can contain nodes.
// make sure you get the level right
void CDXML_Reader::ParseNode(QString ntag) {
  tmp_node = new DPoint;

  nodedepth++;
  cout << endl << "<--node-->" << endl;
  cout << ntag << endl << endl << endl;

  int i1, i2;
  bool flag;

  // tokenize the <n> tag
  QStringList tokens;
  QString a1, v1, nodetype;
  i1 = ntag.find(">");
  tokens = ReadAttr(ntag.left(i1 + 1));
  cout << "<--nodetokens-->" << endl;
  for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
    a1 = *(tokens.at(c1));
    v1 = *(tokens.at(c1 + 1));
    cout << "-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
    // compare attribute a1 with list of relevant attr's
    if (a1.upper() == QString("ID")) tmp_node->id = v1;
    if (a1.upper() == QString("NODETYPE")) nodetype = v1;
    if (a1.upper() == QString("P")) {
      i1 = v1.find(QString(" "));
      tmp_node->x = v1.mid(0, i1).toDouble();
      tmp_node->y = v1.mid(i1 + 1).toDouble();
    }
  }

  do {
    flag = false;
    // find nodes
    i1 = ntag.find("<fragment");
    if (i1 >= 0) {
      i2 = i1 + PositionOfEndTag(ntag.mid(i1), "fragment");
      // right now, we only care about top-level fragment/nodes
      //ParseFragment(ntag.mid(i1, i2 - i1));
      ntag.remove(i1, i2 - i1);
      flag = true;
    }
    // find text (strings)
    i1 = ntag.find("<t");
    if (i1 >= 0) {
      i2 = i1 + PositionOfEndTag(ntag.mid(i1), "t");
      QString rs = ParseText(ntag.mid(i1, i2 - i1));
      tmp_node->element = rs;
      ntag.remove(i1, i2 - i1);
      flag = true;
    }
  } while (flag);
  nodedepth--;

  nodelist.append(tmp_node);
}

void CDXML_Reader::ParseBond(QString btag) {
  tmp_obj = new CDXML_Object;
  tmp_obj->type = TYPE_BOND;
  tmp_obj->idata1 = 1;

  // tokenize the <b> tag
  QStringList tokens;
  QString a1, v1;
  int i1;

  i1 = btag.find(">");
  tokens = ReadAttr(btag.left(i1 + 1));
  cout << "<--bondtokens-->" << endl;
  for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
    a1 = *(tokens.at(c1));
    v1 = *(tokens.at(c1 + 1));
    cout << "-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
    // compare attribute a1 with list of relevant attr's
    if (a1.upper() == QString("ID")) tmp_obj->id = v1;
    if (a1.upper() == QString("B")) tmp_obj->start_id = v1;
    if (a1.upper() == QString("E")) tmp_obj->end_id = v1;
    if (a1.upper() == QString("ORDER")) tmp_obj->idata1 = v1.toUInt();
    if (a1.upper() == QString("DISPLAY")) {
      if (v1 == QString("WedgeBegin")) tmp_obj->idata1 = 5;
      if (v1 == QString("WedgedHashBegin")) tmp_obj->idata1 = 7;
    }
  }

  objectlist.append(tmp_obj);
}

DPoint *CDXML_Reader::FindNode(QString key) {
  for (tmp_node = nodelist.first(); tmp_node != NULL;
       tmp_node = nodelist.next()) {
    if (tmp_node->id == key) return tmp_node;
  }
  return NULL;
}

// build structure from data we read
void CDXML_Reader::Build() {
  QPtrList<DPoint> up;
  DPoint *s1, *e1;
  Text *nt;
  double bondlength = 0.0;
  int nbonds = 0;

  cout << "nodes: " << nodelist.count() << endl;
  cout << "objects: " << objectlist.count() << endl;

  // add all non-text objects
  for (tmp_obj = objectlist.first(); tmp_obj != NULL;
       tmp_obj = objectlist.next()) {
    if (tmp_obj->type == TYPE_BOND) {
      s1 = FindNode(tmp_obj->start_id);
      e1 = FindNode(tmp_obj->end_id);
      bondlength += s1->distanceTo(e1);
      nbonds++;
      c->addBond(s1, e1, 1, tmp_obj->idata1, QColor(0,0,0), true);
      s1->hit = true; e1->hit = true;
      if (up.find(s1) == -1) up.append(s1);
      if (up.find(e1) == -1) up.append(e1);
    }
    if (tmp_obj->type == TYPE_ARROW) {
      c->addArrow(tmp_obj->end, tmp_obj->start, QColor(0,0,0), 
		  tmp_obj->idata2, true);
      if (up.find(tmp_obj->start) == -1) up.append(tmp_obj->start);
      if (up.find(tmp_obj->end) == -1) up.append(tmp_obj->end);
    }
    if (tmp_obj->type == TYPE_BRACKET) {
      c->addBracket(tmp_obj->start, tmp_obj->end, QColor(0,0,0), 
		    tmp_obj->idata2, true);
      if (up.find(tmp_obj->start) == -1) up.append(tmp_obj->start);
      if (up.find(tmp_obj->end) == -1) up.append(tmp_obj->end);
    }
  }
  // add text
  for (tmp_node = nodelist.first(); tmp_node != NULL;
       tmp_node = nodelist.next()) {
    if (tmp_node->element != "C") {
      nt = new Text(c->getRender2D());
      nt->setPoint(tmp_node);
      if (up.find(tmp_node) == -1) up.append(tmp_node);
      if (tmp_node->hit) {  // true = part of molecule
	nt->setJustify(JUSTIFY_CENTER);
      } else {  // false = free-standing text
	nt->setJustify(JUSTIFY_TOPLEFT);
      }
      nt->setText(tmp_node->element);
      tmp_node->element.fill(' ');
      nt->setTextMask(tmp_node->element);
      nt->Highlight(true);
      c->addText(nt);
    }
  }
  // clear "hit" flag
  for (tmp_node = nodelist.first(); tmp_node != NULL;
       tmp_node = nodelist.next()) {
    tmp_node->hit = false;
  }
  // scale drawing
  double avglen = bondlength / (double)nbonds;
  double curfixed = preferences.getBond_fixedlength();
  double sf = curfixed / avglen;
  cout << "Scale:" << sf << endl;
  //sf = 1.0;
  double sl = 9999.0, sr = -9999.0, st = 9999.0, sb = -9999.0;
  for (tmp_node = up.first(); tmp_node != NULL; tmp_node = up.next() ) {
    tmp_node->x *= sf;
    tmp_node->y *= sf;
    if (tmp_node->x < sl) sl = tmp_node->x;
    if (tmp_node->x > sr) sr = tmp_node->x;
    if (tmp_node->y < st) st = tmp_node->y;
    if (tmp_node->y > sb) sb = tmp_node->y;
  }
  double tx = 50 - sl;
  double ty = 50 - st;
  for (tmp_node = up.first(); tmp_node != NULL; tmp_node = up.next() ) {
    tmp_node->x += tx;
    tmp_node->y += ty;
  }
}
