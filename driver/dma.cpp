#include "dma.h"
typedef std::chrono::high_resolution_clock Clock;
#define CLK_DEBUG1

DMA::DMA()
    : pcie_fd(0),
      pix_row(0),
      pix_col(0),
      img_finish(false),
      img(V_NUM, H_NUM, CV_8UC3)
{
    dma_operator = new dma_oper;
    memset(dma_operator, 0, sizeof(dma_oper));
    dma_operator->current_len = DW_NUM; // DW
    dma_operator->offset_addr = 0;
}

DMA::~DMA()
{
    delete dma_operator;
}

void print_clk(std::string str, std::chrono::_V2::system_clock::time_point t1, std::chrono::_V2::system_clock::time_point t2){
    std::cout << str << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << "ns" << std::endl;
}
void DMA::dma_check(){
    while (1)
    {
        dma_operator->current_len = 4;
        dma_operator->offset_addr = 0;
        pcie_rd();
        if (dma_operator->read_buf[0] == 0x00
        &   dma_operator->read_buf[1] == 0x00
        &   dma_operator->read_buf[2] == 0x00
        &   dma_operator->read_buf[3] == 0x1f)
        {
        // printf("dw_cnt = %d; read_data = 0x%02x%02x%02x%02x;\n",1,
        //     dma_operator->read_buf[4], dma_operator->read_buf[1], dma_operator->read_buf[2], dma_operator->read_buf[3]);
            return;
        }
    }
}

void DMA::setfd(int fd)
{
    pcie_fd = fd;
}

void DMA::dma_auto_process(cv::Mat &dst)
{
        img_finish = false;
        auto t1 = Clock::now();
        while (!img_finish)
        {
            // dma_check();
            dma_auto(dst);
            // pcie_data_printf();
            auto t2 = Clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() > 1000)
            {
                break;
            }
            // print_clk("one line dma used: ", t1, t2);
            // sleep(1);
        }
        // cout << "one frame receive down" << endl;
}

bool check_continuous_value(uint8_t *buf, int start, int end, uint8_t value)
{
    int left = start, right = start;
    while(right <= end)
    {
        if(buf[right] == value)
        {
            right++;
            if(right - left == end - start + 1)
            {
                return true; // 连续的值都是特定的值
            }
        }
        else
        {
            left++;
            right = left;
        }
    }
    return false; // 不是连续的特定值
}

void DMA::dma_auto(cv::Mat &dst)
{
    int pix_num = 0;
    pcie_rd();
    #ifdef CLK_DEBUG
        auto t1 = Clock::now();
    #endif
    for (int i = 3; i < DW_NUM; i+=4)
    {
        uint16_t current_row = 0;
        uint16_t current_col = 0;
        uint32_t total_cnt = 0;
        for (int k = (i - 3); k < i + 1; k++)
        {
            // 128 bit = 4'bframe? + 4'b0 + 24'b total_cnt + 16'b pix * 6
            // 32 bit = 4'bframe? + 4'b0 + 24'b total_cnt
            // 96 bit = 16'b pix * 6
            if (k == (i - 3))
            {
                uint8_t frame = dma_operator->read_buf[k * 4 + 0];
                // printf("frame = 0x%02x;\n",frame);
                if (frame == 0xAA) break;
                total_cnt = (dma_operator->read_buf[k * 4 + 1] << 16)   \
                            | (dma_operator->read_buf[k * 4 + 2] << 8)  \
                            | (dma_operator->read_buf[k * 4 + 3] << 0);
                if (total_cnt < 5) total_cnt = 5;
                // if (total_cnt == 5)    
                // {
                //     for (size_t s = 4; s < 16; s++)
                //     {
                //         printf("%02x", dma_operator->read_buf[k * 4 + s]);
                //     }
                //     printf("\n");
                //     int p1 = dma_operator->read_buf[k * 4 + 12] << 8  | dma_operator->read_buf[k * 4 + 13];
                //     int p2 = dma_operator->read_buf[k * 4 + 14] << 8 | dma_operator->read_buf[k * 4 + 15];
                //     int p3 = dma_operator->read_buf[k * 4 + 8] << 8  | dma_operator->read_buf[k * 4 + 9];
                //     int p4 = dma_operator->read_buf[k * 4 + 10] << 8  | dma_operator->read_buf[k * 4 + 11];
                //     int p5 = dma_operator->read_buf[k * 4 + 4] << 8  | dma_operator->read_buf[k * 4 + 5];
                //     int p6 = dma_operator->read_buf[k * 4 + 6] << 8  | dma_operator->read_buf[k * 4 + 7];
                //     printf("total_cnt = %d;\n",total_cnt);
                //     printf("pix1 = %d;\n",p1);
                //     printf("pix2 = %d;\n",p2);
                //     printf("pix3 = %d;\n",p3);
                //     printf("pix4 = %d;\n",p4);
                //     printf("pix5 = %d;\n",p5);
                //     printf("pix6 = %d;\n",p6);
                // }
            }else {
                int s1,s2;
                for (int j = 2; j >= 1; j--)
                {
                    if (j == 2)
                    {
                        s1 = k+(k*3)+j;
                        s2 = k+(k*3)+j+1;
                    }else {
                        s1 = k - 1 + (k*3)+j;
                        s2 = k - 1 + (k*3)+j+1;
                    }
                    // printf("%d %d\n",k,j);
                    // printf("%d %d\n",s1, s2);
                    // k = 1   6  7  4  5    +5  +6 +3  +4  
                    // k = 2   10 11 8  9    +8  +9 +6  +7  
                    // k = 3   14 15 12 13   +11 +12 +9 +10 
                    // printf("total_cnt = %d\n", total_cnt);
                    try
                    {
                        pix_row = total_cnt / H_NUM;
                        pix_col = total_cnt % H_NUM;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << total_cnt << '\n';
                    }
                    // if (total_cnt== 5)
                    // {
                    //     printf("%d", total_cnt);
                    // }
                    
                    // printf("current_row = %d\n", pix_row);
                    // printf("current_col = %d\n", pix_col);
                    total_cnt--;
                    // printf("pix = 0x%02x%02x;\n",dma_operator->read_buf[k * 4 + j], dma_operator->read_buf[k * 4 + j + 1]);
                    uint16_t pix   = (static_cast<uint16_t>(dma_operator->read_buf[s1]) << 8) | static_cast<uint16_t>(dma_operator->read_buf[s2]);
                    uint8_t  r     = (pix >> 11) & 0x1f;
                    uint8_t  g     = (pix >> 5) & 0x3f;
                    uint8_t  b     = pix & 0x1f;
                    uint32_t r_pos = (pix_row * img.step) + (pix_col * 3) + 0;
                    uint32_t g_pos = (pix_row * img.step) + (pix_col * 3) + 1;
                    uint32_t b_pos = (pix_row * img.step) + (pix_col * 3) + 2;
                    img.data[r_pos] = r << 3;
                    img.data[g_pos] = g << 2;
                    img.data[b_pos] = b << 3;
                    // img.at<cv::Vec3b>(pix_row, pix_col) = cv::Vec3b(r << 3, g << 2, b << 3);
                    if (pix_col == (H_NUM - 1) & pix_row == (V_NUM - 1))
                    {
                        // printf("one frame down\n");
                        dst = img;
                        img_finish = true;
                        return; // finish
                    }
                }
            }
        }
    }
    #ifdef CLK_DEBUG
        auto t2 = Clock::now();
        print_clk("data parse used: ", t1, t2);
    #endif
}

void DMA::PCI_MAP(bool flag){
    if (flag){
        ioctl(pcie_fd, PCI_MAP_ADDR_CMD, dma_operator);         /* 地址映射,以及数据缓存申请 */
    }
    else 
        ioctl(pcie_fd, PCI_UMAP_ADDR_CMD, dma_operator);        /* 释放数据缓存 */
}

void DMA::pcie_data_printf(){
    for (int i = 0; i < dma_operator->current_len; i++)
    {
        printf("dw_cnt = %d; write_data = 0x%02x%02x%02x%02x; read_data = 0x%02x%02x%02x%02x;\n", i + 1,
               dma_operator->write_buf[i * 4], dma_operator->write_buf[i * 4 + 1], dma_operator->write_buf[i * 4 + 2], dma_operator->write_buf[i * 4 + 3],
               dma_operator->read_buf[i * 4], dma_operator->read_buf[i * 4 + 1], dma_operator->read_buf[i * 4 + 2], dma_operator->read_buf[i * 4 + 3]);
    }
}

void DMA::resume(int fd)
{
    // cout<<inital_flag<<endl;
    // if (inital_flag)
    //     simulation_fram(0xA);
    // else 
        simulation_fram(0xB);
}

void DMA::simulation_fram(uint32_t v)
{
    // 模拟一帧开始的信号
    dma_operator->current_len = 4;
    dma_operator->offset_addr = 0;
    dma_operator->write_buf[0] = 0xff;
    pcie_wr();
    dma_operator->current_len = DW_NUM;
    dma_operator->offset_addr = 0;
}

void DMA::pcie_rd()
{
    #ifdef CLK_DEBUG
       auto t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_DMA_WRITE_CMD, dma_operator);        /* 将数据从设备读出到内核（DMA写） */
    #ifdef CLK_DEBUG
        auto t2= Clock::now();
        print_clk("PCI_DMA_WRITE_CMD used: ", t1, t2);
    #endif

    #ifdef CLK_DEBUG
        t1 = Clock::now();
    #endif
    int i = 0;
    while (i < 65000)
    {
        i++;
    }
    #ifdef CLK_DEBUG
        t2= Clock::now();
        print_clk("count used: ", t1, t2);
    #endif

    #ifdef CLK_DEBUG
        t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_READ_FROM_KERNEL_CMD, dma_operator); /* 将数据从内核读出 */
    #ifdef CLK_DEBUG
        auto t2= Clock::now();
        print_clk("PCI_READ_FROM_KERNEL_CMD used: ", t1, t2);
    #endif
    // pcie_data_printf();
}

void DMA::pcie_wr()
{
    #ifdef CLK_DEBUG
       auto t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_WRITE_TO_KERNEL_CMD, dma_operator); /* 将数据从内核读出 */
    #ifdef CLK_DEBUG
       auto t2= Clock::now();
        print_clk("PCI_READ_FROM_KERNEL_CMD used: ", t1, t2);
    #endif

    #ifdef CLK_DEBUG
        t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_DMA_READ_CMD, dma_operator);        /* 将数据从设备读出到内核（DMA写） */
    #ifdef CLK_DEBUG
        t2= Clock::now();
        print_clk("PCI_DMA_READ_CMD used: ", t1, t2);
    #endif

    #ifdef CLK_DEBUG
        t1 = Clock::now();
    #endif
    int i = 0;
    while (i < 50000)
    {
        i++;
    }
    #ifdef CLK_DEBUG
        t2= Clock::now();
        print_clk("count used: ", t1, t2);
    #endif
}

void DMA::pcie_rw()
{
    // #ifdef CLK_DEBUG
    //     auto t1 = Clock::now();
    // #endif
    // ioctl(pcie_fd, PCI_MAP_ADDR_CMD, dma_operator);         /* 地址映射,以及数据缓存申请 */
    // #ifdef CLK_DEBUG
    //     auto t2 = Clock::now();
    //     print_clk("PCI_MAP_ADDR_CMD used: ", t1, t2);
    // #endif

    #ifdef CLK_DEBUG
       auto t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_WRITE_TO_KERNEL_CMD, dma_operator); /* 将数据从内核读出 */
    #ifdef CLK_DEBUG
       auto t2= Clock::now();
        print_clk("PCI_READ_FROM_KERNEL_CMD used: ", t1, t2);
    #endif

    #ifdef CLK_DEBUG
        t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_DMA_READ_CMD, dma_operator);        /* 将数据从设备读出到内核（DMA写） */
    #ifdef CLK_DEBUG
        t2= Clock::now();
        print_clk("PCI_DMA_READ_CMD used: ", t1, t2);
    #endif

    #ifdef CLK_DEBUG
        t1 = Clock::now();
    #endif
    int i = 0;
    while (i < 100000)
    {
        i++;
    }
    #ifdef CLK_DEBUG
        t2= Clock::now();
        print_clk("count used: ", t1, t2);
    #endif

    #ifdef CLK_DEBUG
        t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_DMA_WRITE_CMD, dma_operator);        /* 将数据从设备读出到内核（DMA写） */
    #ifdef CLK_DEBUG
        t2= Clock::now();
        print_clk("PCI_DMA_WRITE_CMD used: ", t1, t2);
    #endif

    #ifdef CLK_DEBUG
        t1 = Clock::now();
    #endif
    i = 0;
    while (i < 100000)
    {
        i++;
    }
    #ifdef CLK_DEBUG
        t2= Clock::now();
        print_clk("count used: ", t1, t2);
    #endif

    #ifdef CLK_DEBUG
        t1 = Clock::now();
    #endif
    ioctl(pcie_fd, PCI_READ_FROM_KERNEL_CMD, dma_operator); /* 将数据从内核读出 */
    #ifdef CLK_DEBUG
        t2= Clock::now();
        print_clk("PCI_READ_FROM_KERNEL_CMD used: ", t1, t2);
    #endif

    // #ifdef CLK_DEBUG
    //     t1 = Clock::now();
    // #endif
    // ioctl(pcie_fd, PCI_UMAP_ADDR_CMD, dma_operator);        /* 释放数据缓存 */
    // #ifdef CLK_DEBUG
    //     t2 = Clock::now();
    //     print_clk("PCI_UMAP_ADDR_CMD used: ", t1, t2);
    // #endif
}