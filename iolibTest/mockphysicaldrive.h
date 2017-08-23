#ifndef MOCKPHYSICALDRIVE_H
#define MOCKPHYSICALDRIVE_H

#include <gmock/gmock.h>
#include "physicaldrive.h"

class MockPhysicalDrive
{
public:
    MOCK_CONST_METHOD0_T(getPath , IO::path_string());
    MOCK_CONST_METHOD0_T(getDriveNumber , uint32_t());
};



#endif // MOCKPHYSICALDRIVE_H
