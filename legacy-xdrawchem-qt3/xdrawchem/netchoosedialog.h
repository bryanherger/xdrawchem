#ifndef NETCHOOSEDIALOG_H
#define NETCHOOSEDIALOG_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qstringlist.h>

class NetChooseDialog:public QDialog {
	Q_OBJECT
public:
	NetChooseDialog(QWidget *parent, QString name, QStringList);
	QString getFile() { return fn; }

public slots:
	  void OK();

private:
 QString fn;
 QStringList results;
 QListView *lv;
};

#endif
