#include "dma.h"

DMA::DMA()
    : test_num(0),
      start(0),
      end(0),
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

void DMA::set_dma_oper(std::vector<unsigned int> values, int fd)
{
    test_num = values[0];
    start = values[1];
    end = values[2];
    step = values[3];
    write_cnt = values[4];
    read_cnt = values[5];

    pcie_fd = fd;

    cout << "dma auto process bigin" << endl;
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

    temp_data = 1;

    if (0 == test_num)
    {
        printf("DMA Auto Cycle operation......\n");
        while (1)
        {
            dma_rd();
            // if (FALSE == button_flag.dma_auto)
            //     break;
        }
    }
    else
    {
        printf("DMA Auto Operate %d times......\n", test_num);
        for (i = 0; i < test_num; i++)
        {
            dma_rd();
            // if (false == button_flag)
            //     break;
            temp_cnt++;
        }
        if (temp_cnt == test_num)
        {
            cout << "A total of " << test_num / 2025 << " frames of images were read" << endl;
            // button_flag.dma_auto = set_button_text(AUTO_BUTTON_NUM, button_flag.dma_auto);
        }
    }
}
int a = 0;
void DMA::dma_rd()
{
    dma_operator->current_len = start + step;
    dma_operator->current_len = (dma_operator->current_len > end) ? end : dma_operator->current_len;
    dma_operator->current_len = dma_operator->current_len >> 2; /* 将字节转换成DW(四字节) */
    dma_operator->offset_addr = 0;

    for (int i = 0; i < dma_operator->current_len; i++)
    {
        for (int j = 0; j < 4; j += 2)
        {
            // write_buf          0   1   2   3   4   5  ... 2046   2047    0    1    ...
            // pix_row_ pix_col_  0   0   0   1   0   2  ... 0      1023    0    1024 ...
            // wr_vec             0   1   2   3   4   5  ... 2046   2047    2048 2049 ...
            //  wr_vec[0---2047]   [2048---4095]
            int k = 2048 * pix_cnt;
            dma_operator->write_buf[i * 4 + j] = wr_vec[i * 4 + j + k];
            dma_operator->write_buf[i * 4 + j + 1] = wr_vec[i * 4 + j + 1 + k];
            // printf("%d\n",i * 4 + j);
            // printf("%d\n",i * 4 + j + 1);
            // printf("0x%0x\n",wr_vec[i * 4 + j + k]);
            // printf("0x%0x\n",wr_vec[i * 4 + j + 1 + k]);
            // usleep(100000);
        }
    }
    pix_cnt = pix_cnt == 2024 ? 0 : pix_cnt + 1;

    ioctl(pcie_fd, PCI_MAP_ADDR_CMD, dma_operator);        /* 地址映射,以及数据缓存申请 */
    ioctl(pcie_fd, PCI_WRITE_TO_KERNEL_CMD, dma_operator); /* 将数据写入内核缓存 */
    ioctl(pcie_fd, PCI_DMA_READ_CMD, dma_operator);        /* 将数据写入设备（DMA读） */
    usleep(1);
    ioctl(pcie_fd, PCI_DMA_WRITE_CMD, dma_operator); /* 将数据从设备读出到内核（DMA写） */
    usleep(1);
    ioctl(pcie_fd, PCI_READ_FROM_KERNEL_CMD, dma_operator); /* 将数据从内核读出 */
    ioctl(pcie_fd, PCI_UMAP_ADDR_CMD, dma_operator);        /* 释放数据缓存 */

    for (int i = 0; i < dma_operator->current_len; i++)
    {
        printf("dw_cnt = %d; write_data = 0x%02x%02x%02x%02x; read_data = 0x%02x%02x%02x%02x;\n total_cnt = %d;\n", i + 1,
               dma_operator->write_buf[i * 4], dma_operator->write_buf[i * 4 + 1], dma_operator->write_buf[i * 4 + 2], dma_operator->write_buf[i * 4 + 3],
               dma_operator->read_buf[i * 4], dma_operator->read_buf[i * 4 + 1], dma_operator->read_buf[i * 4 + 2], dma_operator->read_buf[i * 4 + 3],
               pix_cnt);
        for (int j = 0; j < 4; j += 2)
        {
            uint16_t pix = (static_cast<uint16_t>(dma_operator->read_buf[i * 4 + j + 1]) << 8) | static_cast<uint16_t>(dma_operator->read_buf[i * 4 + j]);
            uint8_t r = (pix >> 11) & 0x1f;
            uint8_t g = (pix >> 5) & 0x3f;
            uint8_t b = pix & 0x1f;
            img.at<cv::Vec3b>(pix_row, pix_col) = cv::Vec3b(b << 3, g << 2, r << 3);
            if ((pix_col + 1) * (pix_row + 1) == TOTAL_FRAME_PIX)
            {
                a = 1;
                // 准备获取处理好的图像数据,清空之前的图像数据
                wr_vec.clear();
                // 开一个线程去处理图像，获取图像的线程根据处理图像的线程是否完成去准备一帧图像，若完成则直接传入，未完成则继续接收但只继续接收一帧
                // finish默认为true，传入pix_buffer在dma_wr中缓存pix_buffer并 拉低finish，清空pix_buffer,并准备好下一帧图像，阻塞等待dma_wr完成
                // while (process_finish)
                // {
                // 1920*1080*16 / 8 = 4147200 B / 2048 B = 2025
                cv::Mat prmat = inf.base_exam(img);
                float scale = 0.8;
                cv::resize(prmat, prmat, cv::Size(prmat.cols * scale, prmat.rows * scale));
                cv::imshow("Inference", prmat);
                cv::waitKey(-1);
                cv::destroyAllWindows();

                // BGR888转换成BGR565
                for (int i = 0; i < 1080; i++)
                {
                    uint8_t *src_row = prmat.ptr<uint8_t>(i);
                    for (int j = 0; j < 1920; j++)
                    {
                        uint8_t b = src_row[j * 3];
                        uint8_t g = src_row[j * 3 + 1];
                        uint8_t r = src_row[j * 3 + 2];
                        ushort RGB565 = (r & 0xf8) << 8 | (g & 0xf8) << 3 | (b & 0xf8) >> 3;
                        printf("rgb565 0x%02x\n", RGB565);
                        uint8_t m = (RGB565 & 0xFF00) >> 8; // 取高字节,右移8位
                        wr_vec.push_back(m);
                        uint8_t n = RGB565 & 0x00FF; // 取低字节
                        wr_vec.push_back(n);
                    }
                }
                // break;
                // }
            }
            pix_col = pix_col == 1919 ? 0 : pix_col + 1;
            pix_row = pix_col == 1919 && pix_row == 1079 ? 0 : pix_col == 1919 ? pix_row + 1
                                                                               : pix_row;
        }
    }
}

void DMA::dma_wr()
{
    process_finish = false;
    cout << "dma_wr in" << endl;
    process_finish = true;
}
