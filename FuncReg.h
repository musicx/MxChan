#ifndef __FUNC_REG
#define __FUNC_REG
#pragma pack(push,1) 


//����(���ݸ���,���,����a,����b,����c)
typedef void(*pPluginFUNC)(int, float*, float*, float*, float*);

typedef struct tagPluginTCalcFuncInfo
{
	unsigned short		nFuncMark;//�������
	pPluginFUNC			pCallFunc;//������ַ
}PluginTCalcFuncInfo;

typedef BOOL(*pRegisterPluginFUNC)(PluginTCalcFuncInfo**);


#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
	__declspec(dllexport) BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun);
#ifdef __cplusplus
}
#endif //__cplusplus

#pragma pack(pop)
#endif
