//
// Created by Administrator on 2023/2/4.
//

#include "TouchEventTracer.h"
#include "logger.h"
#include "BlockCanary.h"
#include "unistd.h"

#include <thread>
#include <ctime>
#include <deque>
#include <chrono>

using namespace std;

static mutex queueMutex;
static lock_guard<mutex> lock(queueMutex);
static bool loopRunning = false;
static bool startDetect = false;
static int LAG_THRESHOLD;

long lastRecvTouchEventTimeStamp = 0;
static int  currentFd = 0;
static int lagFd = 0;

void reportLag(){
    if (lagFd == 0){
        return;
    }
    onTouchEventLag(lagFd);
}

void TouchEventTracer::touchRecv(int fd) {
    currentFd = fd;
    lagFd = 0;

    if (lagFd == fd){
        lastRecvTouchEventTimeStamp = 0;
    } else {
        lastRecvTouchEventTimeStamp = time(nullptr);
        queueMutex.unlock();
    }

}

void TouchEventTracer::touchSendFinish(int fd) {
    if (lagFd == fd){
        reportLag();
    }
    //reset
    lagFd = 0;
    lastRecvTouchEventTimeStamp = 0;
    startDetect = true;
}

void recvQueueLooper(){
    queueMutex.lock();
    while (loopRunning){
        if (lastRecvTouchEventTimeStamp == 0 ){
            queueMutex.lock();
        } else {
            long lastRecvTouchEventTimeStampNow =  lastRecvTouchEventTimeStamp;
            if (lastRecvTouchEventTimeStampNow <=0){
                continue;
            }

            if (time(nullptr) - lastRecvTouchEventTimeStampNow >= LAG_THRESHOLD &&
                    startDetect){
                lagFd = currentFd;

                //todo invoke method
                //report lag

                queueMutex.lock();

            }

        }


    }

}


void TouchEventTracer::start(int threshold) {
    LAG_THRESHOLD = threshold /1000;
    loopRunning = true;

    thread recvThread = thread(recvQueueLooper);
    recvThread.detach();
}

