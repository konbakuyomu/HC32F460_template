#include "app_manage.h"

typedef struct
{
    void (*app_kill)();
    void (*app_load)();
    void (*app_close)();
    void (*app_power_off)();
} app_func_t;

typedef struct _app_list_t {
    app_func_t func;
    app_obj_t obj;
    app_handle_t handle;
    struct _app_list_t *next;
} app_list_t;

static app_list_t *list_head = NULL;
static app_list_t *list_tail = NULL;
static int32_t app_cnt       = 0;
static app_obj_t *app_loaded = NULL;

static int _strcmp(const char *str1, const char *str2)
{
    for (; *str1 == *str2; ++str1, ++str2) {
        if (*str1 == '\0')
            return (0);
    }
    return 1;
}

static void app_list_add(app_list_t *app)
{
    app->next = NULL;
    if (list_head == NULL) {
        list_head = app;
    } else {
        list_tail->next = app;
    }
    list_tail = app;
}

static app_list_t *app_list_find(const char *name)
{
    app_list_t *move = list_head;
    while (move != NULL) {
        if (_strcmp(name, move->obj.name) == 0)
            return move;
        move = move->next;
    }
    return NULL;
}

static app_list_t *app_list_find_id(app_handle_t handle)
{
    if (handle >= app_cnt)
        return NULL;
    app_list_t *move = list_head;
    for (app_handle_t i = 0; i < handle; i++)
        move = move->next;
    return move;
}

app_handle_t app_install(app_config_t *cfg)
{
    app_list_t *app = malloc(sizeof(app_list_t));
    if (app == NULL)
        return app_none;

    app->func.app_load  = cfg->app_load;
    app->func.app_close = cfg->app_close;
    app->func.app_kill  = cfg->app_kill;

    app->obj.name                 = cfg->name;
    app->obj.name_font            = cfg->name_font;
    app->obj.icon                 = cfg->icon;
    *(uint8_t *)&app->obj.has_gui = cfg->has_gui;

    app->handle = app_cnt;
    app_cnt++;
    app_list_add(app);

    if (cfg->app_init != NULL)
        cfg->app_init();

    return (app_cnt - 1);
}

uint8_t app_load(const char *name, app_handle_t handle)
{

    app_list_t *tmp;
    if (name == NULL && handle == app_none)
        return 0;
    if (name == NULL) {
        tmp = app_list_find_id(handle);
    } else {
        tmp = app_list_find(name);
    }
    if (tmp == NULL) {
        return 0;
    }

    if (app_loaded != NULL) {

        app_close(app_loaded->name, app_none);
    }

    app_loaded = &tmp->obj;

    if (tmp->func.app_load != NULL) {
        tmp->func.app_load();
    }

    return 1;
}

uint8_t app_close(const char *name, app_handle_t handle)
{
    app_list_t *tmp;
    if (name == NULL && handle == app_none)
        return 0;
    if (name == NULL)
        tmp = app_list_find_id(handle);
    else
        tmp = app_list_find(name);
    if (tmp == NULL)
        return 0;
    if (tmp->func.app_close != NULL)
        tmp->func.app_close();
    app_loaded = NULL;
    return 1;
}

uint8_t app_kill(const char *name, app_handle_t handle)
{
    app_list_t *tmp;
    if (name == NULL && handle == app_none)
        return 0;
    if (name == NULL)
        tmp = app_list_find_id(handle);
    else
        tmp = app_list_find(name);
    if (tmp == NULL)
        return 0;
    if (tmp->func.app_kill != NULL)
        tmp->func.app_kill();
    return 1;
}

void app_kill_all()
{
    app_list_t *tmp = list_head;
    for (app_handle_t i = 0; i < app_cnt; i++) {
        if (tmp->func.app_kill != NULL)
            tmp->func.app_kill();
        tmp = tmp->next;
    }
}

void app_power_off_all()
{
    app_list_t *tmp = list_head;
    for (app_handle_t i = 0; i < app_cnt; i++) {
        if (tmp->func.app_power_off != NULL)
            tmp->func.app_power_off();
        tmp = tmp->next;
    }
}

app_handle_t app_get_handle(const char *name)
{
    app_list_t *tmp = app_list_find(name);
    if (tmp == NULL)
        return (app_none);
    else
        return (tmp->handle);
}

const char *app_get_name(app_handle_t handle)
{
    app_list_t *tmp = app_list_find_id(handle);
    if (tmp == NULL)
        return (NULL);
    else
        return (tmp->obj.name);
}

int32_t app_get_cnt()
{
    return app_cnt;
}

app_obj_t *app_get(const char *name, app_handle_t handle)
{
    if (name == NULL)
        return (&app_list_find_id(handle)->obj);
    return (&app_list_find(name)->obj);
}

app_obj_t *app_get_loaded()
{
    return app_loaded;
}
