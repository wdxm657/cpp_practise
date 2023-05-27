#ifndef DMA_H
#define DMA_H

#include "config_gui.h"
#include <vector>
#include "iostream"
#include "inference.h"

// 计算数据总长度  1个数据:32位 2个像素     1次512个数据  总共2025次 = 1920*1080
#define TOTAL_FRAME_PIX 2 * 512 * 2025

struct dma_oper
{
    int current_len;
    int offset_addr;
    int cmd;
    unsigned char read_buf[DMA_MAX_PACKET_SIZE];
    unsigned char write_buf[DMA_MAX_PACKET_SIZE];
};

using namespace std;

class DMA
{
public:
    DMA();
    ~DMA();

    void dma_auto_process(std::vector<unsigned int> values, int fd, cv::Mat &dst);
    void resume();

private:
    // dma operator
    dma_oper *dma_operator;

    // dma auto
    unsigned int test_num;
    unsigned int start;
    unsigned int end;
    unsigned int step;
    unsigned int write_cnt;
    unsigned int read_cnt;

    // driver
    int pcie_fd;

    // dnn inf
    Inference inf;

    // buffer
    int pix_col;
    int pix_row;

    int pix_cnt;

    cv::Mat img;
    std::vector<uint8_t> wr_vec;
    int pt;
    bool process_finish;

    /**************************************************************************
    ** 函数名称:    dma_auto_process
    ** 函数功能:dma auto测试界面执行函数
    ** 输入参数:*dma_auto：dma auto测试界面参数信息结构体
    **			*dma_oper:配置参数以及数据缓存
    ** 输出参数:    无
    ** 返回参数:    无
    ****************************************************************************/
    void dma_auto(cv::Mat &dst);
    void simulation_fram(bool begin);
    void dma_wr();
    void dma_rd(cv::Mat &dst);
};

#endif // DMA_H