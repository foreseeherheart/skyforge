#ifndef APPLICATION_ENTRY_H_
#define APPLICATION_ENTRY_H_

#include "hal_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 名称：application_entry
 * 作用：应用层入口函数，由 FSP 的 hal_entry 调用，负责初始化板级资源、传感器任务和边缘 AI 应用。
 * 接受值：无。
 * 返回值：无。
 */
void application_entry(void);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_ENTRY_H_ */