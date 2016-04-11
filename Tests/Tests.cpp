#include <gtest/gtest.h>
#include "../Geometry.h"
#include "../Objects/Triangle3d.h"

TEST(Geometry, Vector3dNormal) {
    ASSERT_EQ(Vector3d(0, 100, 0).normalize(), Vector3d(0, 1, 0));
    ASSERT_EQ(Vector3d(3, 4, 0).normalize(), Vector3d(0.6, 0.8, 0));
    ASSERT_TRUE(areDoubleEqual(Vector3d(199, -0.981912, 999).normalize().length(), 1));
}

TEST(Geometry, Vector3dProduct) {
    ASSERT_EQ(Vector3d::crossProduct(Vector3d(9, 9, 9), Vector3d(1, 1, 1)).length(), 0);
    ASSERT_EQ(Vector3d::crossProduct(Vector3d(1, 0, 0), Vector3d(0, -1, 0)), Vector3d(0, 0, -1));
    ASSERT_EQ(Vector3d::crossProduct(Vector3d(1, 0, 0), Vector3d(0, 1, 0)), Vector3d(0, 0, 1));
}

TEST(Objects, GetBoundingBox) {
    Triangle3d triangle(Point(-1, -10, 2), Point(1, 0, -2), Point(0, 1, 0));
    ASSERT_EQ(triangle.getBoundingBox().minCorner, Point(-1, -10, -2));
    ASSERT_EQ(triangle.getBoundingBox().maxCorner, Point(1, 1, 2));
    triangle = Triangle3d(Point(0, 0, 0), Point(1, 0, 0), Point(0, 1, 0));
    ASSERT_EQ(triangle.getBoundingBox().minCorner, Point(0, 0, 0));
    ASSERT_EQ(triangle.getBoundingBox().maxCorner, Point(1, 1, 0));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}