#ifndef  _SYSTEM_EVENT_H_

#define _SYSTEM_EVENT_H_

enum {
    WT_ERROR_BAD_PARAMETER = 0,                    ///< 参数错误
    WT_ERROR_FEATURE_NOT_SUPPORTED,                ///< 不支持此操作
    WT_ERROR_UNKNOWN_ERROR,                        ///< 一般错误
    WT_ERROR_NO_MEMORY,                            ///<     无内存可用错误
    WT_ERROR_TIMEOUT,                              ///< 超时错误
    WT_ERROR_THREAD_BUSY                           ///< 任务在运行中
};

#endif//_SYSTEM_EVENT_H_
