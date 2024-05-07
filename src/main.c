#include "utils/logger.h"
#include <coreinit/interrupts.h>
#include <coreinit/scheduler.h>
#include <coreinit/thread.h>
#include <wups.h>
#include <wups/config/WUPSConfigItemStub.h>

/**
    Mandatory plugin information.
    If not set correctly, the loader will refuse to use the plugin.
**/
WUPS_PLUGIN_NAME("Threads Viewer");
WUPS_PLUGIN_DESCRIPTION("Displays a list of active threads");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("ItzSwirlz, Maschell");
WUPS_PLUGIN_LICENSE("BSD");

WUPS_USE_WUT_DEVOPTAB();            // Use the wut devoptabs
WUPS_USE_STORAGE("threads_viewer"); // Unique id for the storage api

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle root) {
    {
        OSThread *curThread = OSGetCurrentThread();
        int state           = OSDisableInterrupts();
        __OSLockScheduler(curThread);
        OSThread *t = *((OSThread **) 0x100567F8); // active threadlist address
        if(t == NULL) {
            DEBUG_FUNCTION_LINE_ERR("The first thread obtained was NULL. This should never happen.");
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        }

        OSThread *threads[30]; // lets hope we dont have more than 100
        int i = 0;

        // threads are handled by a linked list.
        // Since the scheduler is locked and interrupts are disabled, to save time,
        // have a local array pointing to these threads instead of making new ones
        // and holding up the system
        while (t->activeLink.next != NULL && t != NULL && i < 30) {
            threads[i] = t;
            if(t->activeLink.next == NULL) break;
            t          = t->activeLink.next;
            i++;
        }
        __OSUnlockScheduler(curThread);
        OSRestoreInterrupts(state);

        if(threads[0] == NULL) {
            DEBUG_FUNCTION_LINE_ERR("The first thread obtained was NULL. This should never happen.");
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        }
        for (int i = 0; i < sizeof(threads) / sizeof(threads[0]); i++) {
            if (threads[i]) {
                if(threads[i]->name != NULL) {
                if (WUPSConfigItemStub_AddToCategory(root, threads[i]->name) != WUPSCONFIG_API_RESULT_SUCCESS) {
                    DEBUG_FUNCTION_LINE_ERR("Failed to add thread %d to the list", threads[i]->name);
                    return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
                };
                }
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
    DEBUG_FUNCTION_LINE("INITIALIZE_PLUGIN of threads_viewer!");

    WUPSConfigAPIOptionsV1 configOptions = {.name = "Threads Viewer"};
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
    DEBUG_FUNCTION_LINE("DEINITIALIZE_PLUGIN of threads_viewer!");
}

/**
    Gets called when an application starts.
**/
ON_APPLICATION_START() {
    initLogging();

    DEBUG_FUNCTION_LINE("ON_APPLICATION_START of threads_viewer!");
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
    DEBUG_FUNCTION_LINE_INFO("ON_APPLICATION_REQUESTS_EXIT of threads_viewer!");
}
