#include <QTextEdit>
#include <QKeyEvent>

// custom text editor widget to handle enter key...
class XdcTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    XdcTextEdit( QWidget * parent ) : QTextEdit( parent )
    {
    }

    void keyPressEvent(QKeyEvent *event)
    {
        if (event->key() == Qt::Key_Return)
        {
            emit returnPressed();
        }
        else
        {
            QTextEdit::keyPressEvent(event);
        }
    }

signals:
void returnPressed();

};

