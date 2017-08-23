#include <gtest/gtest.h>
#include "mockphysicaldrive.h"
#include "fakephysicaldrive.h"

using ::testing::Return;

TEST(physicaldrivetest , getPathTest)
{
    MockPhysicalDrive mockPhysicalDrive;
    EXPECT_CALL(mockPhysicalDrive,getPath()).WillOnce(Return(L"111"));

    ASSERT_STREQ(mockPhysicalDrive.getPath().c_str(),L"111");
}

class TestPhysicalDrive
        : public ::testing::Test
{
protected:
    void SetUp()
    {
        auto physical_drive0 = create_fake_physical_drive(0);
        auto physical_drive1 = create_fake_physical_drive(1);

        auto physical_drive2 = create_fake_physical_drive(2);

        drivelist.add(physical_drive2);
        drivelist.add(physical_drive0);
        drivelist.add(physical_drive1);
    }
    void TearDown()
    {

    }
    IO::ListDrives drivelist;
};

TEST_F(TestPhysicalDrive , addInDeviceListTest)
{
    auto disk_1 = create_fake_physical_drive(0);
    drivelist.add(disk_1);
    auto count = drivelist.getSize();
    ASSERT_EQ(count ,drivelist.getSize() );

    auto disk_4 = create_fake_physical_drive(4);
    drivelist.add(disk_4);
    ASSERT_EQ(drivelist.getSize() , 4);

}

TEST_F (TestPhysicalDrive, removeInDeviceListTest)
{
    ASSERT_NE(drivelist.find_by_number(1), nullptr);
    drivelist.remove(1);
    ASSERT_EQ(drivelist.find_by_number(1), nullptr);
}
TEST_F(TestPhysicalDrive , FindByNumberFromDriveListTest)
{
    auto disk_1 = drivelist.find_by_number(1);

    ASSERT_EQ(disk_1->getDriveNumber() , 1 );

    auto disk_4 =  drivelist.find_by_number(4);
    ASSERT_EQ(disk_4 , nullptr);

}

TEST_F (TestPhysicalDrive , indexInDeviceListTest)
{
    ASSERT_NE(drivelist.index(1), nullptr);
    ASSERT_EQ(drivelist.index(4), nullptr);
}

TEST_F (TestPhysicalDrive , sortInDeviceListTest)
{
    drivelist.sort();
    ASSERT_EQ(drivelist.index(0)->getDriveNumber() , 0);
    ASSERT_EQ(drivelist.index(2)->getDriveNumber() , 2);

}
