
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __gnu_javax_print_PrinterDialog$AppearancePanel__
#define __gnu_javax_print_PrinterDialog$AppearancePanel__

#pragma interface

#include <javax/swing/JPanel.h>
extern "Java"
{
  namespace gnu
  {
    namespace javax
    {
      namespace print
      {
          class PrinterDialog;
          class PrinterDialog$AppearancePanel;
          class PrinterDialog$AppearancePanel$Color;
          class PrinterDialog$AppearancePanel$JobAttributes;
          class PrinterDialog$AppearancePanel$Quality;
          class PrinterDialog$AppearancePanel$SidesPanel;
      }
    }
  }
}

class gnu::javax::print::PrinterDialog$AppearancePanel : public ::javax::swing::JPanel
{

public:
  PrinterDialog$AppearancePanel(::gnu::javax::print::PrinterDialog *);
public: // actually package-private
  void update();
  static ::gnu::javax::print::PrinterDialog * access$0(::gnu::javax::print::PrinterDialog$AppearancePanel *);
private:
  ::gnu::javax::print::PrinterDialog$AppearancePanel$Quality * __attribute__((aligned(__alignof__( ::javax::swing::JPanel)))) quality_panel;
  ::gnu::javax::print::PrinterDialog$AppearancePanel$JobAttributes * jobAttr_panel;
  ::gnu::javax::print::PrinterDialog$AppearancePanel$SidesPanel * sides_panel;
  ::gnu::javax::print::PrinterDialog$AppearancePanel$Color * chromaticy_panel;
public: // actually package-private
  ::gnu::javax::print::PrinterDialog * this$0;
public:
  static ::java::lang::Class class$;
};

#endif // __gnu_javax_print_PrinterDialog$AppearancePanel__
