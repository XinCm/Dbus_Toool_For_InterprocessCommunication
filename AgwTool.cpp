#define LOG_TAG "agwtool"
#include <android/log.h>
#include <log/log.h>
#include <list>
#include <vector>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <utils/Log.h>
#include <stdlib.h>
#include <stdio.h>
#include "AgwManager.h"
#include <cstring>
#include <iostream>
#include <cstdio>
#include <thread>

using namespace vendor::yfvet::infrastructure::agw;
using namespace android;

static std::list<target> sList = {
  {VipGroupName("ipcl.out.to.droid"),EventName("DSP2DROIDAUDIO_RESP")},
  {QnxGroupName("vgw.in.diag.sigout"),EventName("agw_request_DID_message")},
  {QnxGroupName("vgw.in.diag.sigout"),EventName("agw_request_IOControl_message")},
}; 
class listener;
class Event;
AgwClient* sClient = nullptr;
AgwManager* sManager = nullptr;
sp<listener> sListener = nullptr;
sp<Event> sEvent = nullptr;
bool isGear = false;
bool isEND = false;

class Event : public AgwEvent {
public:
	void onEventChanged(const target& name, const std::vector<uint8_t>& payload) {
		ALOGI("onEventChanged groupname=%s,memname=%s payload_size=%ld payload[0]:%d\n",
			name.first.c_str(), name.second.c_str(), payload.size(), payload[0]);
		static uint8_t cnt = 0;
		if (payload[0] != cnt) {
			ALOGE("miss shm cnt:%d payload:%d.", cnt, payload[0]);
		}
		cnt++;
	};

};

class listener : public AgwListener {
public:
	virtual ~listener() = default;
	void onServiceConnected(AgwClient &client) override {
		printf("onServiceConnected \n");
		sClient = &client;
		//Event evt;
		if(!sClient->registerEvent(sList, sEvent)) 
            isGear = true;
        ALOGD("the scline_init is %p\n",sClient);
		ALOGD("registerEvent isGear=%d \n", isGear);
	};

	void onServiceDisconnected() override {
		ALOGE("onServiceDisconnected \n");
		sClient = nullptr;
		sManager->connect(sListener);
	};
};
    
void testThread() {
	ALOGI("agw testThread start \n");
        if(!sManager->connect(sListener)){
            while(isEND){
		   // sManager->wait();
            printf("after wait\n");
            return ;
            }
    }
            ALOGI("agw testThread end \n");
   // pthread_exit(NULL);
}

void print_usage () {
	printf("Usage:\n");
	printf("agwtool <sendmode> <GroupName> <EventName> <data[0]> <data[1]> ......\n");
    printf("            sendmode  1: to_QNX(dbus)  2: to_QNX(ShM) 3: to_VIP(AIPCL) \n");
}

int Agw_send(uint8_t *payload, uint8_t data_length,char *GName,char *ENmae,char* mode){
    std::vector<uint8_t> a(payload,payload+data_length);
    int sel = atoi(mode);
    ALOGD("the sCline is %p\n",sClient);
    if (sClient != nullptr){
        switch(sel){
            case 1: 
                /*Android->QNX(Dbus)*/
                sClient->set({QnxGroupName(GName),EventName(ENmae) }, a);
                ALOGD("end the case 1\n");
                break;
            case 2:
                /*Android->QNX(ShareMemory)*/
                sClient->set({ShmGroupName(GName),EventName(ENmae) }, a);
                ALOGD("end the case 2\n");
                break;
            case 3:
                /*Android->VIP(AIPCL)*/
		        sClient->set({VipGroupName(GName),EventName(ENmae)}, a);
                ALOGD("end the case 3\n");
                break;
            default:
                ALOGD("wrong send mode!\n");
                return -1;
        }
    }
    return 0;
}

void agwtool_init(){
    sListener = new listener();
	sEvent = new Event();
    sManager = AgwManager::getManager();
    std::thread t(testThread);
    t.detach();
}

/*
    argv[1] :   send_mode
    argv[2] :   GroupName
    argv[3] :   EventName
    argv[4-N] : data
*/
int main(int argc,char *argv[]) {

    if(argc == 1 ){
        print_usage(); 
        return 0;
    }
    if(argc>=4){
    uint8_t agw_d[argc-4];
    //agwtool_init();

    sListener = new listener();
	sEvent = new Event();
    sManager = AgwManager::getManager();
    std::thread t(testThread);
    //t.detach();
    printf("after testThread\n");
    for(int i=argc; i>=5; i--){
         agw_d[i-5] = atoi(argv[i-1]);
    }

    while (!isGear)     sleep(0);
    printf("enter agwsend\n");
    Agw_send(agw_d,argc-4,argv[2],argv[3],argv[1]); 
        isEND = true;
        t.joinable();
    printf("exit agwsend\n");
    }
    else{
        printf("Wrong Parameter!\n");
    }
 
    printf("main exit\n");
    delete sListener;
    delete sEvent;
    delete sManager;
    return 0;
}
