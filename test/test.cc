#include <opencv2/opencv.hpp>
#include <fstream>

using namespace cv;

int main(int argc, char **argv)
{
    // 加载图像
    Mat img = imread("/home/wdxm/test/test0.bmp");

    // 获取图像的宽度和高度
    int width = img.cols;
    std::cout << width << std::endl;
    int height = img.rows;
    std::cout << height << std::endl;

    // 打开输出文件
    std::ofstream outfile("/home/wdxm/test/test0.txt");

    // 遍历图像的每个像素并将其写入文件
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Vec3b pixel = img.at<Vec3b>(y, x);
            outfile << (int)pixel[0] << " " << (int)pixel[1] << " " << (int)pixel[2] << std::endl;
        }
    }

    // 关闭输出文件
    outfile.close();

    return 0;
}