#include <getopt.h>
#include "window.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

    // 创建主窗口
    MainWindow window;
    window.set_title("Input Example");
    window.set_default_size(1580, 960);

    // 显示窗口及其所有子控件
    return app->run(window);
}