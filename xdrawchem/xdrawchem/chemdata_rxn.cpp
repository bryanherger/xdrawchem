#include <iostream>
#include <qobject.h>
#include <qptrlist.h>
#include <qmessagebox.h>
#include <qstringlist.h>

using std::cout;
using std::endl;

#include "graphdialog.h"
#include "tool_1h_nmr.h"
#include "tool_13c_nmr.h"
#include "drawable.h"
#include "molecule.h"
#include "bond.h"
#include "text.h"
#include "chemdata.h"
#include "defs.h"

#include "boshcp.h"

using std::cout;

void ChemData::ReactionAnalysis(int flags) {
  QPtrList<Molecule> reactantList;
  QPtrList<Molecule> productList;
  Tool_1HNMR_Dialog tool1hnmr;
  Tool_13CNMR_Dialog tool13cnmr;

  Molecule *tmp_mol;
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    if (tmp_draw->Type() == TYPE_MOLECULE) {
      tmp_mol = (Molecule *)tmp_draw;
      if (tmp_mol->groupType() == GROUP_REACTANT) reactantList.append(tmp_mol);
      if (tmp_mol->groupType() == GROUP_PRODUCT) productList.append(tmp_mol);
    }
  }

  if ( (reactantList.count() == 0) || (productList.count() == 0) ) {
    QMessageBox::information( 0, tr("Missing reactant or product"),
			      tr("You must select at least one reactant and one product.") );
    return;
  }

  cout << "Reaction analysis called" << endl;

  if (flags == RXN_ENTHALPY_ESTIMATE) {
    cout << "Enthalpy change" << endl;
    QString summary_report = tr("Estimated gas-phase enthalpy change:\n");
    double this_dh, reactant_dh = 0.0, product_dh = 0.0;
    int x = 1;
    for (tmp_mol = reactantList.first(); tmp_mol != 0;
	 tmp_mol = reactantList.next() ) {
      this_dh = tmp_mol->SumBondEnthalpy();
      reactant_dh += this_dh;
      x++;
    }
    summary_report.append(tr("Reactants = %1 kJ/mol\n").arg(reactant_dh));
    x = 1;
    for (tmp_mol = productList.first(); tmp_mol != 0;
	 tmp_mol = productList.next() ) {
      this_dh = tmp_mol->SumBondEnthalpy();
      product_dh += this_dh;
      x++;
    }
    product_dh *= -1.0;
    summary_report.append(tr("Products = %1 kJ/mol\n").arg(product_dh));
    this_dh = reactant_dh + product_dh;
    summary_report.append(tr("Reaction = %1 kJ/mol").arg(this_dh));
    QMessageBox::information(0, tr("Enthalpy change"), summary_report);
    return;
  }  // enthalpy

  if (flags == RXN_1HNMR) {
    int c1, c2, c3, mul;
    double shift1;
    QString n1, nfull, ntemp;

    if ( (reactantList.count() > 1) || (productList.count() > 1) ) {
      QMessageBox::warning(0, tr("1H NMR compare error"),
			   tr("Please select only one reactant and one product to compare.") );
      return;
    }
    QStringList rlist, plist;
    Molecule *r1 = reactantList.first();
    Molecule *p1 = productList.first();

    tool1hnmr.setProductMolecule(p1);
    tool1hnmr.setMolecule(r1);
    tool1hnmr.exec();

    /*
    rlist = r1->Calc1HNMR(false);
    plist = p1->Calc1HNMR(false);
    GraphDialog *g = new GraphDialog(r, tr("Predicted 1H-NMR"));

    for ( QStringList::Iterator it = rlist.begin(); 
	  it != rlist.end(); ++it ) {
      ntemp = *it;
      c1 = ntemp.find(',');
      c2 = ntemp.find(',', c1 + 1);
      c3 = ntemp.find(',', c2 + 1);
      shift1 = ntemp.mid(0, c1).toDouble();
      nfull = "";
      n1.setNum(shift1);
      nfull = n1 + " ppm";
      nfull = nfull + tr(" Intensity: ") + ntemp.mid(c1 + 1, c2 - c1 - 1);
      nfull = nfull + tr(" Multiplicity: ") + ntemp.mid(c2 + 1, c3 - c2 - 1);
      mul = ntemp.mid(c1 + 1, c2 - c1 - 1).toInt();
      g->AddPeak(shift1, mul, QColor(255,0,0), QString("peak"), nfull);
    }

    for ( QStringList::Iterator it2 = plist.begin(); 
	  it2 != plist.end(); ++it2 ) {
      ntemp = *it2;
      c1 = ntemp.find(',');
      c2 = ntemp.find(',', c1 + 1);
      c3 = ntemp.find(',', c2 + 1);
      shift1 = ntemp.mid(0, c1).toDouble();
      nfull = "";
      n1.setNum(shift1);
      nfull = n1 + " ppm";
      nfull = nfull + tr(" Intensity: ") + ntemp.mid(c1 + 1, c2 - c1 - 1);
      nfull = nfull + tr(" Multiplicity: ") + ntemp.mid(c2 + 1, c3 - c2 - 1);
      mul = ntemp.mid(c1 + 1, c2 - c1 - 1).toInt();
      g->AddPeak(shift1, mul, QColor(0,0,255), QString("peak"), nfull);
    }
    
    g->show();
    */
    return;
  } // 1H-NMR

  if (flags == RXN_13CNMR) {
    if ( (reactantList.count() > 1) || (productList.count() > 1) ) {
      QMessageBox::warning(0, tr("13C NMR compare error"),
			   tr("Please select only one reactant and one product to compare.") );
      return;
    }
    BremserOneSphereHOSECodePredictor boshcp;
    QStringList rlist, plist;
    Molecule *r1 = reactantList.first();
    Molecule *p1 = productList.first();
    rlist = r1->Calc13CNMR(false);
    plist = p1->Calc13CNMR(false);

    tool13cnmr.setProductMolecule(p1);
    tool13cnmr.setMolecule(r1);
    tool13cnmr.exec();

    /*
    GraphDialog *g = new GraphDialog(r, "Predicted 13C-NMR");
    for ( QStringList::Iterator it = rlist.begin(); 
	  it != rlist.end(); ++it ) {
      g->AddPeak(boshcp.predict(*it), QColor(255,0,0), boshcp.getMult(*it), 
		boshcp.predictFull(*it));
    }
    for ( QStringList::Iterator it2 = plist.begin(); 
	  it2 != plist.end(); ++it2 ) {
      g->AddPeak(boshcp.predict(*it2) + 0.1, QColor(0,0,255), 
		boshcp.getMult(*it2), boshcp.predictFull(*it2));
    }
    
    g->show();
    */
  } // RXN_13CNMR
}
