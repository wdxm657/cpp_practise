#include "window.h"

MainWindow::MainWindow()
    : driver()
{
  const std::vector<std::string> titles = {"test num", "Start size", "End size", "Packet Step", "other1", "other2"};
  const std::vector<std::string> default_values = {"2025", "2048", "2048", "0", "0", "0"};
  // 创建网格容器
  add(grid_);

  // 设置列和行之间的间距
  grid_.set_column_spacing(10);
  grid_.set_row_spacing(10);

  grid_.attach(image_, 2, 0, 1, 3);

  // 创建按钮
  button_.set_label("Get Values");
  grid_.attach(button_, 1, 0, 1, 1);

  // 创建按钮
  button_1.set_label("Resume");
  grid_.attach(button_1, 1, 1, 1, 1);

  // 将输入框数组添加到按钮回调函数中
  button_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked));
  button_1.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked_1));

  // 创建图像处理器对象
  image_processor = new ImageProcessor();

  // 创建图像接收线程，在该线程中接收摄像头的图像数据
  std::thread receive_thread([&]()
                             {
          while (true)
          {
          // cv::Mat dst = cv::imread("/home/wdxm/code/cpp_practise/test.png");
          cv::Mat dst;
          driver.dma.dma_auto_process( driver.getfd(), dst);
                image_processor->store_frame(dst); 
          } });

  // 创建图像显示线程，在该线程中从缓冲区中获取图像数据，并将其显示在Gtk::Image控件中
  std::thread display_thread([&]()
                             {
      bool start_processing = false; // 标志变量，用于指示是否需要启动图像处理线程
      while (true) {
          cv::Mat frame = image_processor->get_frame();
          cv::resize(frame, frame, cv::Size(1700, 1000));
          auto size = frame.size();
          auto pixbuf = Gdk::Pixbuf::create_from_data(frame.data, Gdk::COLORSPACE_RGB, frame.channels() == 4, 8, size.width, size.height, (int)frame.step);
          // 如果标志变量为true，则启动图像处理线程
          if (start_processing) {
              std::cout << "start_processing!!!" << std::endl;
              image_.set(pixbuf);
              // 将标志变量重置为false
              start_processing = false;
          }

          // 检查是否需要启动图像处理线程
          if (start_flag) {
              start_processing = true;
              // start_flag = false;
          }
      } });

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
}

void MainWindow::on_button_clicked()
{
  start_flag = !start_flag;
  std::cout << "button clicked!!! start_flag " << start_flag << std::endl;
}

void MainWindow::on_button_clicked_1()
{
  driver.dma.resume();
}