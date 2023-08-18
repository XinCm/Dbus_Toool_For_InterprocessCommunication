/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef vendor_yfvet_infrastructure_agw_AgwManager_H_
#define vendor_yfvet_infrastructure_agw_AgwManager_H_

#include <functional>
#include <thread>
#include <mutex>
#include "AgwClient.h"
#include "MessageQueue.h"
#include "IAgwService.h"
namespace vendor {
namespace yfvet {
namespace infrastructure {
namespace agw {

enum class State {
    INIT = 0,
    RUNNING = 1,
    STOP_REQUESTED = 2,
    STOPPED = 3,
};
enum class MessageId {
    CONNECT_SERVER = 0,
    DISCONNECT_SERVER = 1,
};

using handleMessageFunc = std::function<void(const std::vector<MessageId>& vec)>;

class AgwListener : public RefBase{
public:
    virtual ~AgwListener() = default;
    virtual void onServiceConnected(AgwClient&){};
    virtual void onServiceDisconnected(){};
};

class AgwManager {
public: 
    static AgwManager* getManager();
    int32_t connect(sp<AgwListener> listener);
    void wait();

    AgwManager(const AgwManager&) = delete;
    AgwManager& operator = (const AgwManager &) = delete;
protected:
    virtual ~AgwManager() = default;
private:
    AgwManager();
    void agwServiceDied();
    void runInternal(AgwManager*);
    void handleMessage(const std::vector<MessageId>& s);
    int32_t startService(sp<IAgwService>& service);
private:
    //sp<IBinder::DeathRecipient> mDeathObserver;
    std::thread mWorkerThread;
    Queue<MessageId> mQueue;
    std::atomic<State> mState;
    mutable std::mutex mLock;
    sp<AgwListener> mListener;
    AgwClient* mClient;
    sp<IBinder::DeathRecipient> mdeathObserver;
};

}  // namespace agw
}  // namespace infrastructure
}  // namespace yfvet
}  // namespace vendor


#endif  // vendor_yfvet_infrastructure_agw_AgwManager_H_