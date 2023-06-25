#include "dma.h"

DMA::DMA()
    : test_num(2025),
      start(2048),
      end(2048),
      step(0),
      write_cnt(0),
      read_cnt(0),
      pcie_fd(0),
      pt(0),
      process_finish(true),
      pix_col(0),
      pix_row(0),
      pix_cnt(0),
      inf(),
      wr_vec(TOTAL_FRAME_PIX * 2, 0),
      img(1080, 1920, CV_8UC3)
{
    dma_operator = new dma_oper;
    memset(dma_operator, 0, sizeof(dma_oper));
}

DMA::~DMA()
{
    delete dma_operator;
}

void DMA::dma_auto_process(int fd, cv::Mat &dst)
{
    pcie_fd = fd;
    cout << "dma auto process bigin" << endl;

    int temp_cnt = 0;
    for (int i = 0; i < test_num; i++)
    {
        dma_auto(dst);
        // cout << "dma auto bigin" << temp_cnt << endl;
        temp_cnt++;
    }
    if (temp_cnt == test_num)
    {
        cout << "A total of " << test_num / 2025 << " frames of images were read" << endl;
        // button_flag.dma_auto = set_button_text(AUTO_BUTTON_NUM, button_flag.dma_auto);
    }
}

void DMA::dma_auto(cv::Mat &dst)
{
    dma_operator->current_len = start + step;
    dma_operator->current_len = (dma_operator->current_len > end) ? end : dma_operator->current_len;
    dma_operator->current_len = dma_operator->current_len >> 2; /* 将字节转换成DW(四字节) */
    dma_operator->offset_addr = 0;

    dma_wr();

    ioctl(pcie_fd, PCI_MAP_ADDR_CMD, dma_operator);        /* 地址映射,以及数据缓存申请 */
    ioctl(pcie_fd, PCI_WRITE_TO_KERNEL_CMD, dma_operator); /* 将数据写入内核缓存 */
    ioctl(pcie_fd, PCI_DMA_READ_CMD, dma_operator);        /* 将数据写入设备（DMA读） */
    usleep(1);
    ioctl(pcie_fd, PCI_DMA_WRITE_CMD, dma_operator); /* 将数据从设备读出到内核（DMA写） */
    usleep(1);
    ioctl(pcie_fd, PCI_READ_FROM_KERNEL_CMD, dma_operator); /* 将数据从内核读出 */
    ioctl(pcie_fd, PCI_UMAP_ADDR_CMD, dma_operator);        /* 释放数据缓存 */

    dma_rd(dst);
}

void DMA::resume()
{
    simulation_fram(false);
}
void DMA::simulation_fram(bool begin)
{
    // 模拟一帧开始的信号
    dma_operator->current_len = 4;
    dma_operator->offset_addr = 0;
    for (int i = 0; i < 16; i++)
    {
        dma_operator->write_buf[i] = begin ? 0x88 : 0x99;
    }
    // printf("hsync simulation\n");
    ioctl(pcie_fd, PCI_MAP_ADDR_CMD, dma_operator);        /* 地址映射,以及数据缓存申请 */
    ioctl(pcie_fd, PCI_WRITE_TO_KERNEL_CMD, dma_operator); /* 将数据写入内核缓存 */
    ioctl(pcie_fd, PCI_DMA_READ_CMD, dma_operator);        /* 将数据写入设备（DMA读） */
    usleep(1);
    ioctl(pcie_fd, PCI_DMA_WRITE_CMD, dma_operator); /* 将数据从设备读出到内核（DMA写） */
    usleep(1);
    ioctl(pcie_fd, PCI_READ_FROM_KERNEL_CMD, dma_operator); /* 将数据从内核读出 */
    ioctl(pcie_fd, PCI_UMAP_ADDR_CMD, dma_operator);        /* 释放数据缓存 */

    dma_operator->current_len = start + step;
    dma_operator->current_len = dma_operator->current_len >> 2; /* 将字节转换成DW(四字节) */
    dma_operator->offset_addr = 0;
    usleep(1);
}

void DMA::dma_wr()
{
    if (pix_cnt == 0)
    {
        simulation_fram(true);
    }
    memset(dma_operator->write_buf, 0, 4096);
    pix_cnt = pix_cnt == 2024 ? 0 : pix_cnt + 1;
}

void DMA::dma_rd(cv::Mat &dst)
{
    for (int i = 0; i < dma_operator->current_len; i++)
    {
        // printf("dw_cnt = %d; write_data = 0x%02x%02x%02x%02x; read_data = 0x%02x%02x%02x%02x;\ntotal_cnt = %d;\n", i + 1,
        //        dma_operator->write_buf[i * 4], dma_operator->write_buf[i * 4 + 1], dma_operator->write_buf[i * 4 + 2], dma_operator->write_buf[i * 4 + 3],
        //        dma_operator->read_buf[i * 4], dma_operator->read_buf[i * 4 + 1], dma_operator->read_buf[i * 4 + 2], dma_operator->read_buf[i * 4 + 3],
        //        pix_cnt);
        for (int j = 0; j < 4; j += 2)
        {
            uint16_t pix = (static_cast<uint16_t>(dma_operator->read_buf[i * 4 + j + 1]) << 8) | static_cast<uint16_t>(dma_operator->read_buf[i * 4 + j]);
            uint8_t r = (pix >> 11) & 0x1f;
            uint8_t g = (pix >> 5) & 0x3f;
            uint8_t b = pix & 0x1f;
            img.at<cv::Vec3b>(pix_row, pix_col) = cv::Vec3b(r << 3, g << 2, b << 3);
            if (pix_col == 1919 && pix_row == 1079)
            {
                simulation_fram(false);
                // 1920*1080*16 / 8 = 4147200 B / 512 DW = 8100 / 4 = 2025
                // dst = img;
                dst = inf.base_exam(img);
            }
            pix_col = pix_col == 1919 ? 0 : pix_col + 1;
            pix_row = pix_col == 1919 && pix_row == 1079 ? 0 : pix_col == 1919 ? pix_row + 1
                                                                               : pix_row;
        }
    }
}