#ifndef _CONFIG_GUI_H
#define _CONFIG_GUI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h> 
#include <sys/ioctl.h> 
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>

 
#define NO_TEST														/* 代码功能测试时打开注释 */
#define DEBUG
/* 宽间距为了兼容低版本，部件尺寸标准不一致 */
//#define WIDGET_SPACE													/* 部件间距，宏定义打开表示为宽松；注释掉表示间距紧凑 */

#define PCIE_DRIVER_FILE_PATH 			"/dev/pango_pci_driver"			/* pcie驱动文件目录 */
#define MEM_FILE_PATH 					"/dev/mem"						/* mem驱动文件目录 */
#define VEISION 						"Pango PCIe Test v1.0"			/* 软件测试版本信息 */

#define TYPE 							'S'
#define PCI_READ_DATA_CMD    			_IOWR(TYPE, 0, int)				/* 读数据指令 */
#define PCI_WRITE_DATA_CMD    			_IOWR(TYPE, 1, int)				/* 写数据指令 */
#define PCI_MAP_ADDR_CMD    			_IOWR(TYPE, 2, int)				/* DMA总线地址映射 */
#define PCI_WRITE_TO_KERNEL_CMD    		_IOWR(TYPE, 3, int)				/* 写内核数据操作 */
#define PCI_DMA_READ_CMD    			_IOWR(TYPE, 4, int)				/* DMA读操作 */
#define PCI_DMA_WRITE_CMD    			_IOWR(TYPE, 5, int)				/* DMA写操作 */
#define PCI_READ_FROM_KERNEL_CMD    	_IOWR(TYPE, 6, int)				/* 读取内核数据操作 */
#define PCI_UMAP_ADDR_CMD    			_IOWR(TYPE, 7, int)				/* 释放映射地址 */
#define PCI_PERFORMANCE_START_CMD		_IOWR(TYPE, 8, int)				/* 性能测试开始操作 */
#define PCI_PERFORMANCE_END_CMD			_IOWR(TYPE, 9, int)				/* 性能测试结束操作 */



#define MAX_BLOCK_SIZE   				1024							/* 位流分块缓存区最大值 */
#define LINK_OK							0x11							/* PCI链接成功，返回值 bit[4:0]*/
#define CRC_OK							0xa00							/* 位流加载完成后，校验成功返回值 bit[11:8]*/
#define CRC_ERROR						0xe00							/* 位流加载完成后，校验错误返回值 bit[11:8]*/
#define CRC_REPEAT						0xf00							/* 位流加载完成后，重新读取校验返回值 bit[11:8]*/
#define AXI_CONNECT_USER				0xa								/* AXI链接用户接口配置值 bit[3:0]*/
#define AXI_CONNECT_SWITCH				0xf								/* AXI链接转换接口配置值 bit[3:0]*/
#define LOAD_DATA_FINISH        		0xa0							/* 位流加载成功返回值 bit[7:4]*/
#define LOAD_DATA_UNFINISH				0x00							/* 位流正在加载返回值 bit[7:4]*/
#define BAR_OFFSET_1					0x00							/* 读取PCI状态地址偏移地址 */
#define BAR_OFFSET_2					0x10							/* 配置PCI加载偏移地址 */
#define BAR_OFFSET_3					0x20							/* 加载位流偏移地址 */
#define PEFORMANCE_STATUS_OFFSET		0x00							/* 性能测试状态寄存器 */
#define PEFORMANCE_WRITE_CNT_OFFSET		0x04							/* 性能测试DMA写计数 */	
#define PEFORMANCE_READ_CNT_OFFSET		0x08							/* 性能测试DMA读计数 */	
#define PEFORMANCE_ERROR_CNT_OFFSET		0x0c							/* 性能测试错误计数 */	
#define PEFORMANCE_DATA_CNT_OFFSET		0x10							/* 性能测试数据包计数 */	

#define PAGE_ROUND_DOWN(x)				((x) & ~(getpagesize() - 1))
#define PAGE_ROUND_UP(x)				(PAGE_ROUND_DOWN((x) + getpagesize() - 1))
#define file_len(len)					((len)%4 == 0 ? (len)/4 : ((len)/4)+1)
#define BOOL_SWITCH(flag)  				(((flag) == TRUE) ? FALSE : TRUE)
#define DMA_MAX_PACKET_SIZE   			4096							/* DMA包最大值 */
#define DMA_MIN_PACKET_SIZE   			4								/* DMA包最小值 */

typedef struct _DMA_DATA_
{
	unsigned char read_buf[DMA_MAX_PACKET_SIZE];
	unsigned char write_buf[DMA_MAX_PACKET_SIZE];
}DMA_DATA;

typedef struct _DMA_OPERATION_
{
	unsigned int current_len;
	unsigned int offset_addr;
	unsigned int cmd;
	DMA_DATA data;
}DMA_OPERATION;

typedef struct _DMA_AUTO_
{
	unsigned int test_num;
	unsigned int start;
	unsigned int end;
	unsigned int step;
	unsigned int write_cnt;
	unsigned int read_cnt;
	unsigned int error_cnt;
	unsigned int step_add_cnt;
}DMA_AUTO;


#endif // _CONFIG_GUI_H
