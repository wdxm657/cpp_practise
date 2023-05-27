#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <gtkmm.h>
#include <opencv2/opencv.hpp>

class ImageProcessor
{
public:
    ImageProcessor() {}

    // 该方法由接收线程调用，用于将收集到的一帧图像数据存储在缓冲区中
    void store_frame(cv::Mat frame)
    {
        std::unique_lock<std::mutex> lock(frame_mutex);
        frame_buffer = frame.clone();
        frame_ready = true;
        lock.unlock();
        frame_cv.notify_one();
    }

    // 该方法由主线程调用，用于将处理后的图像数据显示出来
    void display_image(Glib::RefPtr<Gdk::Pixbuf> pixbuf, Gtk::Image &image)
    {
        // 将Gdk::Pixbuf对象设置为图像控件的值
        image.set(pixbuf);
    }

    // 该方法在图像显示线程中调用，用于从缓冲区中获取图像数据
    cv::Mat get_frame()
    {
        std::unique_lock<std::mutex> lock(frame_mutex);
        while (!frame_ready)
        {
            frame_cv.wait(lock);
        }
        frame_ready = false;
        return frame_buffer;
    }

private:
    std::mutex frame_mutex;
    std::condition_variable frame_cv;
    cv::Mat frame_buffer;
    bool frame_ready = false;
};