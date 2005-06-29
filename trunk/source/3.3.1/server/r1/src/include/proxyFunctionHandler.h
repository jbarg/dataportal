
/* functionHandler.h - Header file for functionHandler.c
 */
#include <string.h>

#ifndef PROXY_FUNCTION_HANDLER_H
#define PROXY_FUNCTION_HANDLER_H


int
extractMetadata( int argc, char **argv);

typedef struct {
    func_ptr    func;
    char*       funcName;
} proxyFunctCall;


proxyFunctCall functProxyBuiltin[] = {
        {(func_ptr) extractMetadata, "extractMetadata"},
	{(func_ptr)NULL, "dummy"}
};
int nProxyBuiltinFunct = 
       (sizeof(functProxyBuiltin) / sizeof(proxyFunctCall)) - 1;



#endif	/* PROXY_FUNCTION_HANDLER_H */
