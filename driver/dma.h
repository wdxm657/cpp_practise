#ifndef DMA_H
#define DMA_H

#include "config_gui.h"
#include <vector>
#include "iostream"

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

    void set_auto(std::vector<unsigned int> values, int fd);

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

    /**************************************************************************
    ** 函数名称:    dma_auto_process
    ** 函数功能:dma auto测试界面执行函数
    ** 输入参数:*dma_auto：dma auto测试界面参数信息结构体
    **			*dma_oper:配置参数以及数据缓存
    ** 输出参数:    无
    ** 返回参数:    无
    ****************************************************************************/
    void dma_auto_process();
    void dma_oper_fun();
};

#endif // DMA_H