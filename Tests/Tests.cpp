#include <gtest/gtest.h>
#include "../Geometry.h"
#include "../Objects/Triangle3d.h"

TEST(Vector3d, Order) {
    ASSERT_TRUE(Point(10, 10, 10).totallyLessEqualThan(Point(10, 10, 10.1)));
    ASSERT_TRUE(Point(0, 0, 10).totallyLessEqualThan(Point(10, 10, 10.1)));
    ASSERT_TRUE(Point(10, 10, 10).totallyLessEqualThan(Point(10, 10, 10)));
    ASSERT_FALSE(Point(10, 10, 10).totallyLessEqualThan(Point(0, 0, 0)));
}

TEST(Geometry, Vector3dNormal) {
    ASSERT_EQ(Vector3d(0, 100, 0).normalize(), Vector3d(0, 1, 0));
    ASSERT_EQ(Vector3d(3, 4, 0).normalize(), Vector3d(0.6, 0.8, 0));
    ASSERT_TRUE(areDoubleEqual(Vector3d(199, -0.981912, 999).normalize().length(), 1));
}

TEST(Geometry, Vector3dGeneral) {
    Vector3d vector3d(9, -0.9, 1.7);
    ASSERT_EQ(vector3d[AXIS_X], 9);
    ASSERT_EQ(vector3d[AXIS_Y], -0.9);
    ASSERT_EQ(vector3d[AXIS_Z], 1.7);
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

TEST(BoundingBox, SurfaceArea) {
    ASSERT_EQ(BoundingBox(Point(-1, 0, 2), Point(0, 1, 0)).surfaceArea(), 10);
    ASSERT_EQ(BoundingBox(Point(-1, 0, 2), Point(0, 1, 2)).surfaceArea(), 2);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}