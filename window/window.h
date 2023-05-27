#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm.h>
#include <vector>
#include <iostream>
#include "driver.h"
#include <thread>
#include "ImageProcessor.h"
#include <mutex>
#include <condition_variable>

class MainWindow : public Gtk::Window
{
public:
  MainWindow();
  ~MainWindow();

protected:
  void on_button_clicked();
  void on_button_clicked_1();

  std::vector<std::string> titles_;
  std::vector<std::string> default_values_;

  Gtk::Grid grid_;
  std::vector<Gtk::Label *> labels_;
  std::vector<Gtk::Entry *> entries_;
  Gtk::Button button_;
  Gtk::Button button_1;

  Gtk::ScrolledWindow scrolled_window_;
  Gtk::TextView text_view_;
  Gtk::Image image_;

  Driver driver;
  ImageProcessor *image_processor;
  bool start_flag = false; // 标志变量，用于指示是否需要启动图像处理线程
};

#endif // MAIN_WINDOW_H