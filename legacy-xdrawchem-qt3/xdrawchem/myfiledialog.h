#ifndef MYFILEDIALOG_H
#define MYFILEDIALOG_H

#include <qfiledialog.h>
#include <qcheckbox.h>

class MyFileDialog:public QFileDialog {
	Q_OBJECT
public:
	MyFileDialog(QWidget *parent, QString name);
	bool isTransparent();

private:
	QCheckBox *trans_toggle;
};

#endif
