#ifndef PEPTDIALOG_H
#define PEPTDIALOG_H

#include <qlineedit.h>
#include <qdialog.h>
#include <qcheckbox.h>

class PeptDialog:public QDialog {
	Q_OBJECT
public:
	PeptDialog(QWidget *parent, QString name);
	QString getPeptide();

	public slots:
	  void buttonPress(int);

private:
	QLineEdit *peptide;
};

#endif
