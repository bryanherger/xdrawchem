// XML_Reader.h - base class for XML type readers

#include <qstring.h>
#include <qstringlist.h>

class XML_Reader {
 public:
  // reimplement for subclasses
  virtual bool ReadFile(QString) { return false; }
  // utility functions
  bool SelfContainedTag(QString);
  QString ReadTag(QString, int &);
  QString ReadData(QString, int &);
  QStringList ReadAttr(QString);
  QStringList Tokenize(QString);
  void ParseBoundingBox(QString, double &, double &, double &, double &);
  int PositionOfEndTag(QString, QString);
};
