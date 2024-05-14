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
                    if (strcmp(threads[i]->name, "") == 0) {
                        threads[i]->name = "Unnamed Thread";
                    }
                    WUPSConfigAPICreateCategoryOptionsV1 catOp = {.name = threads[i]->name};

                    if (WUPSConfigAPI_Category_Create(catOp, &catHandle) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to create category for thread %d", threads[i]->name);
                        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
                    }

                    char typeText[30];
                    switch (threads[i]->type) {
                        case OS_THREAD_TYPE_DRIVER:
                            snprintf(typeText, 30, "Type: Driver");
                            break;
                        case OS_THREAD_TYPE_IO:
                            snprintf(typeText, 30, "Type: I/O");
                            break;
                        case OS_THREAD_TYPE_APP:
                            snprintf(typeText, 30, "Type: App");
                            break;
                        default:
                            snprintf(typeText, 30, "Type Undefined: %d", threads[i]->type);
                            break;
                    }
                    if (WUPSConfigItemStub_AddToCategory(catHandle, typeText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add type of thread %d to its category", threads[i]->name);
                    }


                    char attrText[30];
                    switch (threads[i]->attr) {
                        case OS_THREAD_ATTRIB_AFFINITY_CPU0:
                            snprintf(attrText, 30, "Attribute: Affinity CPU 0");
                            break;
                        case OS_THREAD_ATTRIB_AFFINITY_CPU1:
                            snprintf(attrText, 30, "Attribute: Affinity CPU 1");
                            break;
                        case OS_THREAD_ATTRIB_AFFINITY_CPU2:
                            snprintf(attrText, 30, "Attribute: Affinity CPU 2");
                            break;
                        case OS_THREAD_ATTRIB_AFFINITY_ANY:
                            snprintf(attrText, 30, "Attribute: Affinity Any");
                            break;
                        case OS_THREAD_ATTRIB_DETACHED:
                            snprintf(attrText, 30, "Attribute: Detached");
                            break;
                        case OS_THREAD_ATTRIB_STACK_USAGE:
                            snprintf(attrText, 30, "Attribute: Stack Usage");
                            break;
                        case OS_THREAD_ATTRIB_UNKNOWN:
                            snprintf(attrText, 30, "Attribute: Unknown");
                            break;
                        default:
                            snprintf(attrText, 30, "Attribute Undefined: %d", threads[i]->attr);
                            break;
                    }
                    if (WUPSConfigItemStub_AddToCategory(catHandle, attrText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add attributes of thread %d to its category", threads[i]->name);
                    }

                    char stateText[50];
                    switch (threads[i]->state) {
                        case OS_THREAD_STATE_NONE:
                            snprintf(stateText, 50, "State: None");
                            break;
                        case OS_THREAD_STATE_READY:
                            snprintf(stateText, 50, "State: Ready");
                            break;
                        case OS_THREAD_STATE_RUNNING:
                            snprintf(stateText, 50, "State: Running");
                            break;
                        case OS_THREAD_STATE_WAITING:
                            snprintf(stateText, 50, "State: Waiting (Probably bc WUPS is active)");
                            break;
                        case OS_THREAD_STATE_MORIBUND:
                            snprintf(stateText, 50, "State: Moribund");
                            break;
                        default:
                            snprintf(stateText, 50, "State Undefined: %d", threads[i]->state);
                            break;
                    }
                    if (WUPSConfigItemStub_AddToCategory(catHandle, stateText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add state of thread %d to its category", threads[i]->name);
                    }

                    char priorityText[20];
                    snprintf(priorityText, 20, "Priority: %d", threads[i]->priority);
                    if (WUPSConfigItemStub_AddToCategory(catHandle, priorityText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add priority of thread %d to its category", threads[i]->name);
                    }

                    char basePriorityText[20];
                    snprintf(basePriorityText, 20, "Base Priority: %d", threads[i]->basePriority);
                    if (WUPSConfigItemStub_AddToCategory(catHandle, basePriorityText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add base priority of thread %d to its category", threads[i]->name);
                    }

                    // most threads probably have no tls sections so only show on ones have them
                    if (threads[i]->tlsSectionCount) {
                        char tlsSectionText[20];
                        snprintf(tlsSectionText, 20, "TLS Sections: %d", threads[i]->tlsSectionCount);
                        if (WUPSConfigItemStub_AddToCategory(catHandle, tlsSectionText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                            DEBUG_FUNCTION_LINE_ERR("Failed to add tls section count %d to its category", threads[i]->name);
                        }
                    }

                    char alarmCancelledText[30];
                    snprintf(alarmCancelledText, 30, "Alarm Cancelled: %s", threads[i]->alarmCancelled ? "True" : "False");
                    if (WUPSConfigItemStub_AddToCategory(catHandle, alarmCancelledText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add alarm cancelled value of thread %d to its category", threads[i]->name);
                    }

                    char exitValueText[20];
                    snprintf(exitValueText, 20, "Exit Value: %d", threads[i]->exitValue);
                    if (WUPSConfigItemStub_AddToCategory(catHandle, exitValueText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add exit value of thread %d to its category", threads[i]->name);
                    }

                    char wakeCountText[24];
                    snprintf(wakeCountText, 24, "Wake Count: %lld", threads[i]->wakeCount);
                    if (WUPSConfigItemStub_AddToCategory(catHandle, wakeCountText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add wake count of thread %d to its category", threads[i]->name);
                    }

                    char unk0x610Text[25];
                    snprintf(unk0x610Text, 25, "unk0x610: %lld", threads[i]->unk0x610);
                    if (WUPSConfigItemStub_AddToCategory(catHandle, unk0x610Text) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add unk0x610 value of thread %lld to its category", threads[i]->name);
                    }

                    char unk0x618Text[25];
                    snprintf(unk0x618Text, 25, "unk0x618: %lld", threads[i]->unk0x618);
                    if (WUPSConfigItemStub_AddToCategory(catHandle, unk0x618Text) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add unk0x618 value of thread %lld to its category", threads[i]->name);
                    }

                    char unk0x620Text[25];
                    snprintf(unk0x620Text, 25, "unk0x620: %lld", threads[i]->unk0x620);
                    if (WUPSConfigItemStub_AddToCategory(catHandle, unk0x620Text) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add unk0x620 value of thread %lld to its category", threads[i]->name);
                    }

                    char unk0x628Text[25];
                    snprintf(unk0x628Text, 25, "unk0x628: %lld", threads[i]->unk0x628);
                    if (WUPSConfigItemStub_AddToCategory(catHandle, unk0x628Text) != WUPSCONFIG_API_RESULT_SUCCESS) {
                        DEBUG_FUNCTION_LINE_ERR("Failed to add unk0x628 value of thread %lld to its category", threads[i]->name);
                    }

                    // ----
                    // mutexes
                    // ----
                    if (threads[i]->mutex) {
                        WUPSConfigCategoryHandle mutexCatHandle;
                        WUPSConfigAPICreateCategoryOptionsV1 mutexCatOp = {.name = "Mutexes"};

                        OSMutex *m = threads[i]->mutex;
                        WUPSConfigAPI_Category_Create(mutexCatOp, &mutexCatHandle);
                        while (m) {
                            if (m->name != NULL) {
                                WUPSConfigItemStub_AddToCategory(mutexCatHandle, "Unnamed Mutex");
                            } else {
                                WUPSConfigItemStub_AddToCategory(mutexCatHandle, m->name);
                            }
                            m = m->link.next;
                        }
                        WUPSConfigAPI_Category_AddCategory(catHandle, mutexCatHandle);
                    }
                    // ----
                    // fast mutexes
                    //
                    if (threads[i]->fastMutex) {
                        WUPSConfigCategoryHandle fastMutexCatHandle;
                        WUPSConfigAPICreateCategoryOptionsV1 fastMutexCatOp = {.name = "Fast Mutexes"};


                        OSFastMutex *fm = threads[i]->fastMutex;
                        WUPSConfigAPI_Category_Create(fastMutexCatOp, &fastMutexCatHandle);
                        while (fm) {
                            if (fm->name != NULL) {
                                WUPSConfigItemStub_AddToCategory(fastMutexCatHandle, "Unnamed Fast Mutex");
                            } else {
                                WUPSConfigItemStub_AddToCategory(fastMutexCatHandle, fm->name);
                            }
                            fm = fm->link.next;
                        }
                        WUPSConfigAPI_Category_AddCategory(catHandle, fastMutexCatHandle);
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
