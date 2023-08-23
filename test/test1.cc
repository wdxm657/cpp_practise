#include <vector>
#include <opencv2/opencv.hpp>

int main()
{
    // 创建包含 1920*1080 个 uint16_t 类型元素的向量对象
    std::vector<uint16_t> vec(1920 * 1080);

    // 将向量对象填充为随机值
    for (int i = 0; i < vec.size(); i++)
    {
        vec[i] = static_cast<uint16_t>(0x1f << 8) | static_cast<uint16_t>(0x00);
    }

    // 创建空的 cv::Mat 对象
    cv::Mat img(1080, 1920, CV_8UC3);

    // 复制向量数据到图像数据
    for (int i = 0; i < vec.size(); i++)
    {
        uint16_t pixel = vec[i];
        uint8_t r = (0xfff0 >> 11) & 0x1f;
        uint8_t g = (0xfff0 >> 5) & 0x3f;
        uint8_t b = 0xfff0 & 0x1f;
        // printf("r%2x\r\n",r);
        // printf("g%2x\r\n",g);
        // printf("b%2x\r\n",b);
        uint32_t t1 = r << 3;
        uint32_t t2 = g << 2;
        uint32_t t3 = b << 3;
        // printf("t1   %2x\r\n",t1);
        // printf("t2   %2x\r\n",t2);
        // printf("t3   %2x\r\n",t3);
        t1 = t1 << 16;
        t2 = t2 << 8;
        t3 = t3 << 0;
        uint32_t t4 = 0x00FF0000;
        // printf("t1   %2x\r\n",t1);
        // printf("t2   %2x\r\n",t2);
        // printf("t3   %2x\r\n",t3);
        // printf("t4   %2x\r\n",t4);
        // img.at<cv::Vec3b>(i / 1920, i % 1920) = cv::Vec3b(r << 3, g << 2, b << 3);
        // img.at<cv::uint32_t>(i / 1920, i % 1920) = t4;
        img.data[(i / 1920 * img.step) +  (i % 1920 * 3) + 0] = r << 3;
        img.data[(i / 1920 * img.step) +  (i % 1920 * 3) + 1] = g << 2;
        img.data[(i / 1920 * img.step) +  (i % 1920 * 3) + 2] = b << 3;
    }

    // 在窗口中显示图像
    cv::imshow("Image", img);
    cv::waitKey(0);

    return 0;
}