#include "utils/logger.h"
#include <coreinit/fastmutex.h>
#include <coreinit/interrupts.h>
#include <coreinit/mutex.h>
#include <coreinit/scheduler.h>
#include <coreinit/thread.h>
#include <stdio.h>
#include <string.h>
#include <wups.h>
#include <wups/config/WUPSConfigItemStub.h>
#include <wups/config/WUPSConfigItemBoolean.h>

WUPSConfigAPICallbackStatus ThredListCreateThreadCategory(WUPSConfigCategoryHandle rootHandle, OSThread *thread);
