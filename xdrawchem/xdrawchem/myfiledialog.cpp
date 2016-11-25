#include "myfiledialog.h"

MyFileDialog::MyFileDialog( QWidget* parent, QString name ) :
  QFileDialog( parent, name, true )
{
  trans_toggle = new QCheckBox( tr("Transparent"), this );
  trans_toggle->setChecked(false);
  addWidgets( 0, trans_toggle, 0 );
}
  
bool MyFileDialog::isTransparent() { return trans_toggle->isChecked(); }
