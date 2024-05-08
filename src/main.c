#include "utils/logger.h"
#include <coreinit/interrupts.h>
#include <coreinit/scheduler.h>
#include <coreinit/thread.h>
#include <stdio.h>
#include <wups.h>
#include <wups/config/WUPSConfigItemStub.h>

/**
    Mandatory plugin information.
    If not set correctly, the loader will refuse to use the plugin.
**/
WUPS_PLUGIN_NAME("Thred List");
WUPS_PLUGIN_DESCRIPTION("Displays a list of active threads");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("ItzSwirlz, Maschell");
WUPS_PLUGIN_LICENSE("BSD");

WUPS_USE_WUT_DEVOPTAB();         // Use the wut devoptabs
WUPS_USE_STORAGE("thred_list"); // Unique id for the storage api

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle root) {
    {
        int sizeThreads = OSCheckActiveThreads();
        if (sizeThreads == 0) {
            DEBUG_FUNCTION_LINE_ERR("OSCheckActiveThreads() claims no threads are running. This should never happen.");
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        }

        OSThread *threads[sizeThreads];
        // WUPSConfigCategoryHandle categoryHandles[sizeThreads];
        // WUPSConfigAPICreateCategoryOptionsV1 categoryOptions[sizeThreads];
        OSThread *curThread = OSGetCurrentThread();
        int state           = OSDisableInterrupts();
        int i               = 0;

        __OSLockScheduler(curThread);
        OSThread *t = *((OSThread **) 0x100567F8); // active threadlist address
        if (t == NULL) {
            DEBUG_FUNCTION_LINE_ERR("The first thread obtained was NULL. This should never happen.");
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        }

        // threads are handled by a linked list.
        // Since the scheduler is locked and interrupts are disabled, to save time,
        // have a local array pointing to these threads instead of making new ones
        // and holding up the system
        while (t && i < sizeThreads) {
            threads[i] = t;
            t          = t->activeLink.next;
            i++;
        }
        __OSUnlockScheduler(curThread);
        OSRestoreInterrupts(state);

        if (!threads[0]) {
            DEBUG_FUNCTION_LINE_ERR("The first thread obtained was NULL. This should never happen.");
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        }

        for (int i = 0; i < sizeThreads; i++) {
            if (threads[i]) {
                if (threads[i]->name) {
                    WUPSConfigCategoryHandle catHandle;
                    WUPSConfigAPICreateCategoryOptionsV1 catOp = {.name = threads[i]->name};

                    if(WUPSConfigAPI_Category_Create(catOp, &catHandle) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to create category for thread %d", threads[i]->name);
                        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
                    }

                    char typeText[30];
                    if(threads[i]->type) {
                        snprintf(typeText, 30, "Type: %d", threads[i]->type);
                        if(WUPSConfigItemStub_AddToCategory(catHandle, typeText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                            DEBUG_FUNCTION_LINE_ERR("Failed to add type of thread %d to its category", threads[i]->type);
                        }
                    }

                    char stateText[30];
                    if(threads[i]->state) {
                        snprintf(stateText, 30, "State: %d", threads[i]->state);
                        if(WUPSConfigItemStub_AddToCategory(catHandle, stateText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                            DEBUG_FUNCTION_LINE_ERR("Failed to add state of thread %d to its category", threads[i]->state);
                        }
                    }

                    char attrText[30];
                    if(threads[i]->attr) {
                        snprintf(attrText, 30, "Attributes: %d", threads[i]->attr);
                        if(WUPSConfigItemStub_AddToCategory(catHandle, attrText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                            DEBUG_FUNCTION_LINE_ERR("Failed to add attributes of thread %d to its category", threads[i]->attr);
                        }
                    }

                    char idText[10];
                    snprintf(idText, 10, "ID: %d", threads[i]->id);
                    if(WUPSConfigItemStub_AddToCategory(catHandle, idText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add id of thread %d to its category", threads[i]->id);
                    }

                    if (WUPSConfigAPI_Category_AddCategory(root, catHandle) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add thread category %d to the list", threads[i]->name);
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
