#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm.h>
#include <vector>
#include <iostream>
#include "driver.h"
#include "inference.h"

class MainWindow : public Gtk::Window
{
public:
  MainWindow();
  ~MainWindow();

protected:
  void on_button_clicked();

  std::vector<std::string> titles_;
  std::vector<std::string> default_values_;

  Gtk::Grid grid_;
  std::vector<Gtk::Label *> labels_;
  std::vector<Gtk::Entry *> entries_;
  Gtk::Button button_;

  Gtk::ScrolledWindow scrolled_window_;
  Gtk::TextView text_view_;

  Driver driver;
  Inference inf;
};

#endif // MAIN_WINDOW_H