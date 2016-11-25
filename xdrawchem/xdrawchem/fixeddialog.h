#ifndef FIXEDDIALOG_H
#define FIXEDDIALOG_H

#include <qlineedit.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qcheckbox.h>
#include <qlabel.h>

class FixedDialog:public QDialog {
	Q_OBJECT
public:
	FixedDialog(QWidget *parent, QString name);
	double getLength_bond(void);
	double getAngle_bond(void);
	void setLength_bond(double);
	void setAngle_bond(double);
	double getLength_arrow(void);
	double getAngle_arrow(void);
	void setLength_arrow(double);
	void setAngle_arrow(double);
	double getDoubleBondOffset(void);
	void setDoubleBondOffset(double);
	void setAntiChecked(bool n1) { anti_toggle->setChecked(n1); }
	bool getAntiChecked() { return anti_toggle->isChecked(); }

	public slots:
	  void onSuccess();
	  void setDefaults();
	void setUnits(int);

private:
	QComboBox *unitCombo;
	QLineEdit *bond_length;
	QLineEdit *bond_angle;
	QLineEdit *arrow_length;
	QLineEdit *arrow_angle;
	QLineEdit *dbond_offset;
	QLineEdit *gridspacing;
	QCheckBox *anti_toggle, *snapgrid;
	QComboBox *showgrid;
	QLabel *ul1, *ul2, *ul3, *ul4, *ul5;
	int bl, al, dsp, gsp, prevunit;
};

#endif
