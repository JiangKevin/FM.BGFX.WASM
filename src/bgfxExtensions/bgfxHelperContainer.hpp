#pragma once

namespace bx {
class FileReaderI;
class AllocatorI;
}

namespace iocContainerHelper
{
bx::FileReaderI* getFileReader();
bx::AllocatorI* getAllocator();
}