#include "window.h"

MainWindow::MainWindow()
    : driver(), inf()
{
  const std::vector<std::string> titles = {"test num", "Start size", "End size", "Packet Step", "other1", "other2"};
  const std::vector<std::string> default_values = {"0", "2048", "2048", "0", "0", "0"};
  // 创建网格容器
  add(grid_);

  // 设置列和行之间的间距
  grid_.set_column_spacing(10);
  grid_.set_row_spacing(10);

  // 创建6个带标题的输入框
  for (int i = 0; i < 6; i++)
  {
    Gtk::Label *label = new Gtk::Label(titles[i]);
    grid_.attach(*label, 0, i, 1, 1);
    labels_.push_back(label);

    Gtk::Entry *entry = new Gtk::Entry();
    entry->set_text(default_values[i]);
    grid_.attach(*entry, 1, i, 1, 1);
    entries_.push_back(entry);
  }

  // 创建滚动窗口和文本视图
  scrolled_window_.set_policy(Gtk::PolicyType::POLICY_AUTOMATIC, Gtk::PolicyType::POLICY_AUTOMATIC);
  grid_.attach(scrolled_window_, 2, 1, 1, 5);

  text_view_.set_editable(false);
  text_view_.set_cursor_visible(false);
  scrolled_window_.add(text_view_);

  // 创建按钮
  button_.set_label("Get Values");
  grid_.attach(button_, 2, 0, 1, 1);

  // 将输入框数组添加到按钮回调函数中
  button_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked));

  // inf.base_exam();
  // 显示所有子控件
  show_all_children();
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
  // 从输入框中获取值
  std::vector<std::string> values;
  for (auto entry : entries_)
  {
    values.push_back(entry->get_text());
  }

  // 将值输出到文本视图中
  Glib::ustring output;
  for (auto value : values)
  {
    output += value + "\n";
  }

  text_view_.get_buffer()->set_text(output);

  // 将值输出到控制台中
  for (auto value : values)
  {
    std::cout << value << std::endl;
  }
}