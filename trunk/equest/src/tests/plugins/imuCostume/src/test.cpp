#include <iostream>
#include <windows.h>
#include "BIC.h"

using namespace std;

int main()
{
    InitializeLibrary();

    InitFromCFG("BIC.cfg");

    Calibrate(0);
    Sleep(2000);
    Calibrate(0);
    Sleep(2000);

    while (!GetIsReady())								//Pêtla czekaj¹ca na gotowoœæ biblioteki BIC.dll
        Sleep(1);
    size_t modelsCount = GetModelsCount();
    if (GetIsReady() && modelsCount > 0) {
        size_t id = 0;
        size_t jointsCount = GetJointsCount(id);
        //size_t segmentsCount = GetSegmentsCount(id);
        while (true)								//Pêtla aktualizuj¹ca model
        {
            BICVector3 v = GetGlobalPosition(id, 0);			//Pobranie pozycji pierwszego (g³ównego) stawu
            printf("Pozycja pierwszego stawu: (%.1f,%.1f,%.1f)\n", v.x, v.y, v.z);
            for (size_t i = 1; i < jointsCount; i++)			//Pêtla iteruj¹ca pozosta³e stawy
            {
                size_t s = i - 1;					//Numery segmentów (o 1 mniejsze od numerów stawów)
                v = GetGlobalPosition(id, i);
                printf("Pozycja stawu nr.%d: (%.1f,%.1f,%.1f)\n", i + 1, v.x, v.y, v.z);
                float length = float(GetSegmentLength(id, s));
                BICQuat quat = GetGlobalOrientation(id, s);
                printf("D³ugoœæ segmentu nr.%d: %.1f\n", s + 1, length);
                printf("Kwaternion segmentu nr.%d: (%.1f,%.1f,%.1f,%.1f)\n", s + 1, quat.R, quat.I, quat.J, quat.K);
            }
            Sleep(100);
        }
    }
}



//#include <cppunit/CompilerOutputter.h>
//#include <cppunit/extensions/TestFactoryRegistry.h>
//#include <cppunit/ui/text/TestRunner.h>
//
//int main(int argc, char* argv[])
//{
//    // Get the top level suite from the registry
//    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
//
//    // Adds the test to the list of test to run
//    CppUnit::TextUi::TestRunner runner;
//    runner.addTest( suite );
//
//    // Change the default outputter to a compiler error format outputter
//    runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
//    std::cerr ) );
//    // Run the tests.
//    bool wasSucessful = runner.run();
//
//    // Return error code 1 if the one of test failed.
//    return wasSucessful ? 0 : 1;
//}
