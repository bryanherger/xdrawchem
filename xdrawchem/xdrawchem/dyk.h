#ifndef DYKDIALOG_H
#define DYKDIALOG_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qstringlist.h>

#include "prefs.h"
extern Preferences preferences;

class DYKDialog:public QDialog {
	Q_OBJECT
public:
	DYKDialog(QWidget *parent = 0, QString name = "");

	public slots:
	  void nextTip();
	void prevTip();
	void finished();

private:
	void populateTipList();

	QCheckBox *futureshow;
	QLabel *curtip;
	QStringList tips;
	int tipcount;
};

#endif
