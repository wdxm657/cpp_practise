#include "dma.h"

DMA::DMA()
    : test_num(0), start(0), end(0), step(0), write_cnt(0), read_cnt(0), pcie_fd(0)
{
    dma_operator = new dma_oper;
    memset(dma_operator, 0, sizeof(dma_oper));
}

DMA::~DMA()
{
    delete dma_operator;
}

// unsigned int test_num, unsigned int start, unsigned int end, unsigned int step, unsigned int write_cnt, unsigned int read_cnt
void DMA::set_auto(std::vector<unsigned int> values, int fd)
{
    test_num = values[0];
    start = values[1];
    end = values[2];
    step = values[3];
    write_cnt = values[4];
    read_cnt = values[5];
    pcie_fd = fd;
    cout << "dma dma" << endl;
    dma_auto_process();
}

void DMA::dma_auto_process()
{
    unsigned int i = 0;
    int temp_data = 0;
    unsigned int temp_cnt = 0;
    unsigned int temp_start = 0;
    unsigned int temp_end = 0;
    unsigned int temp_step = 0;

    temp_start = (start / 4) * 4;
    temp_end = temp_start;
    temp_step = step;

    printf("test_num = %d\n", temp_start);
    printf("start    = %d\n", temp_start);
    printf("end      = %d\n", temp_end);
    printf("step     = %d\n", temp_step);

    start = temp_start;
    end = temp_end;

    // temp_data = get_write_data(temp_end, dma_operator);
    temp_data = 1;

    if (temp_data > 0)
    {
        if (0 == test_num)
        {
            printf("DMA Auto Cycle operation......\n");
            // while (1)
            // {
            //     dma_oper_fun(dma_auto, dma_operator);
            //     if (FALSE == button_flag.dma_auto)
            //         break;
            // }
        }
        else
        {
            printf("DMA Auto Operate %d times......\n", test_num);
            for (i = 0; i < test_num; i++)
            {
                dma_oper_fun();
                // if (false == button_flag)
                //     break;
                temp_cnt++;
            }
            if (temp_cnt == test_num)
            {
                // button_flag.dma_auto = set_button_text(AUTO_BUTTON_NUM, button_flag.dma_auto);
            }
        }
    }
    else
    {
        printf("Open DMA Load File Failed !!!\n");
    }
}

void DMA::dma_oper_fun()
{
    unsigned int j = 0;

    dma_operator->current_len = start + j * step;
    dma_operator->current_len = (dma_operator->current_len > end) ? end : dma_operator->current_len;
    dma_operator->current_len = dma_operator->current_len >> 2; /* 将字节转换成DW(四字节) */
    dma_operator->offset_addr = 0;
    memset(dma_operator->read_buf, 0, DMA_MAX_PACKET_SIZE);

    ioctl(pcie_fd, PCI_MAP_ADDR_CMD, dma_operator);        /* 地址映射,以及数据缓存申请 */
    ioctl(pcie_fd, PCI_WRITE_TO_KERNEL_CMD, dma_operator); /* 将数据写入内核缓存 */
    ioctl(pcie_fd, PCI_DMA_READ_CMD, dma_operator);        /* 将数据写入设备（DMA读） */
    usleep(1);
    ioctl(pcie_fd, PCI_DMA_WRITE_CMD, dma_operator); /* 将数据从设备读出到内核（DMA写） */
    usleep(1);
    ioctl(pcie_fd, PCI_READ_FROM_KERNEL_CMD, dma_operator); /* 将数据从内核读出 */
    ioctl(pcie_fd, PCI_UMAP_ADDR_CMD, dma_operator);        /* 释放数据缓存 */

    // 2048byte   512DW    1920*1080*16/8=4147200B/4=1036800DW/512DW=2025  1Frame
    // write_cnt == 2025 表示1F读写完成   60fps的话，2025*60=121500  即1s cnt要加到121500
    write_cnt = (write_cnt >= 0xffffffff) ? 0xffffffff : write_cnt + 1;
    read_cnt = write_cnt;

    /*if(memcmp(dma_operator->data.read_buf, dma_operator->data.write_buf, dma_operator->current_len*4) != 0)
    {
        dma_auto->error_cnt = (dma_auto->error_cnt>= 0xffffffff) ? 0xffffffff : dma_auto->error_cnt + 1;
    }*/
    for (int i = 0; i < dma_operator->current_len; i++)
    {
        printf("dw_cnt = %d; write_data = 0x%02x%02x%02x%02x; read_data = 0x%02x%02x%02x%02x\n", i + 1,
               dma_operator->write_buf[i * 4], dma_operator->write_buf[i * 4 + 1], dma_operator->write_buf[i * 4 + 2], dma_operator->write_buf[i * 4 + 3],
               dma_operator->read_buf[i * 4], dma_operator->read_buf[i * 4 + 1], dma_operator->read_buf[i * 4 + 2], dma_operator->read_buf[i * 4 + 3]);
    }

    // printf_debug("DMA Auto Step operation cnt = %d; write_cnt = %d; error_cnt = %d; current_len = %d (dw)\n", j, dma_auto->write_cnt, dma_auto->error_cnt, dma_operator->current_len);

    dma_operator->current_len = start;
}