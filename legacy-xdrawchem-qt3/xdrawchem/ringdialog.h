#ifndef RINGDIALOG_H
#define RINGDIALOG_H

#include <qdialog.h>

class QComboBox;

class RingDialog:public QDialog {
	Q_OBJECT
public:
	RingDialog(QWidget *parent, QString name);
	QString getFile(void) { return fn; }

public slots:
	void fromComboBox(int);

private:
	QString fn;
	QComboBox *filelist;
};

#endif
