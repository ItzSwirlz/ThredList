#include "threadcategory.h"

WUPSConfigAPICallbackStatus ThredListCreateThreadCategory(WUPSConfigCategoryHandle rootHandle, OSThread *thread) {
    if (thread) {
        if (thread->name) {
            WUPSConfigCategoryHandle catHandle;
            if (strcmp(thread->name, "") == 0) {
                thread->name = "Unnamed Thread";
            }
            WUPSConfigAPICreateCategoryOptionsV1 catOp = {.name = thread->name};

            if (WUPSConfigAPI_Category_Create(catOp, &catHandle) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to create category for thread %d", thread->name);
                return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
            }

            char typeText[20];
            switch (thread->type) {
                case OS_THREAD_TYPE_DRIVER:
                    snprintf(typeText, 20, "Type: Driver");
                    break;
                case OS_THREAD_TYPE_IO:
                    snprintf(typeText, 20, "Type: I/O");
                    break;
                case OS_THREAD_TYPE_APP:
                    snprintf(typeText, 20, "Type: App");
                    break;
                default:
                    snprintf(typeText, 20, "Type Undefined: %d", thread->type);
                    break;
            }
            if (WUPSConfigItemStub_AddToCategory(catHandle, typeText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add type of thread %d to its category", thread->name);
            }


            char attrText[26];
            switch (thread->attr) {
                case OS_THREAD_ATTRIB_AFFINITY_CPU0:
                    snprintf(attrText, 26, "Attribute: Affinity CPU 0");
                    break;
                case OS_THREAD_ATTRIB_AFFINITY_CPU1:
                    snprintf(attrText, 26, "Attribute: Affinity CPU 1");
                    break;
                case OS_THREAD_ATTRIB_AFFINITY_CPU2:
                    snprintf(attrText, 26, "Attribute: Affinity CPU 2");
                    break;
                case OS_THREAD_ATTRIB_AFFINITY_ANY:
                    snprintf(attrText, 26, "Attribute: Affinity Any");
                    break;
                case OS_THREAD_ATTRIB_DETACHED:
                    snprintf(attrText, 26, "Attribute: Detached");
                    break;
                case OS_THREAD_ATTRIB_STACK_USAGE:
                    snprintf(attrText, 26, "Attribute: Stack Usage");
                    break;
                case OS_THREAD_ATTRIB_UNKNOWN:
                    snprintf(attrText, 26, "Attribute: Unknown");
                    break;
                default:
                    snprintf(attrText, 26, "Attribute Undefined: %d", thread->attr);
                    break;
            }
            if (WUPSConfigItemStub_AddToCategory(catHandle, attrText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add attributes of thread %d to its category", thread->name);
            }

            char stateText[35];
            switch (thread->state) {
                case OS_THREAD_STATE_NONE:
                    snprintf(stateText, 35, "State: None");
                    break;
                case OS_THREAD_STATE_READY:
                    snprintf(stateText, 35, "State: Ready");
                    break;
                case OS_THREAD_STATE_RUNNING:
                    snprintf(stateText, 35, "State: Running");
                    break;
                case OS_THREAD_STATE_WAITING:
                    snprintf(stateText, 35, "State: Waiting (this is expected)");
                    break;
                case OS_THREAD_STATE_MORIBUND:
                    snprintf(stateText, 35, "State: Moribund");
                    break;
                default:
                    snprintf(stateText, 35, "State Undefined: %d", thread->state);
                    break;
            }
            if (WUPSConfigItemStub_AddToCategory(catHandle, stateText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add state of thread %d to its category", thread->name);
            }

            char priorityText[15];
            snprintf(priorityText, 15, "Priority: %d", thread->priority);
            if (WUPSConfigItemStub_AddToCategory(catHandle, priorityText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add priority of thread %d to its category", thread->name);
            }

            char basePriorityText[20];
            snprintf(basePriorityText, 20, "Base Priority: %d", thread->basePriority);
            if (WUPSConfigItemStub_AddToCategory(catHandle, basePriorityText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add base priority of thread %d to its category", thread->name);
            }

            char wakeCountText[24];
            snprintf(wakeCountText, 24, "Wake Count: %lld", thread->wakeCount);
            if (WUPSConfigItemStub_AddToCategory(catHandle, wakeCountText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add wake count of thread %d to its category", thread->name);
            }

            char suspendCountText[24];
            snprintf(suspendCountText, 24, "Suspend Count: %d", thread->suspendCounter);
            if (WUPSConfigItemStub_AddToCategory(catHandle, suspendCountText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add suspend count of thread %d to its category", thread->name);
            }

            char alarmCancelledText[25];
            snprintf(alarmCancelledText, 25, "Alarm Cancelled: %s", thread->alarmCancelled ? "True" : "False");
            if (WUPSConfigItemStub_AddToCategory(catHandle, alarmCancelledText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add alarm cancelled value of thread %d to its category", thread->name);
            }

            char exitValueText[16];
            snprintf(exitValueText, 16, "Exit Value: %d", thread->exitValue);
            if (WUPSConfigItemStub_AddToCategory(catHandle, exitValueText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add exit value of thread %d to its category", thread->name);
            }

            char suspendResultText[22];
            snprintf(suspendResultText, 22, "Suspend Result: %d", thread->suspendResult);
            if (WUPSConfigItemStub_AddToCategory(catHandle, suspendResultText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add suspend result of thread %d to its category", thread->name);
            }

            char unk0x610Text[25];
            snprintf(unk0x610Text, 25, "unk0x610: %lld", thread->unk0x610);
            if (WUPSConfigItemStub_AddToCategory(catHandle, unk0x610Text) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add unk0x610 value of thread %lld to its category", thread->name);
            }

            char unk0x618Text[25];
            snprintf(unk0x618Text, 25, "unk0x618: %lld", thread->unk0x618);
            if (WUPSConfigItemStub_AddToCategory(catHandle, unk0x618Text) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add unk0x618 value of thread %lld to its category", thread->name);
            }

            char unk0x620Text[25];
            snprintf(unk0x620Text, 25, "unk0x620: %lld", thread->unk0x620);
            if (WUPSConfigItemStub_AddToCategory(catHandle, unk0x620Text) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add unk0x620 value of thread %lld to its category", thread->name);
            }

            char unk0x628Text[25];
            snprintf(unk0x628Text, 25, "unk0x628: %lld", thread->unk0x628);
            if (WUPSConfigItemStub_AddToCategory(catHandle, unk0x628Text) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add unk0x628 value of thread %lld to its category", thread->name);
            }

            // most threads probably have no tls sections so only show on ones have them
            if (thread->tlsSectionCount) {
                char tlsSectionText[20];
                snprintf(tlsSectionText, 20, "TLS Sections: %d", thread->tlsSectionCount);
                if (WUPSConfigItemStub_AddToCategory(catHandle, tlsSectionText) != WUPSCONFIG_API_RESULT_SUCCESS) {
                    DEBUG_FUNCTION_LINE_ERR("Failed to add tls section count %d to its category", thread->name);
                }
            }

            // ----
            // mutexes
            // ----
            if (thread->mutex != NULL || thread->mutexQueue.head != NULL) {
                WUPSConfigCategoryHandle mutexCatHandle;
                WUPSConfigAPICreateCategoryOptionsV1 mutexCatOp = {.name = "Mutexes"};


                OSMutex *m;
                if(thread->mutex != NULL) {
                    m = thread->mutex;
                } else {
                    m = thread->mutexQueue.head;
                }
                WUPSConfigAPI_Category_Create(mutexCatOp, &mutexCatHandle);
                while (m) {
                    if (m->name != NULL) {
                        WUPSConfigItemStub_AddToCategory(mutexCatHandle, m->name);
                    } else {
                        WUPSConfigItemStub_AddToCategory(mutexCatHandle, "Unnamed Mutex");
                    }
                    m = m->link.next;
                }
                WUPSConfigAPI_Category_AddCategory(catHandle, mutexCatHandle);
            }
            // ----
            // fast mutexes
            //
            if (thread->fastMutex != NULL || thread->fastMutexQueue.head != NULL) {
                WUPSConfigCategoryHandle fastMutexCatHandle;
                WUPSConfigAPICreateCategoryOptionsV1 fastMutexCatOp = {.name = "Fast Mutexes"};


                OSFastMutex *fm;
                if(thread->fastMutex != NULL) {
                    fm = thread->fastMutex;
                } else {
                    fm = thread->fastMutexQueue.head;
                }
                WUPSConfigAPI_Category_Create(fastMutexCatOp, &fastMutexCatHandle);
                while (fm) {
                    if (fm->name != NULL) {
                        WUPSConfigItemStub_AddToCategory(fastMutexCatHandle, fm->name);
                    } else {
                        WUPSConfigItemStub_AddToCategory(fastMutexCatHandle, "Unnamed Fast Mutex");
                    }
                    fm = fm->link.next;
                }
                WUPSConfigAPI_Category_AddCategory(catHandle, fastMutexCatHandle);
            }

            if (WUPSConfigAPI_Category_AddCategory(rootHandle, catHandle) != WUPSCONFIG_API_RESULT_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to add thread category %d to the list", thread->name);
                return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
            }
        }
    }
    return WUPSCONFIG_API_RESULT_SUCCESS;
}
