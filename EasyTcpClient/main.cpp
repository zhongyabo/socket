#include "EasyTcpClient.hpp"
#include "CellTimestamp.hpp"
#include<thread>
#include <atomic>
bool g_bRun=true;

void cmdThread()
{
    while (true)
    {
        char cmdBuf[256] = {};
        scanf("%s", cmdBuf);
        if (0 == strcmp(cmdBuf, "exit"))
        {
            g_bRun = false;
            printf("�˳�cmdThread�߳�\n");
            break;
        }
        else {
            printf("��֧�ֵ����\n");
        }
    }
}

//�ͻ�������
const int cCount = 1000;
//�����߳�����
const int tCount = 4;
//�ͻ�������
EasyTcpClient* client[cCount];

const int c = cCount / tCount;

std::atomic_int sendCount(0);

std::atomic_int readyCount(0);

void recvThread(int begin,int end)
{
    while (g_bRun)
    {
        for (int n = begin; n < end; n++)
        {
            client[n]->OnRun();
        }
    }
}
void sendThread(int id)
{
    printf("thread<%d>,start\n",id);
    //4���߳� ID 1~4
    int begin = (id - 1)*c;
    int end = id*c;
    
    for (int n = begin; n < end; n++)
    {
        client[n] = new EasyTcpClient();
    }
    
    for (int n = begin; n < end; n++)
    {
        client[n]->Connect("182.16.70.186", 9999);
//        printf("connect:%d\n",n);
        std::chrono::milliseconds t(10);
        std::this_thread::sleep_for(t);
    }
    printf("thread<%d>,Connect<begin=%d,end=%d>\n", id,begin,end);
    
    readyCount++;
    while(readyCount<tCount)
    {
        std::chrono::milliseconds t(10);
        std::this_thread::sleep_for(t);
    }
    //std::chrono::milliseconds t(10);
    //std::this_thread::sleep_for(t);
    
    std::thread t1(recvThread,begin,end);
    t1.detach();
    
    Login login[1];
    for (int n = 0; n < 1; n++)
    {
        strcpy(login[n].userName, "lyd");
        strcpy(login[n].PassWord, "lydmm");
    }
    
    while (g_bRun)
    {
        for (int n = begin; n < end; n++)
        {
            if(SOCKET_ERROR !=client[n]->SendData(login,sizeof(login)))
            {
                sendCount++;
            }
            std::chrono::milliseconds t(5);
            std::this_thread::sleep_for(t);
        }
        
    }
    for (int n = begin; n < end; n++)
    {
        client[n]->Close();
        delete client[n];
    }
    
}

int main()
{
    //����UI�߳�
    std::thread t1(cmdThread);
    t1.detach();
    
    //���������߳�
    for (int n = 0; n < tCount; n++)
    {
        std::thread t1(sendThread,n+1);
        t1.detach();
    }
    
    std::chrono::milliseconds t(10);
    std::this_thread::sleep_for(t);
    CellTimestamp tTime;
    while (g_bRun)
    {
        auto t = tTime.getElapsedSecond();
        if(t>=1.0)
        {
            printf("thread<%d>,clients<%d>,time<%lf>,send<%d>\n",tCount,cCount,t,(int)(sendCount/t));
            tTime.update();
            sendCount = 0;
        }
        //sleep(1);
        std::this_thread::sleep_for(1000ms);
    }
    printf("���˳���\n");
    return 0;
}
