#include "threadcategory.h"
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

/**
    Mandatory plugin information.
    If not set correctly, the loader will refuse to use the plugin.
**/
WUPS_PLUGIN_NAME("Thred List");
WUPS_PLUGIN_DESCRIPTION("Displays a list of active threads");
WUPS_PLUGIN_VERSION("v1.1");
WUPS_PLUGIN_AUTHOR("ItzSwirlz, Maschell");
WUPS_PLUGIN_LICENSE("BSD");

WUPS_USE_WUT_DEVOPTAB();        // Use the wut devoptabs
WUPS_USE_STORAGE("thred_list"); // Unique id for the storage api

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle root) {
    {
        int sizeThreads = OSCheckActiveThreads();
        if (sizeThreads == 0) {
            DEBUG_FUNCTION_LINE_ERR("OSCheckActiveThreads() claims no threads are running. This should never happen.");
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        }

        OSThread *threads[sizeThreads];
        int i = 0;

        OSThread *t = *((OSThread **) 0x100567F8); // active threadlist address
        if (t == NULL) {
            DEBUG_FUNCTION_LINE_ERR("The first thread obtained was NULL. This should never happen.");
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        }

        // threads are handled by a linked list.
        // Since the scheduler is locked and interrupts are disabled, to save time,
        // have a local array pointing to these threads instead of making new ones
        // and holding up the system
        OSThread *curThread = OSGetCurrentThread();
        int state           = OSDisableInterrupts();
        __OSLockScheduler(curThread);
        while (t && i < sizeThreads) {
            threads[i] = t;
            t          = t->activeLink.next;
            i++;
        }
        __OSUnlockScheduler(curThread);
        OSRestoreInterrupts(state);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

        // this will probably be initialized and shouldn't matter, so override the compiler warning
        // if(null) is basically what this is, threads[0] given the above code should be NULL
        // if the first thread obtained actually was NULL.
        if (!threads[0]) {
            DEBUG_FUNCTION_LINE_ERR("The first thread obtained was NULL. This should never happen.");
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        }
#pragma GCC diagnostic pop

        for (int i = 0; i < sizeThreads; i++) {
            if (ThredListCreateThreadCategory(root, threads[i]) != WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS) {
                return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
            }
        }
    }

    return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
}

void ConfigMenuClosedCallback() {
    WUPSStorageAPI_SaveStorage(false);
}

/**
    Gets called ONCE when the plugin was loaded.
**/
INITIALIZE_PLUGIN() {
    // Logging only works when compiled with `make DEBUG=1`. See the README for more information.
    initLogging();
    DEBUG_FUNCTION_LINE("INITIALIZE_PLUGIN of thred_list!");

    WUPSConfigAPIOptionsV1 configOptions = {.name = "Thred List"};
    if (WUPSConfigAPI_Init(configOptions, ConfigMenuOpenedCallback, ConfigMenuClosedCallback) != WUPSCONFIG_API_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to init config api");
    }

    WUPSStorageAPI_SaveStorage(false);

    deinitLogging();
}

/**
    Gets called when the plugin will be unloaded.
**/
DEINITIALIZE_PLUGIN() {
    DEBUG_FUNCTION_LINE("DEINITIALIZE_PLUGIN of thred_list!");
}

/**
    Gets called when an application starts.
**/
ON_APPLICATION_START() {
    initLogging();

    DEBUG_FUNCTION_LINE("ON_APPLICATION_START of thred_list!");
}

/**
 * Gets called when an application actually ends
 */
ON_APPLICATION_ENDS() {
    deinitLogging();
}

/**
    Gets called when an application request to exit.
**/
ON_APPLICATION_REQUESTS_EXIT() {
    DEBUG_FUNCTION_LINE_INFO("ON_APPLICATION_REQUESTS_EXIT of thred_list!");
}
