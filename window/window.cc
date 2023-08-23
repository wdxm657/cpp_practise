#include "window.h"

typedef std::chrono::high_resolution_clock Clock;
MainWindow::MainWindow()
    : driver(),
      inf()
{
  // 创建网格容器
  add(grid_);

  // 设置列和行之间的间距
  grid_.set_column_spacing(10);
  grid_.set_row_spacing(10);

  grid_.attach(image_, 4, 0, 50, 50);

  // 创建按钮
  button_.set_label("INFERENCE ON");
  grid_.attach(button_, 1, 0, 1, 1);

  // 创建按钮
  button_1.set_label("PCIE DMA ON");
  button_1.set_sensitive(false);
  grid_.attach(button_1, 1, 1, 1, 1);

  // 创建按钮
  button_2.set_label("PCIE MAP ON");
  grid_.attach(button_2, 1, 2, 1, 1);

  // 创建按钮
  button_3.set_label("DESTORY");
  grid_.attach(button_3, 1, 3, 1, 1);

  // const std::vector<std::string> titles = {"rec fps", "det fps"};
  // const std::vector<std::string> default_values = {"0", "0"};
  // // 创建6个带标题的输入框
  // for (int i = 0; i < 2; i++)
  // {
  //   int a = 0;
  //   int b = 0;
  //   if (i == 0)
  //   {
  //     a = 0;
  //     b = 1;
  //   }else {
  //     a = 2;
  //     b = 3;
  //   }
  //   Gtk::Label *label = new Gtk::Label(titles[i]);
  //   grid_.attach(*label, 0, a, 1, 1);
  //   labels_.push_back(label);

  //   Gtk::Entry *entry = new Gtk::Entry();
  //   entry->set_text(default_values[i]);
  //   grid_.attach(*entry, 0, b, 1, 1);
  //   entries_.push_back(entry);
  // }
  // 将输入框数组添加到按钮回调函数中
  button_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked));
  button_1.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked_1));
  button_2.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked_2));
  button_3.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked_3));

  // 创建图像处理器对象
  image_processor = new ImageProcessor();

  // 创建图像接收线程，在该线程中接收摄像头的图像数据
  std::thread receive_thread([&](){
        bool start_pro = true; // 标志变量，用于指示是否需要启动图像处理线程
        cv::Mat dst = cv::imread("/home/wdxm/code/cpp_practise/source/data/bus.jpg");
        driver.dma.setfd(driver.getfd());
        while (true)
        {
          // auto t1 = Clock::now();
          // cv::Mat dst = cv::imread("/home/wdxm/code/cpp_practise/source/data/bus.jpg");
          // dst = inf.base_exam(dst);
          // auto t2 = Clock::now();
          // if (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() > 0)
          // {
          //   cv::imshow("Inference", dst);
          //   int fps = 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
          //   printf("detect fps : %d\n", fps);
          // }
          std::unique_lock<std::mutex> lock(dma_mutex);
          if (dma_flag)
          {
            lock.unlock();
            auto t1 = Clock::now();
            driver.dma.dma_auto_process(dst);
            auto t2 = Clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() > 5)
            {
              int fps = 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
              printf("rec fps : %d\n", fps);
              // entries_[0]->set_text(std::to_string(fps));
            }
            if (dst.cols > 0 && dst.rows > 0 && start_pro)
            {
              // cv::imshow("dst", dst);
              // Glib::RefPtr<Gdk::Pixbuf> pixbuf =
              //   Gdk::Pixbuf::create_from_data((guint8*)dst.data,
              //                                 Gdk::COLORSPACE_RGB,
              //                                 false,
              //                                 8,
              //                                 dst.cols,
              //                                 dst.rows,
              //                                 (int) dst.step);
              // if (start_pro)
              // {
              //   image_.set(pixbuf);
              // }
            }
            
            image_processor->store_frame(dst);
            //下面这行打开的话按钮点一次执行一次，不打开则点击开启，再次点击关闭
            // dma_flag = false;
              // 检查是否需要启动图像处理线程
              std::unique_lock<std::mutex> lock(futex);
              if (start_flag) {
                  start_pro = false;
              }else {
                start_pro = true;
              }
              lock.unlock();
          }
        }
      }
    );

  // 创建图像显示线程，在该线程中从缓冲区中获取图像数据，并将其显示在Gtk::Image控件中
  std::thread display_thread([&](){
      bool start_processing = false; // 标志变量，用于指示是否需要启动图像处理线程
      while (true) {
        // 如果标志变量为true，则启动图像处理线程
        if (start_processing) {
            // cv::Mat test = cv::imread("/home/wdxm/code/cpp_practise/source/data/bus.jpg");
            cv::Mat frame = image_processor->get_frame();
            auto t1 = Clock::now();
            frame = inf.base_exam(frame);
            auto t2 = Clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() > 5)
            {
              int fps = 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
              printf("det fps : %d\n", fps);
            //   entries_[1]->set_text(std::to_string(fps));
            }
            if (frame.cols > 0 && frame.rows > 0)
            {
              Glib::RefPtr<Gdk::Pixbuf> pixbuf =
                Gdk::Pixbuf::create_from_data((guint8*)frame.data,
                                              Gdk::COLORSPACE_RGB,
                                              false,
                                              8,
                                              frame.cols,
                                              frame.rows,
                                              (int) frame.step);
              image_.set(pixbuf);
            }
        }
          // 检查是否需要启动图像处理线程
        std::unique_lock<std::mutex> lock(futex);
        if (start_flag) {
            start_processing = true;
        }else {
          start_processing = false;
        }
        lock.unlock();
      } 
    }
  );

  // 显示所有子控件
  show_all_children();
  // 在新线程中执行图像接收和图像显示操作时，主线程可以继续响应其他控件的事件
  receive_thread.detach();
  display_thread.detach();
  // inf.base_exam();
}

MainWindow::~MainWindow()
{
  for (auto label : labels_)
  {
    delete label;
  }

  for (auto entry : entries_)
  {
    delete entry;
  }
  if (map_flag)
  {
    driver.dma.PCI_MAP(false);
  }
}

void MainWindow::on_button_clicked()
{
  start_flag = !start_flag;
  if (start_flag)
  {
    button_.set_label("INFERENCE OFF");
    std::cout << "INFERENCE ON" << std::endl;
  }
  else
  {
    button_.set_label("INFERENCE ON");
    std::cout << "INFERENCE OFF" << std::endl;
  }
}

void MainWindow::on_button_clicked_1()
{
  std::unique_lock<std::mutex> lock(dma_mutex);
  dma_flag = !dma_flag;
  if (dma_flag)
  {
    lock.unlock();
    button_2.set_sensitive(false);
    button_1.set_label("PCIE DMA OFF");
    std::cout << "PCIE DMA ON" << std::endl;
  }
  else
  {
    lock.unlock();
    button_2.set_sensitive(true);
    button_1.set_label("PCIE DMA ON");
    std::cout << "PCIE DMA OFF" << std::endl;
  }
}

void MainWindow::on_button_clicked_2()
{
  map_flag = !map_flag;
  driver.dma.PCI_MAP(map_flag);
  if (map_flag)
  {
    button_1.set_sensitive(true);
    button_2.set_label("PCIE MAP OFF");
    std::cout << "PCIE MAP ON" << std::endl;
  }
  else
  {
    button_1.set_sensitive(false);
    button_2.set_label("PCIE MAP ON");
    std::cout << "PCIE MAP OFF" << std::endl;
  }
}

void MainWindow::on_button_clicked_3()
{
  cv::destroyAllWindows();
}
