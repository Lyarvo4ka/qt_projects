#ifndef FAKEPHYSICALDRIVE_H
#define FAKEPHYSICALDRIVE_H

#include "physicaldrive.h"

inline IO::PhysicalDrivePtr create_fake_physical_drive(uint32_t drive_number)
{
    auto physical_drive = std::make_shared<IO::PhysicalDrive>();
    physical_drive->setDriveNumber(drive_number);
    physical_drive->setDriveName(L"Physical drive " + std::to_wstring(drive_number));
    physical_drive->setPath(L"drive path " + std::to_wstring(drive_number));
    physical_drive->setTransferLength(1024);
    physical_drive->setSize(1000*512);
    physical_drive->setBytesPerSector(512);
    physical_drive->setSerialNumber("serial number " + std::to_string(drive_number));
    return physical_drive;

}

#endif // FAKEPHYSICALDRIVE_H
