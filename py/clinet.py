import numpy as np
import cv2
from socket import *
import time
# 127.0.0.1表示本机的IP，用于测试，使用时需要改为服务端的ip
debug_once = 0
addr = ('192.168.1.110', 65533)
s_addr = ('192.168.1.109', 65533)
video_arr = [
    'daySequence1.mp4',
    'daySequence2.mp4',
    'nightSequence1.mp4',
    # 'nightSequence2.mp4'
]
index = 1
cap = cv2.VideoCapture(video_arr[index])
real_fps = cap.get(cv2.CAP_PROP_FPS)

s = socket(AF_INET, SOCK_DGRAM)  # 创建UDP套接字
s.bind(s_addr)
s.settimeout(0)
serilize = 3  # 3字节序列  0 - 479
# 这里最大应该可以改到65535  但还是按整数行发送把  不确定HDL写的有木有问题
# 可以改称19200 * 2   20line每次发送，在gen_pix.v里面把接收的技术器同步加大应该就没问题了
# 速度可能会更快  目前写完的是19200 10行发送是没问题的
fragment_size = 19200  # 6400 pix 10 line 发送36次为1帧

total_cnt = 0


def send_assert_vld(data, addr, validate=True):
    global total_cnt
    while True:
        try:
            s.sendto(data, addr)
            #  linux 150000 ~~ 25fps
            #  linux 65000  ~~ 60fps
            #  linux 25000  ~~ 150fps
            #  windo 15000  ~~ 160fps
            for i in range(25000):
                pass
            d, _ = s.recvfrom(1024)
            # if d[:3] == b'ACK':
            if d[4:6] == data[:2]:
                # print(data[:2])
                # print(d)
                # if total_cnt == 480:
                #     total_cnt = 0
                # else:
                #     total_cnt += 1
                # print(f'send and ack success, serial num: {d[4:6]}')
                break
        except Exception as e:
            pass


def vsync(addr):
    # 56 73 5f 52 69 73 65 21
    send_assert_vld("Vs_Rise!".encode(), addr)  # 0x56 0x73 0x5f 0x52 0x69 0x73 0x65 0x21
    send_assert_vld("Vs_Fall!".encode(), addr)  # 0x56 0x73 0x5f 0x46 0x61 0x6c 0x6c 0x21


def hsync(addr):
    # 0x48 0x73 0x5f 0x52 0x69 0x73 0x65 0x21
    send_assert_vld("Hs_Rise!".encode(), addr)
    # send_assert_vld("Hs_Fall!".encode(), addr)  # 0x48 0x73 0x5f 0x46 0x61 0x6c 0x6c 0x21


fram_cnt = 0
while True:
    c = 0
    flag = True
    _, img = cap.read()
    if _:
        img = cv2.resize(img, (640, 360))
        img_d = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        frame = img_d.ravel()
        # print(f'准备发送数据，总大小:{frame.size} Byte')
        vsync(addr)

        start = time.time()
        while frame.size > 0 and flag:
            if frame.size > fragment_size:
                fragment = frame[:fragment_size]
                frame = frame[fragment_size:]
            else:
                fragment = frame
                flag = False
            b = c.to_bytes(2, 'big', signed=False)
            # if c == 0:
            #     print(fragment.tobytes())
            # 给每次发送的数据加上序列号 在fpga处防止重复数据记录
            # print(f'正在发送第{c}次数据，大小:{len(fragment.tobytes())} Byte')
            # print(f'{c} 数据，{fragment.tobytes()}')
            send_assert_vld(b + fragment.tobytes(), addr)
            if debug_once:
                p = ' '.join(format(e, 'x') for e in fragment)
                print("发送的数据为", p)
                if c == 1:
                    break  # 一次只发送一片数据
            c = c + 1
        end = time.time()
        fps = 1 //(end - start)
        # fram_cnt = fram_cnt + 1
        cv2.putText(img, str(fps), (50, 50),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
        cv2.imshow('client', img)
        # 按视频帧率延时显示
        # ts = int(1000/real_fps - 30)
        # cv2.waitKey(ts)
        if cv2.waitKey(1) & 0xFF == ord('q') or fram_cnt == 100:
            break
        if debug_once:
            break  # 一次只发送一帧数据
    else:
        if len(video_arr) == index + 1:
            index = 0
        else:
            index += 1
        cap = cv2.VideoCapture(video_arr[index])

cap.release()
s.close()
# cv2.waitKey(0)
cv2.destroyAllWindows()