#include "SymbolRegister.h"
#include "JavaObject.h"

namespace JniUtils {

#define TABLE_SIZE 1024

JavaClassInfo** GetAutoRegisterTable()
{
    static JavaClassInfo* gAutoRegisterTable[TABLE_SIZE] = { nullptr };
    return gAutoRegisterTable;
}

SymbolRegister::SymbolRegister(JavaClassInfo *javaClassInfo)
{
    JavaClassInfo **table = GetAutoRegisterTable();
    for(int i = 0; i < TABLE_SIZE; ++i)
    {
        if(table[i] == nullptr)
        {
            table[i] = javaClassInfo;
            break;
        }
    }
}

void RegisterAllClass(JNIEnv *env)
{
    JavaClassInfo **table = GetAutoRegisterTable();
    for(int i = 0; i < TABLE_SIZE; ++i)
    {
        if(table[i] == nullptr)
            break;
        OnceRegisterSymbolWithClassName(table[i], env);
    }

}

}