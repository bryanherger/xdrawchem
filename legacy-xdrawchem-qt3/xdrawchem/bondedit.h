#ifndef BOND_EDIT_DIALOG_H
#define BOND_EDIT_DIALOG_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcolor.h>
#include <qpushbutton.h>

#include "previewwidget.h"
#include "dpoint.h"

class BondEditDialog:public QDialog {
	Q_OBJECT
public:
	BondEditDialog(QWidget *parent, QString name, DPoint *s, DPoint *e, int ty, int o, int d, int th, int st, QColor c1);
	int Style() { return style; }
	int Order() { return order; }
	int Dash() { return dash; }
	QColor Color() { return color; }
	int Thick() { return thick; }
	int DoubleBond() { return dbond; }

public slots:
	  void GetNewColor();
void StyleChanged(int s1) {
  style = s1;
  pw->Update(type, thick, dash, order, style);
}
 void OrderChanged(int o1) {
   if (o1 == 0) { order = 1; dash = 1; }
   if (o1 == 1) { order = 1; dash = 0; }
   if (o1 == 2) { order = 2; dash = 0; }
   if (o1 == 3) { order = 3; dash = 0; }
   if (o1 == 5) { order = 5; dash = 0; }
   if (o1 == 6) { order = 6; dash = 0; }
   if (o1 == 7) { order = 7; dash = 0; }
   if (o1 == 11) { order = 2; dash = 1; }
   if (o1 == 12) { order = 3; dash = 1; }
   pw->Update(type, thick, dash, order, style);
 }
 void SetThick(int tnew) {
   thick = tnew + 1;
   pw->Update(type, thick, dash, order, style);
 }
 void SetDoubleBond(int tnew) {
   dbond = tnew;
   dbList->setCurrentItem(dbond);
 }

private:
 QPushButton *pb1; 
 QComboBox *ltList, *dbList;
	DPoint *s1, *e1;
	QColor color;
	int type;
	int order, dash, thick;
	int ahead, style, dbond;
	PreviewWidget *pw;
};

#endif
