#include <vector>
#include <opencv2/opencv.hpp>

int main()
{
    // 创建包含 1920*1080 个 uint16_t 类型元素的向量对象
    std::vector<uint16_t> vec(1920 * 1080);

    // 将向量对象填充为随机值
    for (int i = 0; i < vec.size(); i++)
    {
        vec[i] = 0xef7d;
    }

    // 创建空的 cv::Mat 对象
    cv::Mat img(1080, 1920, CV_8UC3);

    // 复制向量数据到图像数据
    for (int i = 0; i < vec.size(); i++)
    {
        uint16_t pixel = vec[i];
        uint8_t r = (pixel >> 11) & 0x1f;
        uint8_t g = (pixel >> 5) & 0x3f;
        uint8_t b = pixel & 0x1f;
        img.at<cv::Vec3b>(i / 1920, i % 1920) = cv::Vec3b(b << 3, g << 2, r << 3);
    }

    // 在窗口中显示图像
    cv::imshow("Image", img);
    cv::waitKey(0);

    return 0;
}