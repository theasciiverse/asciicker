// QuickJS in place of V8 for the "\" console and the ak scripting API.
// game_app.cpp keeps its V8 code; built with AV_QUICKJS it leaves it out
// and these four take the place of its init_v8(), free_v8(), akAPI_Exec()
// and akAPI_CB(). Printing goes to stderr, since stdout is the screen.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../game_api.h"
#include "quickjs/quickjs.h"

static JSRuntime* rt = 0;
static JSContext* ctx = 0;

static void report_exception()
{
    JSValue exc = JS_GetException(ctx);
    const char* str = JS_ToCString(ctx, exc);
    fprintf(stderr, "Exception: %s\n", str ? str : "?");
    if (str)
        JS_FreeCString(ctx, str);
    JS_FreeValue(ctx, exc);
}

static JSValue js_akPrint(JSContext* ctx, JSValueConst, int argc, JSValueConst* argv)
{
    for (int i = 0; i < argc; i++)
    {
        const char* str = JS_ToCString(ctx, argv[i]);
        fprintf(stderr, i ? " %s" : "%s", str ? str : "<string conversion failed>");
        if (str)
            JS_FreeCString(ctx, str);
    }
    fprintf(stderr, "\n");
    return JS_UNDEFINED;
}

static JSValue js_akAPI_Call(JSContext* ctx, JSValueConst, int argc, JSValueConst* argv)
{
    int id;
    if (argc != 1 || !JS_IsNumber(argv[0]) || JS_ToInt32(ctx, &id, argv[0]))
    {
        fprintf(stderr, "akAPI_Call problem\n");
        return JS_UNDEFINED;
    }
    akAPI_Call(id);
    return JS_UNDEFINED;
}

void init_v8()
{
    rt = JS_NewRuntime();
    ctx = JS_NewContext(rt);

    akAPI_Buff = calloc(1, AKAPI_BUF_SIZE);

    JSValue global = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global, "akPrint", JS_NewCFunction(ctx, js_akPrint, "akPrint", 1));
    JS_SetPropertyStr(ctx, global, "akAPI_Call", JS_NewCFunction(ctx, js_akAPI_Call, "akAPI_Call", 1));
    // the buffer stays ours: no realloc function, freed in free_v8()
    JS_SetPropertyStr(ctx, global, "akAPI_V8AB",
        JS_NewArrayBuffer(ctx, (uint8_t*)akAPI_Buff, AKAPI_BUF_SIZE, 0, 0, 0, false));
    JS_FreeValue(ctx, global);
}

void free_v8()
{
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
    ctx = 0;
    rt = 0;
    free(akAPI_Buff);
    akAPI_Buff = 0;
}

void akAPI_Exec(const char* str, int len, bool root)
{
    if (!ctx)
        return;
    if (len < 0)
        len = strlen(str);

    // as game_app.cpp: user code runs in a function that sees only ak and akPrint
    static const char prefix[] = "(function(ak,akPrint){";
    static const char suffix[] = "}.apply(akAPI_This,[ak,akPrint]))";
    const char* pre = root ? "" : prefix;
    const char* suf = root ? "" : suffix;
    size_t pre_len = strlen(pre), suf_len = strlen(suf);

    // JS_Eval wants the source zero-terminated; the talk box is not
    char* buf = (char*)malloc(pre_len + len + suf_len + 1);
    memcpy(buf, pre, pre_len);
    memcpy(buf + pre_len, str, len);
    memcpy(buf + pre_len + len, suf, suf_len + 1);

    JSValue result = JS_Eval(ctx, buf, pre_len + len + suf_len, "<ak>", JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(result))
        report_exception();
    JS_FreeValue(ctx, result);
    free(buf);
}

void akAPI_CB(int id)
{
    if (!ctx)
        return;
    JSValue global = JS_GetGlobalObject(ctx);
    JSValue cb = JS_GetPropertyStr(ctx, global, "akAPI_CB");
    JSValue arg = JS_NewInt32(ctx, id);
    JSValue result = JS_Call(ctx, cb, global, 1, &arg);
    if (JS_IsException(result))
        report_exception();
    JS_FreeValue(ctx, result);
    JS_FreeValue(ctx, cb);
    JS_FreeValue(ctx, global);
}
