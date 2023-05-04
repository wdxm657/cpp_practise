#include "dma.h"

DMA::DMA()
    : test_num(0), start(0), end(0), step(0), write_cnt(0), read_cnt(0)
{
}

DMA::~DMA()
{
}

// unsigned int test_num, unsigned int start, unsigned int end, unsigned int step, unsigned int write_cnt, unsigned int read_cnt
void DMA::set_auto(std::vector<unsigned int> values)
{
    test_num = values[0];
    start = values[1];
    end = values[2];
    step = values[3];
    write_cnt = values[4];
    read_cnt = values[5];
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

    // temp_data = get_write_data(temp_end, dma_oper);
    temp_data = 1;

    if (temp_data > 0)
    {
        if (0 == test_num)
        {
            printf("DMA Auto Cycle operation......\n");
            // while (1)
            // {
            //     dma_oper_fun(dma_auto, dma_oper);
            //     if (FALSE == button_flag.dma_auto)
            //         break;
            // }
        }
        else
        {
            printf("DMA Auto Operate %d times......\n", test_num);
            for (i = 0; i < test_num; i++)
            {
                // dma_oper_fun(dma_auto, dma_oper);
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