#ifndef PSDIALOG_H
#define PSDIALOG_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>

class PageSetupDialog:public QDialog {
	Q_OBJECT
public:
	PageSetupDialog(QWidget *parent, QString name);
	void setPageSize(int);
	int getPageSize(void);
	void setOrientation(int);
	int getOrientation(void);

	public slots:
	  void onSuccessful();
	void SwapWH(int);

private:
	QComboBox *psize, *porient;
	QLineEdit *pwidth, *pheight;
	QLabel *ul1, *ul2;
	int papersize, orient, ps_set, po_set;
};

#endif
