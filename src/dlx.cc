#include <node_api.h>
#include "dlx.h"
#include <string>
using namespace std;


napi_value methodSolutionCount (napi_env env, napi_callback_info info) {
    napi_status status;
    size_t argc=1;
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, 0, 0);
    if( status != napi_ok) {
        napi_throw_type_error(env, "111", "Wrong number of arguments");
        return 0; // napi_value 实际上是一个指针，返回空指针表示无返回值
    }
    DLXSolver dlx;
    size_t len;
    char pszstr[100]="";
    status = napi_get_value_string_latin1(env, argv[0], pszstr, 100, &len);
    if( status != napi_ok) {
        napi_throw_type_error(env, "112", "Can't read arguments");
        return 0; // napi_value 实际上是一个指针，返回空指针表示无返回值
    }
    int cnt = dlx.solution_count(pszstr);

    napi_value retcnt;
    status = napi_create_int64(env, (size_t)cnt, &retcnt);
    if( status != napi_ok) {
        napi_throw_type_error(env, "113", "set cnt error");
        return 0; // napi_value 实际上是一个指针，返回空指针表示无返回值
    }
    return retcnt;
}

napi_value methodSolve (napi_env env, napi_callback_info info) {
    napi_status status;
    size_t argc=1;
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, 0, 0);
    if( status != napi_ok) {
        napi_throw_type_error(env, "111", "Wrong number of arguments");
        return 0; // napi_value 实际上是一个指针，返回空指针表示无返回值
    }
    DLXSolver dlx;
    size_t len;
    char pszstr[100]="";
    status = napi_get_value_string_latin1(env, argv[0], pszstr, 100, &len);
    if( status != napi_ok) {
        napi_throw_type_error(env, "112", "Can't read arguments");
        return 0; // napi_value 实际上是一个指针，返回空指针表示无返回值
    }
    int cnt=0;
    string res = dlx.solution(pszstr, &cnt);

    // printf("try to solve:%s\ncnt:%d solution:%s\n", pszstr, cnt, res.c_str());
    napi_value retval;
    status = napi_create_string_utf8(env, res.c_str(), strlen(res.c_str()), &retval);
    napi_value retcnt;
    status = napi_create_int64(env, (size_t)cnt, &retcnt);

    napi_value obj;
    status = napi_create_object(env, &obj);
    if (status != napi_ok) return NULL;

    status =napi_set_named_property(env, obj, "cnt", retcnt);
    status =napi_set_named_property(env, obj, "solution", retval);
    return obj;
}

napi_value methodGenerate (napi_env env, napi_callback_info info) {
    napi_status status;
    DLXSolver dlx;
    string strq = dlx.generate();
    int cnt=0;
    string stra = dlx.solution(strq.c_str(), &cnt);
    napi_value vq;
    napi_value va;
    status = napi_create_string_utf8(env, strq.c_str(), strlen(strq.c_str()), &vq);
    status = napi_create_string_utf8(env, stra.c_str(), strlen(stra.c_str()), &va);
    napi_value obj;
    status = napi_create_object(env, &obj);
    if (status != napi_ok) return NULL;
    status =napi_set_named_property(env, obj, "puzzle", vq);
    status =napi_set_named_property(env, obj, "solution", va);
    return obj;
}

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  // https://nodejs.org/docs/latest-v8.x/api/n-api.html#n_api_napi_property_descriptor
  napi_property_descriptor descSolve = { "solve", 0, methodSolve, 0, 0, 0, napi_default, 0 };
  napi_property_descriptor descSolutionCount = { "solutioncount", 0, methodSolutionCount, 0, 0, 0, napi_default, 0 };
  napi_property_descriptor descGenerate = { "generate", 0, methodGenerate, 0, 0, 0, napi_default, 0 };

  status = napi_define_properties(env, exports, 1, &descSolve);
  status = napi_define_properties(env, exports, 1, &descSolutionCount);
  status = napi_define_properties(env, exports, 1, &descGenerate);
  if (status != napi_ok) return NULL;
  return exports;
}

NAPI_MODULE(dlx, Init);  // 注册扩展，扩展名叫做hello，Init为扩展的初始化方法
