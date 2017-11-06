#include <utils/Pool.h>
#include "PoolTest.h"

using namespace utils;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PoolTest );

//! Mała struktura.
struct Small {
  char c;
  bool operator==(const Small & rhs) const { return !memcmp(this, &rhs, sizeof(rhs)); }
  bool operator!=(const Small & rhs) const { return !(*this==rhs); }
};
std::ostream& operator<<(std::ostream& s, const Small& rhs) 
{ 
  return s << "{" << rhs.c << "}"; 
};

//! Struktura średniego rozmiaru.
struct Medium {
  char c[2];
  int i;
  float f;
  bool operator==(const Medium & rhs) const { return !memcmp(this, &rhs, sizeof(rhs)); }
  bool operator!=(const Medium & rhs) const { return !(*this==rhs); }
};
std::ostream& operator<<(std::ostream& s, const Medium& rhs) 
{ 
  return s<<"{"<< rhs.c[0]<<","<<rhs.c[1]<<","<<rhs.i<<","<<rhs.f<<"}";
};

//! Duża struktura.
struct Large {
  double d[2];
  int i[4];
  float f;
  bool operator==(const Large & rhs) const { return !memcmp(this, &rhs, sizeof(rhs)); }
  bool operator!=(const Large & rhs) const { return !(*this==rhs); }
};
std::ostream& operator<<(std::ostream& s, const Large& rhs) 
{ 
  return s<<"{"<< rhs.d[0]<<","<<rhs.d[1]<<","<<rhs.i[0]<<","<<rhs.i[1]<<","<<rhs.i[2]<<","<<rhs.i[3]<<","<<rhs.f<<"}";
};


int totalInstances = 0;

template <class Base>
class Wrapper : public Base
{
public:
  static int instances;
  Wrapper(const Base& rhs)
  {
    CPPUNIT_ASSERT(0 <= Wrapper::instances);
    CPPUNIT_ASSERT(0 <= totalInstances);
    memcpy(this, &rhs, sizeof(rhs));
    ++instances;
    ++totalInstances;
  }
  Wrapper(const Wrapper& rhs)
  {
    CPPUNIT_ASSERT(0 <= Wrapper::instances);
    CPPUNIT_ASSERT(0 <= totalInstances);
    memcpy(this, &rhs, sizeof(rhs));
    ++instances;
    ++totalInstances;
  }
  ~Wrapper()
  {
    CPPUNIT_ASSERT(0 < Wrapper::instances);
    CPPUNIT_ASSERT(0 < totalInstances);
    --instances;
    --totalInstances;
  }
};

typedef Wrapper<Small> SmallClass;
typedef Wrapper<Medium> MediumClass;
typedef Wrapper<Large> LargeClass;
int SmallClass::instances = 0;
int MediumClass::instances = 0;
int LargeClass::instances = 0;



void PoolTest::testPrimitives()
{
  {
    // dodanie stałych
    int *p1, *p2, *p3;
    Pool pool(100, 200, 300);
    pool.getValues(p1, p2, p3);
    CPPUNIT_ASSERT_EQUAL( 100, *p1 );
    CPPUNIT_ASSERT_EQUAL( 200, *p2 );
    CPPUNIT_ASSERT_EQUAL( 300, *p3 );
  }

  // testowanie tych samych typów
  testPool(100);
  testPool(100, 200);
  testPool(100, 200, 300);

  // testowanie różnych typów
  testPool(100, 200.0);
  testPool(100.0f, 200.0);
  testPool(100, 200.0, reinterpret_cast<int*>(666));

  // testowanie struktur
  Small s = { '0' };
  Medium m = { {'0','1'}, 666, 10.0f };
  Large l = { {0.33334, 0.66666}, {1, 1, 2, 3}, {5.0f} };
  testPool(s);
  testPool(s, s);
  testPool(m, s);
  testPool(m, s, m);
  testPool(l, s);
  testPool(l, s, 10);
  testPool('c', l, 10);

}

void PoolTest::testDestructors()
{
  {
    // testowanie struktur
    Small s = { '0' };
    Medium m = { {'0','1'}, 666, 10.0f };
    Large l = { {0.33334, 0.66666}, {1, 1, 2, 3}, {5.0f} };

    // tworzymy wrappery
    SmallClass sc(s);
    MediumClass mc(m);
    LargeClass lc(l);
    // powinna zostać jedna instancja!
    CPPUNIT_ASSERT_EQUAL(1, SmallClass::instances);
    CPPUNIT_ASSERT_EQUAL(1, MediumClass::instances);
    CPPUNIT_ASSERT_EQUAL(1, LargeClass::instances);

    // testujemy kopiowanie
    testPool(sc);
    testPool(sc, sc);
    testPool(mc, sc);
    testPool(mc, sc, mc);
    testPool(lc, sc);
    testPool(lc, sc, 10);
    testPool('c', lc, 10);

    // dodajemy do puli
    Pool pool(sc, mc, lc);
    CPPUNIT_ASSERT_EQUAL(2, SmallClass::instances);
    CPPUNIT_ASSERT_EQUAL(2, MediumClass::instances);
    CPPUNIT_ASSERT_EQUAL(2, LargeClass::instances);
  }
  CPPUNIT_ASSERT_EQUAL(0, totalInstances);
  CPPUNIT_ASSERT_EQUAL(0, SmallClass::instances);
  CPPUNIT_ASSERT_EQUAL(0, MediumClass::instances);
  CPPUNIT_ASSERT_EQUAL(0, LargeClass::instances);
}


template <class T1, int Size>
void PoolTest::testPool( const T1 (&data1)[Size] )
{
  Pool pool(data1);
  T1 (&arr)[Size] = pool.getValue(T1[Size])();
  for (int i = 0; i < Size; ++i) {
    CPPUNIT_ASSERT_EQUAL(data1[i], arr[i]);
  }
}

template <class T1, class T2, class T3>
void PoolTest::testPool( const T1& data1, const T2& data2, const T3& data3 )
{
  Pool pool(data1, data2, data3);
  T1 * p1;
  T2 * p2;
  T3 * p3;
  pool.getValues(p1, p2, p3);
  CPPUNIT_ASSERT_EQUAL(data1, *p1);
  CPPUNIT_ASSERT_EQUAL(data2, *p2);
  CPPUNIT_ASSERT_EQUAL(data3, *p3);
}

template <class T1, class T2>
void PoolTest::testPool( const T1& data1, const T2& data2 )
{
  Pool pool(data1, data2);
  T1 * p1;
  T2 * p2;
  pool.getValues(p1, p2);
  CPPUNIT_ASSERT_EQUAL(data1, *p1);
  CPPUNIT_ASSERT_EQUAL(data2, *p2);
}

template <class T1>
void PoolTest::testPool( const T1& data1 )
{
  Pool pool(data1);
  T1 * p1;
  pool.getValues(p1);
  CPPUNIT_ASSERT_EQUAL(data1, *p1);
}
