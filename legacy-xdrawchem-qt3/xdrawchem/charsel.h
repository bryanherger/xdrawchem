#ifndef CHARDIALOG_H
#define CHARDIALOG_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qfont.h>

class CharSelLabel:public QLabel {
  Q_OBJECT
 public:
  CharSelLabel(QWidget *parent);

 signals:
  void thisChar(int);

 protected:
  void mousePressEvent(QMouseEvent *);
};

class CharSelDialog:public QDialog {
	Q_OBJECT
public:
	CharSelDialog(QWidget *parent, QString name);
	QString selectedChar() { return thischar; }
	QFont selectedFont() { return symbolFont; }

	public slots:
	void fontChange(int);
	void chosenChar(int);

private:
	QComboBox *charfont;
	CharSelLabel *letters;
	QString thischar;
	QFont symbolFont;
	int papersize, orient, ps_set, po_set;
};

#endif
