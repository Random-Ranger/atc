// 
// This file is part of AT&C project which simulates virtual world of air traffic and ATC.
// Code licensing terms are available at https://github.com/felix-b/atc/blob/master/LICENSE
// 
#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>
#include <functional>
#include "gtest/gtest.h"
#include "libworld.h"
#include "libworld_test.h"

using namespace world;

TEST(GeoMathTest, flipHeading) {
    EXPECT_EQ(GeoMath::flipHeading(0), 180);         
    EXPECT_EQ(GeoMath::flipHeading(1), 181);         
    EXPECT_EQ(GeoMath::flipHeading(90), 270); 
    EXPECT_EQ(GeoMath::flipHeading(179), 359); 
    EXPECT_EQ(GeoMath::flipHeading(180), 0); 
    EXPECT_EQ(GeoMath::flipHeading(181), 1); 
    EXPECT_EQ(GeoMath::flipHeading(270), 90); 
    EXPECT_EQ(GeoMath::flipHeading(359), 179); 
}

TEST(GeoMathTest, headingToAngleDegrees) {
    EXPECT_EQ(GeoMath::headingToAngleDegrees(0), 90);         
    EXPECT_EQ(GeoMath::headingToAngleDegrees(1), 89);         
    EXPECT_EQ(GeoMath::headingToAngleDegrees(90), 0); 
    EXPECT_EQ(GeoMath::headingToAngleDegrees(91), -1); 
    EXPECT_EQ(GeoMath::headingToAngleDegrees(179), -89); 
    EXPECT_EQ(GeoMath::headingToAngleDegrees(180), -90); 
    EXPECT_EQ(GeoMath::headingToAngleDegrees(181), -91); 
    EXPECT_EQ(GeoMath::headingToAngleDegrees(269), -179); 
    EXPECT_EQ(GeoMath::headingToAngleDegrees(270), 180); 
    EXPECT_EQ(GeoMath::headingToAngleDegrees(271), 179); 
    EXPECT_EQ(GeoMath::headingToAngleDegrees(359), 91); 
}

TEST(GeoMathTest, getPointAtDistance) {
    GeoPoint p1 = GeoMath::getPointAtDistance(GeoPoint(10, 10), 90, 1000);
    EXPECT_FLOAT_EQ(p1.latitude, 10);
    EXPECT_FLOAT_EQ(p1.longitude, 10.009133);
}

TEST(GeoMathTest, getHeadingFromPoints) {
    float north = GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(1.001, 1));
    float northEast = GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(1.001, 1.001));
    float east = GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(1, 1.001));
    float southEast = GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(0.999, 1.001));
    float south = GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(0.999, 1));
    float southWest = GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(0.999, 0.999));
    float west = GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(1, 0.999));
    float northWest = GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(1.001, 0.999));
    float south2 = GeoMath::getHeadingFromPoints(GeoPoint(40, 10), GeoPoint(20, 10));

    EXPECT_NEAR(north, 0, 0.1);
    EXPECT_NEAR(northEast, 45, 0.1);
    EXPECT_NEAR(east, 90, 0.1);
    EXPECT_NEAR(southEast, 135, 0.1);
    EXPECT_NEAR(south, 180, 0.1);
    EXPECT_NEAR(southWest, 225, 0.1);
    EXPECT_NEAR(west, 270, 0.1);
    EXPECT_NEAR(northWest, 315, 0.1);
    EXPECT_NEAR(south2, 180, 0.1);
}

TEST(GeoMathTest, getRadiansAngleFromHeadingFromPoints) {
    float north = GeoMath::headingToAngleRadians(GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(1.001, 1)));
    float northEast = GeoMath::headingToAngleRadians(GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(1.001, 1.001)));
    float east = GeoMath::headingToAngleRadians(GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(1, 1.001)));
    float southEast = GeoMath::headingToAngleRadians(GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(0.999, 1.001)));
    float south = GeoMath::headingToAngleRadians(GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(0.999, 1)));
    float southWest = GeoMath::headingToAngleRadians(GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(0.999, 0.999)));
    float west = GeoMath::headingToAngleRadians(GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(1, 0.999)));
    float northWest = GeoMath::headingToAngleRadians(GeoMath::getHeadingFromPoints(GeoPoint(1, 1), GeoPoint(1.001, 0.999)));
    float south2 = GeoMath::headingToAngleRadians(GeoMath::getHeadingFromPoints(GeoPoint(40, 10), GeoPoint(20, 10)));

    EXPECT_NEAR(north,      M_PI_2, 0.01);
    EXPECT_NEAR(northEast,  M_PI_4, 0.01);
    EXPECT_NEAR(east,       0,      0.01);
    EXPECT_NEAR(southEast, -M_PI_4, 0.01);
    EXPECT_NEAR(south,     -M_PI_2, 0.01);
    EXPECT_NEAR(southWest, -M_PI + M_PI_4, 0.01);
    EXPECT_NEAR(west,       M_PI,   0.01);
    EXPECT_NEAR(northWest,  M_PI - M_PI_4, 0.01);
    EXPECT_NEAR(south2,    -M_PI_2, 0.01);
}

TEST(GeoMathTest, headingToAngleRadians) {
    double north = GeoMath::headingToAngleRadians(0);
    double northEast = GeoMath::headingToAngleRadians(45);
    double east = GeoMath::headingToAngleRadians(90);
    double southEast = GeoMath::headingToAngleRadians(135);
    double south = GeoMath::headingToAngleRadians(180);
    double southWest = GeoMath::headingToAngleRadians(225);
    double west = GeoMath::headingToAngleRadians(270);
    double northWest = GeoMath::headingToAngleRadians(315);

    EXPECT_NEAR(north,      M_PI_2, 0.01);
    EXPECT_NEAR(northEast,  M_PI_4, 0.01);
    EXPECT_NEAR(east,       0,      0.01);
    EXPECT_NEAR(southEast, -M_PI_4, 0.01);
    EXPECT_NEAR(south,     -M_PI_2, 0.01);
    EXPECT_NEAR(southWest, -M_PI + M_PI_4, 0.01);
    EXPECT_NEAR(west,       M_PI,   0.01);
    EXPECT_NEAR(northWest,  M_PI - M_PI_4, 0.01);
}

TEST(GeoMathTest, getRadiansFromPoints) {
    double north = GeoMath::getRadiansFromPoints(GeoPoint(1, 1), GeoPoint(1.001, 1));
    double northEast = GeoMath::getRadiansFromPoints(GeoPoint(1, 1), GeoPoint(1.001, 1.001));
    double east = GeoMath::getRadiansFromPoints(GeoPoint(1, 1), GeoPoint(1, 1.001));
    double southEast = GeoMath::getRadiansFromPoints(GeoPoint(1, 1), GeoPoint(0.999, 1.001));
    double south = GeoMath::getRadiansFromPoints(GeoPoint(1, 1), GeoPoint(0.999, 1));
    double southWest = GeoMath::getRadiansFromPoints(GeoPoint(1, 1), GeoPoint(0.999, 0.999));
    double west = GeoMath::getRadiansFromPoints(GeoPoint(1, 1), GeoPoint(1, 0.999));
    double northWest = GeoMath::getRadiansFromPoints(GeoPoint(1, 1), GeoPoint(1.001, 0.999));
    double south2 = GeoMath::getRadiansFromPoints(GeoPoint(40, 10), GeoPoint(20, 10));

    EXPECT_NEAR(north,      M_PI_2, 0.01);
    EXPECT_NEAR(northEast,  M_PI_4, 0.01);
    EXPECT_NEAR(east,       0,      0.01);
    EXPECT_NEAR(southEast, -M_PI_4, 0.01);
    EXPECT_NEAR(south,     -M_PI_2, 0.01);
    EXPECT_NEAR(southWest, -M_PI + M_PI_4, 0.01);
    EXPECT_NEAR(west,       M_PI,   0.01);
    EXPECT_NEAR(northWest,  M_PI - M_PI_4, 0.01);
    EXPECT_NEAR(south2,    -M_PI_2, 0.01);
}

TEST(GeoMathTest, calculateTurn) {
    const auto runCase = [=](
        GeoPoint e1p0, GeoPoint e1p1, GeoPoint e2p0, GeoPoint e2p1, float radius,
        GeoPoint arcP0, GeoPoint arcP1, GeoPoint arcCenter,
        double arcA0, double arcA1, bool clockwise, double arcDeltaA
    ){
        shared_ptr<TestHostServices> host(new TestHostServices());

        // cout << "getHeadingFromPoints(e1p0, e1p1)=" << GeoMath::getHeadingFromPoints(e1p0, e1p1) << endl;
        // cout << "getHeadingFromPoints(e2p0, e2p1)=" << GeoMath::getHeadingFromPoints(e2p0, e2p1) << endl;

        double e1Heading = GeoMath::getRadiansFromPoints(e1p0, e1p1);//  calcSlopeRadians(e1p0, e1p1);
        double e2Heading = GeoMath::getRadiansFromPoints(e2p0, e2p1); //calcSlopeRadians(e2p0, e2p1);
        GeoMath::TurnData input = { e1p0, e1p1, e1Heading, e2p0, e2p1, e2Heading, radius };
        GeoMath::TurnArc output;

        GeoMath::calculateTurn(input, output, host);

        EXPECT_FLOAT_EQ(output.p0.latitude, arcP0.latitude);
        EXPECT_FLOAT_EQ(output.p0.longitude, arcP0.longitude);
        EXPECT_FLOAT_EQ(output.p1.latitude, arcP1.latitude);
        EXPECT_FLOAT_EQ(output.p1.longitude, arcP1.longitude);
        EXPECT_FLOAT_EQ(output.arcCenter.latitude, arcCenter.latitude);
        EXPECT_FLOAT_EQ(output.arcCenter.longitude, arcCenter.longitude);
        EXPECT_FLOAT_EQ(output.arcStartAngle, arcA0);
        EXPECT_FLOAT_EQ(output.arcEndAngle, arcA1);
        EXPECT_FLOAT_EQ(output.arcClockwise, clockwise);
        EXPECT_FLOAT_EQ(output.arcRadius, (float)radius);
        EXPECT_FLOAT_EQ(output.arcDeltaAngle, arcDeltaA);
    };

    runCase({40,10},{20,10},{20,10},{20,3},3, /**/ {23,10},{20,7},{23,7},0,-1.5707963267948966,true,-1.5707963267948966);
    runCase({40,10},{20,10},{20,10},{20,30},3, /**/ {23,10},{20,13},{23,13},3.141592653589793,-1.5707963267948966,false,1.5707963267948966);
    runCase({0,10},{20,10},{20,10},{20,30},3, /**/ {17,10},{20,13},{17,13},3.141592653589793,1.5707963267948966,true,-1.5707963267948966);
    runCase({0,10},{20,10},{20,10},{20,3},3, /**/ {17,10},{20,7},{17,7},0,1.5707963267948966,false,1.5707963267948966);
    runCase({0,0},{20,20},{20,20},{40,0},3, /**/ {17.878679656440358,17.878679656440358},{22.121320343559642,17.878679656440358},{20,15.757359312880716},-0.7853981633974483,0.7853981633974483,false,1.5707963267948966);
    runCase({0,0},{20,20},{20,20},{10,0},3, /**/ {6.927835036821705,6.927835036821699},{11.732436953381091,3.464873906762186},{9.049155380381347,4.8065146932620575},-0.7853981633974483,2.0344439357957027,false,2.819842099193151);
    runCase({10,0},{20,20},{20,20},{0,0},3, /**/ {11.732436953381095,3.464873906762186},{6.927835036821703,6.927835036821705},{9.049155380381347,4.80651469326206},2.0344439357957027,-0.7853981633974483,true,-2.819842099193151);
    runCase({40,0},{20,20},{20,20},{30,0},3, /**/ {33.072164963178295,6.927835036821705},{28.26756304661891,3.464873906762186},{30.950844619618653,4.806514693262063},0.7853981633974483,-2.0344439357957027,true,-2.819842099193151);
    runCase({30,0},{20,20},{20,20},{40,0},3, /**/ {28.26756304661891,3.464873906762186},{33.072164963178295,6.927835036821705},{30.950844619618657,4.80651469326206},-2.0344439357957027,0.7853981633974483,false,2.819842099193151);
    runCase({30,40},{20,20},{20,20},{40,40},3, /**/ {28.267563046618907,36.535126093237814},{33.072164963178295,33.072164963178295},{30.950844619618653,35.19348530673794},-1.1071487177940904,2.356194490192345,true,-2.819842099193151);
    runCase({40,40},{20,20},{20,20},{30,40},3, /**/ {33.072164963178295,33.072164963178295},{28.267563046618907,36.535126093237814},{30.950844619618653,35.19348530673794},2.356194490192345,-1.1071487177940904,false,2.819842099193151);
    runCase({0,40},{20,20},{20,20},{10,40},3, /**/ {6.927835036821699,33.072164963178295},{11.732436953381093,36.535126093237814},{9.049155380381341,35.19348530673794},-2.356194490192345,1.1071487177940904,true,-2.819842099193151);
    runCase({10,40},{20,20},{20,20},{0,40},3, /**/ {11.732436953381088,36.53512609323781},{6.927835036821705,33.072164963178295},{9.04915538038134,35.193485306737934},1.1071487177940904,-2.356194490192345,false,2.819842099193151);
    runCase({10,40},{20,20},{20,20},{30,40},3, /**/ {17.31671842700025,25.366563145999493},{22.683281572999746,25.366563145999496},{19.999999999999996,26.708203932499366},-2.0344439357957027,2.0344439357957027,true,-2.214297435588181);
    runCase({30,40},{20,20},{20,20},{10,40},3, /**/ {22.683281572999746,25.366563145999496},{17.316718427000254,25.366563145999496},{20,26.70820393249937},2.0344439357957027,-2.0344439357957027,false,2.214297435588181);
    runCase({30,10},{20,20},{20,20},{30,30},3, /**/ {22.121320343559642,17.878679656440358},{22.121320343559642,22.121320343559642},{24.242640687119284,20},-2.356194490192345,-0.7853981633974483,false,1.5707963267948966);
    runCase({30,30},{20,20},{20,20},{30,10},3, /**/ {22.121320343559642,22.121320343559642},{22.121320343559642,17.878679656440358},{24.242640687119284,20},-0.7853981633974483,-2.356194490192345,true,-1.5707963267948966);
    runCase({30,10},{20,20},{20,20},{10,10},3, /**/ {22.121320343559642,17.878679656440358},{17.878679656440358,17.878679656440358},{20,15.757359312880716},0.7853981633974483,-0.7853981633974483,true,-1.5707963267948966);
    runCase({10,10},{20,20},{20,20},{30,10},3, /**/ {17.878679656440358,17.878679656440358},{22.121320343559642,17.878679656440358},{20,15.757359312880716},-0.7853981633974483,0.7853981633974483,false,1.5707963267948966);
    runCase({10,10},{20,20},{20,20},{10,30},3, /**/ {17.878679656440358,17.878679656440358},{17.878679656440358,22.121320343559642},{15.757359312880716,20},2.356194490192345,0.7853981633974483,true,-1.5707963267948966);
    runCase({10,30},{20,20},{20,20},{30,30},3, /**/ {17.878679656440358,22.121320343559642},{22.121320343559642,22.121320343559642},{20,24.242640687119284},-2.356194490192345,2.356194490192345,true,-1.5707963267948966);
    runCase({10,10},{20,20},{20,20},{10,30},3, /**/ {17.878679656440358,17.878679656440358},{17.878679656440358,22.121320343559642},{15.757359312880716,20},2.356194490192345,0.7853981633974483,true,-1.5707963267948966);
    runCase({10,30},{20,20},{20,20},{10,10},3, /**/ {17.878679656440358,22.121320343559642},{17.878679656440358,17.878679656440358},{15.757359312880716,20},0.7853981633974483,2.356194490192345,false,1.5707963267948966);
    runCase({10,10},{20,20},{20,20},{30,25},3, /**/ {19.65575709817956,19.65575709817956},{20.43543665523933,20.217718327619664},{21.777077441739202,17.534436754619918},-0.7853981633974483,-0.46364760900080615,false,0.32175055439664213);
    runCase({10,10},{20,20},{20,20},{30,25},6, /**/ {19.311514196359116,19.311514196359116},{20.870873310478654,20.43543665523933},{23.554154883478404,15.068873509239832},-0.7853981633974483,-0.46364760900080615,false,0.32175055439664213);
    runCase({10,10},{20,20},{20,20},{30,35},6, /**/ {19.57989578281249,19.57989578281249},{20.329556861720267,20.4943352925804},{15.337255095693205,23.822536469931777},2.356194490192345,2.158798930342464,true,-0.19739555984988089);
    runCase({40,40},{20,20},{20,20},{0,10},6, /**/ {20.688485803640884,20.688485803640884},{19.129126689521346,19.56456334476067},{16.4458451165216,24.93112649076017},2.356194490192345,2.677945044588987,false,0.32175055439664213);
    runCase({40,40},{20,20},{20,20},{0,5},6, /**/ {20.301515190165006,20.301515190165006},{19.65887450304572,19.74415587728429},{16.058874503045722,24.54415587728429},2.356194490192345,2.498091544796509,false,0.14189705460416402);
    runCase({40,40},{20,20},{20,20},{10,5},6, /**/ {20.42010421718751,20.42010421718751},{19.670443138279733,19.5056647074196},{24.662744904306795,16.177463530068223},-0.7853981633974483,-0.9827937232473296,true,-0.19739555984988133);
    runCase({40,0},{20,20},{20,20},{0,5},6, /**/ {23.67962275898296,16.32037724101704},{15.836982071863673,16.877736553897755},{19.436982071863675,12.077736553897758},0.7853981633974483,-0.6435011087932843,true,-1.4288992721907325);
    runCase({40,0},{20,20},{20,20},{0,45},6, /**/ {20.234979240523526,19.765020759476474},{19.79240707098463,20.259491161269214},{24.47761992764281,24.007661446595762},-2.356194490192345,-2.245537269018449,false,0.11065722117389587);
    runCase({40,0},{20,20},{20,20},{10,45},6, /**/ {20.870834677657342,19.129165322342658},{19.542615219465073,21.143461951337322},{25.113475364776626,23.37180600946194},-2.356194490192345,-1.9513027039072615,false,0.4048917862850834);
    runCase({40,0},{20,20},{20,20},{10,35},6, /**/ {20.42010421718751,19.57989578281249},{19.670443138279733,20.4943352925804},{24.662744904306795,23.822536469931777},-2.356194490192345,-2.1587989303424644,false,0.19739555984988044);
    runCase({40,0},{20,20},{20,20},{10,25},6, /**/ {20.688485803640884,19.311514196359116},{19.129126689521346,20.43543665523933},{16.445845116521596,15.068873509239832},0.7853981633974483,0.46364760900080615,true,-0.32175055439664213);


    // runCase({40,10},{20,10},{20,10},{20,3},3, /**/ {23,10},{20,7},{23,7},0,-1.5707963267948966,true);
    // runCase({40,10},{20,10},{20,10},{20,30},3, /**/ {23,10},{20,13},{23,13},3.141592653589793,-1.5707963267948966,false);
    // runCase({0,10},{20,10},{20,10},{20,30},3, /**/ {17,10},{20,13},{17,13},3.141592653589793,1.5707963267948966,true);
    // runCase({0,10},{20,10},{20,10},{20,3},3, /**/ {17,10},{20,7},{17,7},0,1.5707963267948966,false);
    // runCase({0,0},{20,20},{20,20},{40,0},3, /**/ {17.878679656440358,17.878679656440358},{22.121320343559642,17.878679656440358},{20,15.757359312880716},-0.7853981633974483,0.7853981633974483,false);
    // runCase({0,0},{20,20},{20,20},{10,0},3, /**/ {6.927835036821705,6.927835036821699},{11.732436953381091,3.464873906762186},{9.049155380381347,4.8065146932620575},-0.7853981633974483,2.0344439357957027,false);
    // runCase({10,0},{20,20},{20,20},{0,0},3, /**/ {11.732436953381095,3.464873906762186},{6.927835036821703,6.927835036821705},{9.049155380381347,4.80651469326206},2.0344439357957027,-0.7853981633974483,true);
    // runCase({40,0},{20,20},{20,20},{30,0},3, /**/ {33.072164963178295,6.927835036821705},{28.26756304661891,3.464873906762186},{30.950844619618653,4.806514693262063},0.7853981633974483,-2.0344439357957027,true);
    // runCase({30,0},{20,20},{20,20},{40,0},3, /**/ {28.26756304661891,3.464873906762186},{33.072164963178295,6.927835036821705},{30.950844619618657,4.80651469326206},-2.0344439357957027,0.7853981633974483,false);
    // runCase({30,40},{20,20},{20,20},{40,40},3, /**/ {28.267563046618907,36.535126093237814},{33.072164963178295,33.072164963178295},{30.950844619618653,35.19348530673794},-1.1071487177940904,2.356194490192345,true);
    // runCase({40,40},{20,20},{20,20},{30,40},3, /**/ {33.072164963178295,33.072164963178295},{28.267563046618907,36.535126093237814},{30.950844619618653,35.19348530673794},2.356194490192345,-1.1071487177940904,false);
    // runCase({0,40},{20,20},{20,20},{10,40},3, /**/ {6.927835036821699,33.072164963178295},{11.732436953381093,36.535126093237814},{9.049155380381341,35.19348530673794},-2.356194490192345,1.1071487177940904,true);
    // runCase({10,40},{20,20},{20,20},{0,40},3, /**/ {11.732436953381088,36.53512609323781},{6.927835036821705,33.072164963178295},{9.04915538038134,35.193485306737934},1.1071487177940904,-2.356194490192345,false);
    // runCase({10,40},{20,20},{20,20},{30,40},3, /**/ {17.31671842700025,25.366563145999493},{22.683281572999746,25.366563145999496},{19.999999999999996,26.708203932499366},-2.0344439357957027,2.0344439357957027,true);
    // runCase({30,40},{20,20},{20,20},{10,40},3, /**/ {22.683281572999746,25.366563145999496},{17.316718427000254,25.366563145999496},{20,26.70820393249937},2.0344439357957027,-2.0344439357957027,false);
    // runCase({30,10},{20,20},{20,20},{30,30},3, /**/ {22.121320343559642,17.878679656440358},{22.121320343559642,22.121320343559642},{24.242640687119284,20},-2.356194490192345,-0.7853981633974483,false);
    // runCase({30,30},{20,20},{20,20},{30,10},3, /**/ {22.121320343559642,22.121320343559642},{22.121320343559642,17.878679656440358},{24.242640687119284,20},-0.7853981633974483,-2.356194490192345,true);
    // runCase({30,10},{20,20},{20,20},{10,10},3, /**/ {22.121320343559642,17.878679656440358},{17.878679656440358,17.878679656440358},{20,15.757359312880716},0.7853981633974483,-0.7853981633974483,true);
    // runCase({10,10},{20,20},{20,20},{30,10},3, /**/ {17.878679656440358,17.878679656440358},{22.121320343559642,17.878679656440358},{20,15.757359312880716},-0.7853981633974483,0.7853981633974483,false);
    // runCase({10,10},{20,20},{20,20},{10,30},3, /**/ {17.878679656440358,17.878679656440358},{17.878679656440358,22.121320343559642},{15.757359312880716,20},2.356194490192345,0.7853981633974483,true);
    // runCase({10,30},{20,20},{20,20},{30,30},3, /**/ {17.878679656440358,22.121320343559642},{22.121320343559642,22.121320343559642},{20,24.242640687119284},-2.356194490192345,2.356194490192345,true);
    // runCase({10,10},{20,20},{20,20},{10,30},3, /**/ {17.878679656440358,17.878679656440358},{17.878679656440358,22.121320343559642},{15.757359312880716,20},2.356194490192345,0.7853981633974483,true);
    // runCase({10,30},{20,20},{20,20},{10,10},3, /**/ {17.878679656440358,22.121320343559642},{17.878679656440358,17.878679656440358},{15.757359312880716,20},0.7853981633974483,2.356194490192345,false);
    // runCase({10,10},{20,20},{20,20},{30,25},3, /**/ {19.65575709817956,19.65575709817956},{20.43543665523933,20.217718327619664},{21.777077441739202,17.534436754619918},-0.7853981633974483,-0.46364760900080615,false);
    // runCase({10,10},{20,20},{20,20},{30,25},6, /**/ {19.311514196359116,19.311514196359116},{20.870873310478654,20.43543665523933},{23.554154883478404,15.068873509239832},-0.7853981633974483,-0.46364760900080615,false);
    // runCase({10,10},{20,20},{20,20},{30,35},6, /**/ {19.57989578281249,19.57989578281249},{20.329556861720267,20.4943352925804},{15.337255095693205,23.822536469931777},2.356194490192345,2.158798930342464,true);
    // runCase({40,40},{20,20},{20,20},{0,10},6, /**/ {20.688485803640884,20.688485803640884},{19.129126689521346,19.56456334476067},{16.4458451165216,24.93112649076017},2.356194490192345,2.677945044588987,false);
    // runCase({40,40},{20,20},{20,20},{0,5},6, /**/ {20.301515190165006,20.301515190165006},{19.65887450304572,19.74415587728429},{16.058874503045722,24.54415587728429},2.356194490192345,2.498091544796509,false);
    // runCase({40,40},{20,20},{20,20},{10,5},6, /**/ {20.42010421718751,20.42010421718751},{19.670443138279733,19.5056647074196},{24.662744904306795,16.177463530068223},-0.7853981633974483,-0.9827937232473296,true);
    // runCase({40,0},{20,20},{20,20},{0,5},6, /**/ {23.67962275898296,16.32037724101704},{15.836982071863673,16.877736553897755},{19.436982071863675,12.077736553897758},0.7853981633974483,-0.6435011087932843,true);
    // runCase({40,0},{20,20},{20,20},{0,45},6, /**/ {20.234979240523526,19.765020759476474},{19.79240707098463,20.259491161269214},{24.47761992764281,24.007661446595762},-2.356194490192345,-2.245537269018449,false);
    // runCase({40,0},{20,20},{20,20},{10,45},6, /**/ {20.870834677657342,19.129165322342658},{19.542615219465073,21.143461951337322},{25.113475364776626,23.37180600946194},-2.356194490192345,-1.9513027039072615,false);
    // runCase({40,0},{20,20},{20,20},{10,35},6, /**/ {20.42010421718751,19.57989578281249},{19.670443138279733,20.4943352925804},{24.662744904306795,23.822536469931777},-2.356194490192345,-2.1587989303424644,false);
    // runCase({40,0},{20,20},{20,20},{10,25},6, /**/ {20.688485803640884,19.311514196359116},{19.129126689521346,20.43543665523933},{16.445845116521596,15.068873509239832},0.7853981633974483,0.46364760900080615,true);
}

TEST(GeoMathTest, isPointInRectangle_not_rotated)
{
    GeoPoint A(10, 10);
    GeoPoint B(20, 10);
    GeoPoint C(20, 20);
    GeoPoint D(10, 20);

    EXPECT_EQ(GeoMath::isPointInRectangle({ 15, 15 }, A, B, C, D), true);
    EXPECT_EQ(GeoMath::isPointInRectangle({ 9, 15 }, A, B, C, D), false);
    EXPECT_EQ(GeoMath::isPointInRectangle({ 15, 9 }, A, B, C, D), false);
}

TEST(GeoMathTest, isPointInRectangle_rotated_45deg)
{
    GeoPoint A(10, 10);
    GeoPoint B(20, 20);
    GeoPoint C(10, 30);
    GeoPoint D(0, 20);

    EXPECT_EQ(GeoMath::isPointInRectangle({ 10, 20 }, A, B, C, D), true);
    EXPECT_EQ(GeoMath::isPointInRectangle({ 9, 9 }, A, B, C, D), false);
    EXPECT_EQ(GeoMath::isPointInRectangle({ 20, 10 }, A, B, C, D), false);
}

TEST(GeoMathTest, isPointInRectangle_kjfk_13R)
{
    auto const runCase = [](
        const string& title,
        const GeoPoint& A,
        const GeoPoint& B,
        const GeoPoint& C,
        const GeoPoint& D
    ) {
        cout << "------" << title << "------" << endl;
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.652128, -73.798528 }, A, B, C, D), false);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.649104, -73.815944 }, A, B, C, D), false);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.647371, -73.812204 }, A, B, C, D), false);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.641840, -73.805834 }, A, B, C, D), false);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.633832, -73.782285 }, A, B, C, D), false);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.630753, -73.780684 }, A, B, C, D), false);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.629427, -73.772282 }, A, B, C, D), false);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.627500, -73.770617 }, A, B, C, D), false);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.648795, -73.817641 }, A, B, C, D), false);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.648127, -73.816120 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.645708, -73.810967 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.633044, -73.782821 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.628087, -73.771746 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.648408, -73.816731 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.646802, -73.812522 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.642743, -73.805052 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.638231, -73.793107 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.633924, -73.785747 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.631465, -73.780222 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.630301, -73.775823 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.628884, -73.772711 }, A, B, C, D), true);
        EXPECT_EQ(GeoMath::isPointInRectangle({ 40.628469, -73.771660 }, A, B, C, D), true);
    };

    GeoPoint A(40.647908, -73.817450);
    GeoPoint B(40.649088, -73.816771);
    GeoPoint C(40.628245, -73.771064);
    GeoPoint D(40.627292, -73.771885);
    runCase("ABCD", A, B, C, D);
    runCase("BCDA", B, C, D, A);
    runCase("CDAB", C, D, A, B);
    runCase("DABC", D, A, B, C);
}


TEST(GeoMathTest, isPointInRectangle_eham_36R)
{
    auto const runCase = [](
        const string& title,
        const GeoPoint& A,
        const GeoPoint& B,
        const GeoPoint& C,
        const GeoPoint& D
    ) {
        cout << "------" << title << "------" << endl;
        EXPECT_FALSE(GeoMath::isPointInRectangle({ 52.321248, 4.779392 }, A, B, C, D));
        EXPECT_FALSE(GeoMath::isPointInRectangle({ 52.317832, 4.780700 }, A, B, C, D));
        EXPECT_FALSE(GeoMath::isPointInRectangle({ 52.315972, 4.778963 }, A, B, C, D));
        EXPECT_FALSE(GeoMath::isPointInRectangle({ 52.309104, 4.779717 }, A, B, C, D));
        EXPECT_FALSE(GeoMath::isPointInRectangle({ 52.304577, 4.777836 }, A, B, C, D));
        EXPECT_FALSE(GeoMath::isPointInRectangle({ 52.304117, 4.779372 }, A, B, C, D));
        EXPECT_FALSE(GeoMath::isPointInRectangle({ 52.291021, 4.778135 }, A, B, C, D));
        EXPECT_FALSE(GeoMath::isPointInRectangle({ 52.290938, 4.778140 }, A, B, C, D));
        EXPECT_FALSE(GeoMath::isPointInRectangle({ 52.290607, 4.777314 }, A, B, C, D));
        EXPECT_FALSE(GeoMath::isPointInRectangle({ 52.321548, 4.780188 }, A, B, C, D));

        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.321300, 4.780172 }, A, B, C, D));
        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.321112, 4.779853 }, A, B, C, D));
        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.320455, 4.780101 }, A, B, C, D));
        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.317809, 4.780130 }, A, B, C, D));
        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.311713, 4.779295 }, A, B, C, D));
        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.308875, 4.778615 }, A, B, C, D));
        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.304352, 4.778910 }, A, B, C, D));
        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.300028, 4.778216 }, A, B, C, D));
        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.290988, 4.777677 }, A, B, C, D));
        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.290858, 4.777366 }, A, B, C, D));
        EXPECT_TRUE(GeoMath::isPointInRectangle({ 52.292343, 4.777484 }, A, B, C, D));
    };

    GeoPoint A(52.321421, 4.779640);
    GeoPoint B(52.321386, 4.780734);
    GeoPoint C(52.290813, 4.777974);
    GeoPoint D(52.290866, 4.776762);
    runCase("ABCD", A, B, C, D);
    runCase("BCDA", B, C, D, A);
    runCase("CDAB", C, D, A, B);
    runCase("DABC", D, A, B, C);
}

