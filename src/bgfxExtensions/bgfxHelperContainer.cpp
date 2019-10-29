#include "bgfxHelperContainer.hpp"

#include "../IOCContainer.hpp"
#include <bx/allocator.h>
#include <bx/file.h>

namespace iocContainerHelper {

static bx::FileReaderI* s_fileReader = NULL;
static bx::FileWriterI* s_fileWriter = NULL;

extern bx::AllocatorI* getDefaultAllocator();
bx::AllocatorI* g_allocator = getDefaultAllocator();

bx::AllocatorI* getDefaultAllocator()
{
BX_PRAGMA_DIAGNOSTIC_PUSH();
BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4459); // warning C4459: declaration of 's_allocator' hides global declaration
BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wshadow");
    static bx::DefaultAllocator s_allocator;
    return &s_allocator;
BX_PRAGMA_DIAGNOSTIC_POP();
}

bx::AllocatorI* getAllocator() {
    if (NULL == g_allocator) {
        g_allocator = getDefaultAllocator();
    }
    return g_allocator;
}

bx::FileReaderI* getFileReader() {
    if(s_fileReader == NULL) {
        s_fileReader = BX_NEW(getAllocator(), bx::FileReader);
    }
    return s_fileReader;
}
bx::FileWriterI* getFileWriter() {
    if(s_fileWriter == NULL) {
        s_fileWriter = BX_NEW(getAllocator(), bx::FileWriter);
    }
    return s_fileWriter;
}
//BX_DELETE(g_allocator, s_fileReader);
}