#include "utils/logger.h"
#include <coreinit/interrupts.h>
#include <coreinit/scheduler.h>
#include <coreinit/thread.h>
#include <string.h>
#include <stdio.h>
#include <wups.h>
#include <wups/config/WUPSConfigItemStub.h>

using namespace std;

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

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle rootHandle) {
    {
        WUPSConfigCategory root = WUPSConfigCategory(rootHandle);
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
                    WUPSConfigCategory threadCat = WUPSConfigCategory(catHandle);
                    WUPSConfigAPICreateCategoryOptionsV1 catOp = {.name = threads[i]->name};

                    if (WUPSConfigAPI_Category_Create(catOp, &catHandle) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to create category for thread %d", threads[i]->name);
                        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
                    }

                    string typeText = "Type: ";
                    switch (threads[i]->type) {
                        case OS_THREAD_TYPE_DRIVER:
                            typeText += "Driver";
                            break;
                        case OS_THREAD_TYPE_IO:
                            typeText += "I/O";
                            break;
                        case OS_THREAD_TYPE_APP:
                            typeText += "App";
                            break;
                        default:
                            typeText += "Undefined: " + threads[i]->type;
                            break;
                    }
                    threadCat.add(WUPSConfigItemStub::Create(typeText));


                    string attrText = "Attribute: ";
                    switch (threads[i]->attr) {
                        case OS_THREAD_ATTRIB_AFFINITY_CPU0:
                            attrText += "Affinity CPU 0";
                            break;
                        case OS_THREAD_ATTRIB_AFFINITY_CPU1:
                            attrText += "Affinity CPU 1";
                            break;
                        case OS_THREAD_ATTRIB_AFFINITY_CPU2:
                            attrText += "Affinity CPU 2";
                            break;
                        case OS_THREAD_ATTRIB_AFFINITY_ANY:
                            attrText += "Affinity Any";
                            break;
                        case OS_THREAD_ATTRIB_DETACHED:
                            attrText += "Detached";
                            break;
                        case OS_THREAD_ATTRIB_STACK_USAGE:
                            attrText += "Stack Usage";
                            break;
                        case OS_THREAD_ATTRIB_UNKNOWN:
                            attrText += "Unknown";
                            break;
                        default:
                            attrText += "Undefined: " + threads[i]->attr;
                            break;
                    }
                    threadCat.add(WUPSConfigItemStub::Create(attrText));

                    string stateText = "State: ";
                    switch (threads[i]->state) {
                        case OS_THREAD_STATE_NONE:
                            stateText += "None";
                            break;
                        case OS_THREAD_STATE_READY:
                            stateText += "Ready";
                            break;
                        case OS_THREAD_STATE_RUNNING:
                            stateText += "Running";
                            break;
                        case OS_THREAD_STATE_WAITING:
                            stateText += "Waiting (Likely because WUPS is active)";
                            break;
                        case OS_THREAD_STATE_MORIBUND:
                            stateText += "Moribund";
                            break;
                        default:
                            stateText += "Undefined: " + threads[i]->state;
                            break;
                    }
                    threadCat.add(WUPSConfigItemStub::Create(stateText));

                    threadCat.add(WUPSConfigItemStub::Create("Priority: " + threads[i]->priority));
                    threadCat.add(WUPSConfigItemStub::Create("Base Priority: " + threads[i]->basePriority));

                    // most threads probably have no tls sections so only show on ones have them
                    if (threads[i]->tlsSectionCount) {
                        threadCat.add(WUPSConfigItemStub::Create("TLS Sections: " + threads[i]->tlsSectionCount));
                    }

                    threadCat.add(WUPSConfigItemStub::Create("Exit Value: " + threads[i]->exitValue));

                    threadCat.add(WUPSConfigItemStub::Create("OSTimer unk0x610: " + threads[i]->unk0x610));
                    threadCat.add(WUPSConfigItemStub::Create("OSTimer unk0x618: " + threads[i]->unk0x618));
                    threadCat.add(WUPSConfigItemStub::Create("OSTimer unk0x620: " + threads[i]->unk0x620));
                    threadCat.add(WUPSConfigItemStub::Create("OSTimer unk0x628: " + threads[i]->unk0x628));

                    if (WUPSConfigAPI_Category_AddCategory(rootHandle, catHandle) != WUPSCONFIG_API_RESULT_SUCCESS) {
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
