#ifndef SMILESDIALOG_H
#define SMILESDIALOG_H

#include <qlineedit.h>
#include <qdialog.h>

class SmilesDialog:public QDialog {
	Q_OBJECT
public:
	SmilesDialog(QWidget *parent, QString name);
	QString getSMILES(void);

private:
	QLineEdit *tf_smiles;
};

#endif
