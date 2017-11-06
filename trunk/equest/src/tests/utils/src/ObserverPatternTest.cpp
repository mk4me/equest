#include <vector>
#include <utils/ObserverPattern.h>
#include "ObserverPatternTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ObserverPatternTest );

//! Prosty typ który zwiększa wartość wewnętrznego licznika.
class Subject : public utils::Observable<Subject>
{
public:
  int changeCount;

public:
  Subject() : changeCount(0) {}
  void changeState()
  {
    ++changeCount;
    notify();
  }
};


//! Prosty obserwator kopiujący wartość licznika.
struct Observer : public utils::Observer<Subject>
{
  int changeCount;

  Observer(int changeCount = 0) : changeCount(changeCount) {}

  void update(const Subject * subject)
  {
    changeCount = subject->changeCount;
  }

  bool operator<( const Observer& observer ) const
  {
    return changeCount < observer.changeCount;
  }
};


typedef std::vector<Observer>::iterator iterator;
typedef std::pair<iterator, iterator> pair;


void ObserverPatternTest::testAttach()
{
  Subject subject;
  Observer observer;

  // podstawowy test
  CPPUNIT_ASSERT_EQUAL(false, subject.isAttached(&observer));
  subject.attach(&observer);
  CPPUNIT_ASSERT_EQUAL(true, subject.isAttached(&observer));

  // dodanie kilku obserwatorów
  Observer observer1, observer2;
  subject.attach(&observer1);
  subject.attach(&observer2);
  CPPUNIT_ASSERT_EQUAL(true, subject.isAttached(&observer1));
  CPPUNIT_ASSERT_EQUAL(true, subject.isAttached(&observer2));

  // teraz powinniśmy dostać wyjątek
  subject.attach(&observer);
}

void ObserverPatternTest::testDetach()
{
  Subject subject;
  Observer observer;

  // podstawowy test
  subject.attach(&observer);
  CPPUNIT_ASSERT_EQUAL(true, subject.isAttached(&observer));
  subject.detach(&observer);
  CPPUNIT_ASSERT_EQUAL(false, subject.isAttached(&observer));

  // dodanie kilku i odpięcie jednego
  Observer observer1, observer2;
  subject.attach(&observer);
  subject.attach(&observer1);
  subject.attach(&observer2);
  subject.detach(&observer);
  CPPUNIT_ASSERT_EQUAL(false, subject.isAttached(&observer));

  // odpięcie wszystkich
  subject.attach(&observer);
  subject.detachAll();
  CPPUNIT_ASSERT_EQUAL(false, subject.isAttached(&observer));
  CPPUNIT_ASSERT_EQUAL(false, subject.isAttached(&observer1));
  CPPUNIT_ASSERT_EQUAL(false, subject.isAttached(&observer2));

  // teraz powinniśmy dostać wyjątek
  subject.detach(&observer);
}

void ObserverPatternTest::testNotify()
{
  Subject subject;
  std::vector<Observer> observers(10, Observer());

  iterator begin = observers.begin();
  iterator end = observers.end();
  iterator it = begin;
  pair found;
  
  // dodajemy
  for ( it = begin; it != end; ++it ) {
    subject.attach(&*it);
  }

  // pierwszy test
  subject.changeState();
  found = std::equal_range(begin, end, Observer(subject.changeCount));
  CPPUNIT_ASSERT_EQUAL(std::distance(begin, begin), std::distance(begin, found.first));
  CPPUNIT_ASSERT_EQUAL(std::distance(begin, end), std::distance(begin, found.second));

  // drugi
  subject.changeState();
  found = std::equal_range(begin, end, Observer(subject.changeCount));
  CPPUNIT_ASSERT_EQUAL(std::distance(begin, begin), std::distance(begin, found.first));
  CPPUNIT_ASSERT_EQUAL(std::distance(begin, end), std::distance(begin, found.second));

  // wyłączamy dowiązanie w pierwszym
  subject.detach(&observers[0]);
  subject.changeState();
  found = std::equal_range(begin, end, Observer(subject.changeCount));
  CPPUNIT_ASSERT_EQUAL(std::distance(begin, begin+1), std::distance(begin, found.first));
  CPPUNIT_ASSERT_EQUAL(std::distance(begin, end), std::distance(begin, found.second));

  // wyłączamy dowiązanie w kilku kolejnych
  subject.detach(&observers[1]);
  subject.detach(&observers[5]);
  subject.detach(&observers[8]);
  subject.changeState();
  std::sort(begin, end);
  found = std::equal_range(begin, end, Observer(subject.changeCount));
  CPPUNIT_ASSERT_EQUAL(std::distance(begin, begin+4), std::distance(begin, found.first));
  CPPUNIT_ASSERT_EQUAL(std::distance(begin, end), std::distance(begin, found.second));

  // wyłączamy wszystko
  subject.detachAll();
  subject.changeState();
  found = std::equal_range(begin, end, Observer(subject.changeCount));
  CPPUNIT_ASSERT_EQUAL(std::distance(begin, end), std::distance(begin, found.first));
  CPPUNIT_ASSERT_EQUAL(std::distance(begin, end), std::distance(begin, found.second));
}
