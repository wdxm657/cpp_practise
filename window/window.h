#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm.h>
#include <vector>
#include <iostream>
#include "driver.h"
#include <thread>
#include <string>
#include <mutex>
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
  void on_button_clicked_2();
  void on_button_clicked_3();

  std::vector<std::string> titles_;
  std::vector<std::string> default_values_;

  Gtk::Grid grid_;
  std::vector<Gtk::Label *> labels_;
  std::vector<Gtk::Entry *> entries_;
  Gtk::Button button_;
  Gtk::Button button_1;
  Gtk::Button button_2;
  Gtk::Button button_3;

  Gtk::ScrolledWindow scrolled_window_;
  Gtk::TextView text_view_;
  Gtk::Image image_;

  Driver driver;
  // dnn inf
  Inference inf;
  ImageProcessor *image_processor;
  std::mutex futex;
  std::mutex dma_mutex;
  bool start_flag = false; // 标志变量，用于指示是否需要启动图像处理线程
  bool dma_flag   = false; // 标志变量，用于指示是否需要启动PCIE接受HDMI数据
  bool map_flag   = false; // 标志变量，用于指示是否需要打开或关闭PCIE DMA MAP
};

#endif // MAIN_WINDOW_H