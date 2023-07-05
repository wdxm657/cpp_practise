#include "dma.h"
typedef std::chrono::high_resolution_clock Clock;
#define NCLK_DEBUG

DMA::DMA()
    : pcie_fd(0),
      pix_row(0),
      img_finish(false),
      img(V_NUM, H_NUM, CV_8UC3)
{
    dma_operator = new dma_oper;
    memset(dma_operator, 0, sizeof(dma_oper));
    dma_operator->current_len = DW_NUM; // DW
    // memset(dma_operator->write_buf, 0, DMA_MAX_PACKET_SIZE);
}

DMA::~DMA()
{
    delete dma_operator;
}

void print_clk(std::string str, std::chrono::_V2::system_clock::time_point t1, std::chrono::_V2::system_clock::time_point t2){
    std::cout << str << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << "ns" << std::endl;
}

void DMA::dma_auto_process(int fd, cv::Mat &dst)
{
        pcie_fd = fd;
        while (!img_finish)
        {
            #ifdef NCLK_DEBUG
                auto t1 = Clock::now();
            #endif
            dma_auto(dst);
            #ifdef NCLK_DEBUG
                auto t2 = Clock::now();
                print_clk("one line dma used: ", t1, t2);
                // cout << "finish " << img_finish << endl;
            #endif
            // sleep(1);
        }
        // cout << "one frame receive down" << endl;
        img_finish = false;
}

void DMA::dma_auto(cv::Mat &dst)
{
    pcie_rw();

    // 1DW/2pix  total 640*2=1280
    int pix_col = 0;
    #ifndef NCLK_DEBUG
        auto t1 = Clock::now();
    #endif
    for (int i = 0; i < DW_NUM; i++)
    {
        for (int j = 0; j < 4; j += 2)
        {
            uint16_t pix = (static_cast<uint16_t>(dma_operator->read_buf[i * 4 + j + 1]) << 8) | static_cast<uint16_t>(dma_operator->read_buf[i * 4 + j]);
            uint8_t r = (pix >> 11) & 0x1f;
            uint8_t g = (pix >> 5) & 0x3f;
            uint8_t b = pix & 0x1f;
            img.at<cv::Vec3b>(pix_row, pix_col) = cv::Vec3b(r << 3, g << 2, b << 3);
            pix_col == (H_NUM - 1) ? pix_col = 0 : pix_col++;
            if (pix_col == 0 && pix_row == (V_NUM - 1))
            {
                dst = img;
                img_finish = true;
                pix_row = 0;
                return;
            }
        }
    }
    #ifndef NCLK_DEBUG
        auto t2 = Clock::now();
        print_clk("data parse used: ", t1, t2);
    #endif
    pix_row++;
}

void DMA::PCI_MAP(bool flag, int fd){
    // if (flag){
    //     ioctl(pcie_fd, PCI_MAP_ADDR_CMD, dma_operator);         /* 地址映射,以及数据缓存申请 */
    // }
    // else 
    //     ioctl(pcie_fd, PCI_UMAP_ADDR_CMD, dma_operator);        /* 释放数据缓存 */
}

void DMA::pcie_rw()
{
    #ifndef NCLK_DEBUG
        auto t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_MAP_ADDR_CMD, dma_operator);         /* 地址映射,以及数据缓存申请 */
    #ifndef NCLK_DEBUG
        auto t2 = Clock::now();
        print_clk("PCI_MAP_ADDR_CMD used: ", t1, t2);
    #endif

    #ifndef NCLK_DEBUG
        t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_DMA_WRITE_CMD, dma_operator);        /* 将数据从设备读出到内核（DMA写） */
    #ifndef NCLK_DEBUG
        t2= Clock::now();
        print_clk("PCI_DMA_WRITE_CMD used: ", t1, t2);
    #endif

    #ifndef NCLK_DEBUG
        t1 = Clock::now();
    #endif
    int i = 0;
    while (i < 100000)
    {
        i++;
    }
    #ifndef NCLK_DEBUG
        t2= Clock::now();
        print_clk("count used: ", t1, t2);
    #endif

    #ifndef NCLK_DEBUG
        t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_READ_FROM_KERNEL_CMD, dma_operator); /* 将数据从内核读出 */
    #ifndef NCLK_DEBUG
        t2= Clock::now();
        print_clk("PCI_READ_FROM_KERNEL_CMD used: ", t1, t2);
    #endif

    #ifndef NCLK_DEBUG
        t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_UMAP_ADDR_CMD, dma_operator);        /* 释放数据缓存 */
    #ifndef NCLK_DEBUG
        t2 = Clock::now();
        print_clk("PCI_UMAP_ADDR_CMD used: ", t1, t2);
    #endif
    if (dma_operator->read_buf[4] == 0 && dma_operator->read_buf[5] == 0 \
       && dma_operator->read_buf[6] == 0 && dma_operator->read_buf[7] == 0)
    {
        cout << "line error" << endl;
    }
}

void DMA::pcie_data_printf(){
    for (int i = 0; i < dma_operator->current_len; i++)
    {
        printf("dw_cnt = %d; write_data = 0x%02x%02x%02x%02x; read_data = 0x%02x%02x%02x%02x;\n", i + 1,
               dma_operator->write_buf[i * 4], dma_operator->write_buf[i * 4 + 1], dma_operator->write_buf[i * 4 + 2], dma_operator->write_buf[i * 4 + 3],
               dma_operator->read_buf[i * 4], dma_operator->read_buf[i * 4 + 1], dma_operator->read_buf[i * 4 + 2], dma_operator->read_buf[i * 4 + 3]);
    }
}

void DMA::resume()
{
    cv::Mat dst;
    dma_auto(dst);
}

void DMA::dma_wr()
{
    // if (pix_cnt == 0)
    // {
    //     simulation_fram(true);
    // }
    memset(dma_operator->write_buf, 0x11, DMA_MAX_PACKET_SIZE);
    // pix_cnt = pix_cnt == (TOTAL_SEND_TIME - 1) ? 0 : pix_cnt + 1;
}