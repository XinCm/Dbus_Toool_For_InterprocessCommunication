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

#ifndef vendor_yfvet_infrastructure_agw_AgwClient_H_
#define vendor_yfvet_infrastructure_agw_AgwClient_H_

#include <string>
#include <vector>
#include <list>
#include <utility>
#include <set>
#include "IAgwCallback.h"
#include "IAgwService.h"
namespace vendor {
namespace yfvet {
namespace infrastructure {
namespace agw {

class AgwEvent : public RefBase{
public:
	virtual ~AgwEvent() = default;
	virtual void onEventChanged(const target&, const std::vector<uint8_t>&){};
};

class AgwClient {
public:
    int32_t registerEvent(const std::list<target>& names, sp<AgwEvent> evt);
    int32_t set(const target& name, const std::vector<uint8_t>& payload);
    int32_t asyncset(const target& name, const std::vector<uint8_t>& payload);
    void test();
private:
	sp<IAgwService> mService;
	sp<IAgwCallback> mCallback;
	sp<AgwEvent> mListener;
private:
	AgwClient(sp<IAgwService> service);
	virtual ~AgwClient();
    AgwClient(const AgwClient&) = delete;
    AgwClient& operator = (const AgwClient &) = delete;
    friend class AgwManager;
	class AgwCallback : public BnAgwCallback{
	public:
		AgwCallback(AgwClient* parent);
	private:
		virtual void test();
		virtual void onEventChanged(const target& name, std::vector<uint8_t> payload);
	private:
		AgwClient* mAgwClient;
	};
};

}  // namespace agw
}  // namespace infrastructure
}  // namespace yfvet
}  // namespace vendor


#endif  // vendor_yfvet_infrastructure_agw_AgwClient_H_
