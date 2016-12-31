#ifndef GRAPHDATA_H
#define GRAPHDATA_H

class GraphData {
 public:
  double value;
  QString label;
  QString fulltext;
  int intensity;
  bool drawlabel;
  QColor pcolor;
  GraphData() { drawlabel = false; intensity = 1; pcolor = QColor(0,0,0); }
};

#endif
