#include "beatTask.h"
#if btask_compile_en

static btask_datas_t *list_head = NULL;
static btask_datas_t *list_tail = NULL;
static btask_cb_t delay_cb      = NULL;
static btask_event_t delay_userdata;
static int32_t btask_delay_t          = 0;
static uint32_t (*_timer_get_count)() = NULL;
static btask_timer_count_mode _count_mode;
static uint32_t tic_count    = 0;
static int tic_flg           = 0;
static uint32_t pass_time_ms = 0;

#if btask_support_rtos
static btask_mutex_cb_t mutex;
static uint8_t del_get_key_flg = 0;
void btask_mutex_register(btask_mutex_cb_t *cb)
{
    mutex = *cb;
}
#endif

static btask_datas_t *_find_task_handle(const char *name)
{
    btask_datas_t *move = list_head;
    if (list_head == NULL)
        return NULL;
    for (;;) {
        if (strcmp(move->name, name) == 0)
            return move;
        if (move->next == NULL)
            return NULL;
        move = move->next;
    }
}

void btask_set_timer_count_cb(uint32_t (*timer_get_count)(), btask_timer_count_mode count_mode)
{
    _timer_get_count = timer_get_count;
    _count_mode      = count_mode;
}

void btask_del_timer_count_cb()
{
    _timer_get_count = NULL;
}

void btask_set_delay_cb(btask_cb_t cb, void *userdata)
{
    delay_cb                = cb;
    delay_userdata.handle   = NULL;
    delay_userdata.userdata = userdata;
}

void btask_del_delay_cb()
{
    delay_cb = NULL;
}

btask_handle_t btask_creat_ms(uint32_t million, btask_cb_t cb, int16_t count, const char *name, void *userdata)
{

    btask_datas_t *tmp = btask_malloc(sizeof(btask_datas_t));

    if (tmp == NULL)
        return NULL;

#if btask_support_rtos
    mutex.mutex_get_cb();
#endif

    if (list_tail != NULL) {
        list_tail->next = tmp;
        tmp->last       = list_tail;
    }
    if (list_head == NULL)
        list_head = tmp;

    list_tail                  = tmp;
    tmp->_busy                 = 0;
    tmp->name                  = name;
    tmp->next                  = NULL;
    tmp->del_cb                = NULL;
    tmp->tick_p                = cb;                // 注册回调函数
    tmp->tick_t_count          = 0;                 // 更新计数
    tmp->tick_t_count_max      = million;           // 更新定时
    tmp->tick_t_mode_flg       = count;             // 更新次数
    tmp->userdata.handle       = (const void *)tmp; // 获取句柄地址
    tmp->userdata.userdata     = userdata;          // 更新用户指针
    tmp->del_userdata.handle   = tmp->userdata.handle;
    tmp->del_userdata.userdata = NULL;

#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
    return tmp->userdata.handle; // 返回句柄
}

btask_handle_t btask_creat(uint16_t second, btask_cb_t cb, int16_t count, const char *name, void *userdata)
{
    return btask_creat_ms((uint32_t)second * 1000, cb, count, name, userdata);
}

void btask_set_del_cb(btask_handle_t handle, const char *name, btask_cb_t cb, void *userdata)
{
    btask_datas_t *tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t *)handle;
    tmp->del_cb                = cb;
    tmp->del_userdata.userdata = userdata;
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
}

uint8_t btask_has_task(btask_handle_t handle, const char *name)
{
    if (list_head == NULL)
        return 0;
    if (handle == NULL) {
        if (_find_task_handle(name) == NULL)
            return 0;
        else
            return 1;
    } else {
        btask_datas_t *move = list_head;
        btask_datas_t *tmp  = (btask_datas_t *)handle;
        for (;;) {
            if (move == tmp)
                return 1;
            if (move->next == NULL)
                return 0;
            move = move->next;
        }
    }
}

void btask_del(btask_handle_t handle, const char *name)
{
    btask_datas_t *tmp;
    btask_datas_t *move;

#if btask_support_rtos
    if (del_get_key_flg == 0)
        mutex.mutex_get_cb();
#endif

    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            if (del_get_key_flg == 0)
                mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t *)handle;

    if (tmp->del_cb != NULL)
        tmp->del_cb(&tmp->del_userdata);

    if (tmp == list_head) {
        if (list_head->next == NULL) {
            move      = NULL;
            list_tail = NULL;
        } else {
            move       = list_head->next;
            move->last = NULL;
        }

        btask_free(list_head);
        list_head = move;
    } else if (tmp == list_tail) {
        if (list_tail->last == NULL) {
            move      = NULL;
            list_head = NULL;
        } else {
            move       = list_tail->last;
            move->next = NULL;
        }
        btask_free(list_tail);
        list_tail = move;
    } else {
        btask_datas_t *_next;
        move        = tmp->last;
        _next       = tmp->next;
        move->next  = tmp->next;
        _next->last = move;
        btask_free(tmp);
    }
#if btask_support_rtos
    if (del_get_key_flg == 0)
        mutex.mutex_give_cb();
#endif
}

/**
 * @brief 暂停任务
 *
 * @param handle 册时返回的句柄
 */
void btask_pause(btask_handle_t handle, const char *name)
{
    btask_datas_t *tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t *)handle;
    if (tmp->tick_t_mode_flg < 0) // 已经暂停
    {
#if btask_support_rtos
        mutex.mutex_give_cb();
#endif
        return;
    }
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
    tmp->tick_t_mode_flg = -tmp->tick_t_mode_flg - 1; //-1 -- -32768
}

/**
 * @brief 恢复任务
 *
 * @param handle 册时返回的句柄
 */
void btask_resume(btask_handle_t handle, const char *name)
{
    btask_datas_t *tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t *)handle;
    if (tmp->tick_t_mode_flg >= 0) // 已经恢复
    {
#if btask_support_rtos
        mutex.mutex_give_cb();
#endif
        return;
    }
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
    tmp->tick_t_mode_flg = -tmp->tick_t_mode_flg - 1; //-1 -- -32768
}

void btask_ready(btask_handle_t handle, const char *name, uint8_t clr_count)
{
    btask_datas_t *tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t *)handle;

    if (clr_count == 0)
        tmp->tick_t_count += tmp->tick_t_count_max;
    else
        tmp->tick_t_count = tmp->tick_t_count_max;

#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
}

/**
 * @brief 重置任务计时
 *
 * @param handle 册时返回的句柄
 */
void btask_reset(btask_handle_t handle, const char *name)
{
    btask_datas_t *tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t *)handle;
    tmp->tick_t_count = 0;
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
}

void btask_set_time(btask_handle_t handle, const char *name, uint32_t ms)
{
    btask_datas_t *tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t *)handle;
    tmp->tick_t_count     = 0;
    tmp->tick_t_count_max = ms;
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
}

void btask_set_count(btask_handle_t handle, const char *name, int16_t count)
{
    btask_datas_t *tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return;
        }
    } else
        tmp = (btask_datas_t *)handle;
    if (count < 0) {
#if btask_support_rtos
        mutex.mutex_give_cb();
#endif
        return;
    }
    tmp->tick_t_mode_flg = count;
#if btask_support_rtos
    mutex.mutex_give_cb();
#endif
}

int16_t btask_get_count(btask_handle_t handle, const char *name)
{
    btask_datas_t *tmp;
#if btask_support_rtos
    mutex.mutex_get_cb();
#endif
    if (handle == NULL) {
        tmp = _find_task_handle(name);
        if (tmp == NULL) {
#if btask_support_rtos
            mutex.mutex_give_cb();
#endif
            return -1;
        }
    } else
        tmp = (btask_datas_t *)handle;

#if btask_support_rtos
    mutex.mutex_give_cb();
#endif

    return tmp->tick_t_mode_flg;
}

void btask_tick_inc(int _ms)
{
    tic_count += _ms;
    pass_time_ms += _ms;
    tic_flg = 1;
    btask_delay_t -= _ms;
}

void btask_handler()
{
    uint32_t _tic_count;

    btask_datas_t *node = list_head;
    btask_datas_t *next = NULL;

    if (tic_flg == 0) {
        return;
    }

    _tic_count = tic_count;
    tic_count  = 0;
    tic_flg    = 0;

    if (list_head == NULL) {
        return;
    }

#if btask_support_rtos
    mutex.mutex_get_cb();
    del_get_key_flg = 1;
#endif

    for (;;) {

        node->tick_t_count += _tic_count;
        // 注册了
        if (node->tick_t_count < node->tick_t_count_max) // 定时未到
            goto chek_null;

        node->tick_t_count -= node->tick_t_count_max; // 定时到了减去一次计时

        if (node->_busy != 1) // 防止无限递归
        {
            node->_busy = 1;
            node->tick_p(&node->userdata); // 调用回调
            node->_busy = 0;
        }

        if (node->tick_t_mode_flg == btask_infinite)
            goto chek_null;

        // 不是无限次执行
        node->tick_t_mode_flg--; // 次数减1
        if (node->tick_t_mode_flg != 0)
            goto chek_null;

        if (node->next == NULL) {

            btask_del((btask_handle_t)node, NULL);
            break;
        }

        next = node->next;

        btask_del((btask_handle_t)node, NULL);

    chek_null:

        if (next != NULL) {
            node = next;
            next = NULL;
            continue;
        }

        if (node->next == NULL)
            break;
        node = node->next;
    }
#if btask_support_rtos
    mutex.mutex_give_cb();
    del_get_key_flg = 0;
#endif
}

void btask_delay(uint32_t ms)
{
    uint32_t timer_count = 0;
    if (_timer_get_count != NULL)
        timer_count = _timer_get_count();
    btask_delay_t = ms;
    while (btask_delay_t > 0) {
        btask_handler();
        if (delay_cb != NULL)
            delay_cb(&delay_userdata);
    }
    if (_timer_get_count == NULL)
        return;
    if (_count_mode == count_down) {
        while (_timer_get_count() > timer_count) {
            btask_handler();
            if (delay_cb != NULL)
                delay_cb(&delay_userdata);
        }
        return;
    }
    while (_timer_get_count() < timer_count) {
        btask_handler();
        if (delay_cb != NULL)
            delay_cb(&delay_userdata);
    }
}

void btask_delay_without_task(uint32_t ms)
{
    uint32_t timer_count = 0;
    if (_timer_get_count != NULL)
        timer_count = _timer_get_count();
    btask_delay_t = ms;
    while (btask_delay_t > 0) {
        if (delay_cb != NULL)
            delay_cb(&delay_userdata);
    }
    if (_timer_get_count == NULL)
        return;
    if (_count_mode == count_down) {
        while (_timer_get_count() > timer_count) {
            if (delay_cb != NULL)
                delay_cb(&delay_userdata);
        }
        return;
    }
    while (_timer_get_count() < timer_count) {
        if (delay_cb != NULL)
            delay_cb(&delay_userdata);
    }
}

uint32_t btask_get_time_ms()
{
    return pass_time_ms;
}

#endif
