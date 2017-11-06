#include "NamedTreeBaseTest.h"
#include <utils/NamedTree.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( NamedTreeBaseTest );

void NamedTreeBaseTest::setUp()
{
    rootA.reset(new NamedTreeBase("Root A"));
    rootB.reset(new NamedTreeBase("Root B"));
}

void NamedTreeBaseTest::testRootStructure()
{
    CPPUNIT_ASSERT(rootA->getParent() == nullptr);
    CPPUNIT_ASSERT(rootA->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->isRoot() == true);
    CPPUNIT_ASSERT(rootA->isLeaf() == true);
}

void NamedTreeBaseTest::testAddChild()
{
    NamedTreeBasePtr node(new NamedTreeBase("A"));

    //mamy nowego roota
    CPPUNIT_ASSERT(node->getParent() == nullptr);
    CPPUNIT_ASSERT(node->getRoot() == node);
    CPPUNIT_ASSERT(node->size() == 0);
    CPPUNIT_ASSERT(node->isLeaf() == true);
    CPPUNIT_ASSERT(node->isRoot() == true);

    NamedTreeBase::size_type size = rootA->size();

    rootA->addChild(node);

    CPPUNIT_ASSERT(node->getParent() == rootA);
    CPPUNIT_ASSERT(node->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->size() == size + 1);

    CPPUNIT_ASSERT(rootA->getChild("A") == node);
    CPPUNIT_ASSERT(rootA->getChild("A")->size() == 0);
    CPPUNIT_ASSERT(rootA->getChild("A")->isLeaf() == true);
    CPPUNIT_ASSERT(rootA->getChild("A")->isRoot() == false);
    CPPUNIT_ASSERT(rootA->getChild(0) == node);
}

void NamedTreeBaseTest::testAddChildPose()
{
    NamedTreeBasePtr node(new NamedTreeBase("B"));    
    NamedTreeBase::size_type size = rootA->size();

    rootA->addChild(node);

    CPPUNIT_ASSERT(node->getParent() == rootA);
    CPPUNIT_ASSERT(node->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->size() == size + 1);

    CPPUNIT_ASSERT(rootA->getChild("B") == rootA->getChild(size));
    CPPUNIT_ASSERT(rootA->getChild("B")->size() == 0);
    CPPUNIT_ASSERT(rootA->getChild("B")->isLeaf() == true);
    CPPUNIT_ASSERT(rootA->getChild("B")->isRoot() == false);

    node.reset(new NamedTreeBase("C"));
    //nadpisz
    rootA->addChild(node,size);

    CPPUNIT_ASSERT(node->getParent() == rootA);
    CPPUNIT_ASSERT(node->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->size() == size + 1);

    CPPUNIT_ASSERT(rootA->getChild("C") == rootA->getChild(size));
    CPPUNIT_ASSERT(rootA->getChild("C")->size() == 0);
    CPPUNIT_ASSERT(rootA->getChild("C")->isLeaf() == true);
    CPPUNIT_ASSERT(rootA->getChild("C")->isRoot() == false);
}

void NamedTreeBaseTest::testAddChildExist()
{
    NamedTreeBasePtr node(new NamedTreeBase("D"));
    rootA->addChild(node);
    //wyjatek
    rootA->addChild(node);
}

void NamedTreeBaseTest::testAddChildWithParent()
{
    NamedTreeBasePtr node(new NamedTreeBase("E"));
    rootA->addChild(node);
    //wyjatek
    rootB->addChild(node);
}

void NamedTreeBaseTest::testAddChildName()
{
    NamedTreeBase::size_type size = rootA->size();

    rootA->addChild("F");

    CPPUNIT_ASSERT(rootA->size() == size + 1);
    CPPUNIT_ASSERT(rootA->getChild(size) == rootA->getChild("F"));
    CPPUNIT_ASSERT(rootA->getChild("F")->getParent() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("F")->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("F")->isLeaf() == true);
    CPPUNIT_ASSERT(rootA->getChild("F")->isRoot() == false);
}

void NamedTreeBaseTest::testAddChildNamePose()
{
    NamedTreeBase::size_type size = 0;

    rootA->addChild("G");

    CPPUNIT_ASSERT(rootA->size() == size + 1);
    CPPUNIT_ASSERT(rootA->getChild(size) == rootA->getChild("G"));
    CPPUNIT_ASSERT(rootA->getChild("G")->getParent() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("G")->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("G")->isLeaf() == true);
    CPPUNIT_ASSERT(rootA->getChild("G")->isRoot() == false);

    //nadpisz
    rootA->addChild("H", size);

    //rozmiar nie powinien sie zwiekszyc
    CPPUNIT_ASSERT(rootA->size() == size + 1);
    CPPUNIT_ASSERT(rootA->getChild(size) == rootA->getChild("H"));
    CPPUNIT_ASSERT(rootA->getChild("H")->getParent() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("H")->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("H")->isLeaf() == true);
    CPPUNIT_ASSERT(rootA->getChild("H")->isRoot() == false);
}

void NamedTreeBaseTest::testAddChildNameExist()
{
    rootA->addChild("I");
    //wyjatek
    rootA->addChild("I");
}

void NamedTreeBaseTest::testAddChildPath()
{
    NamedTreeBase::size_type size = rootA->size();

    rootA->addChild("E/F/G");

    CPPUNIT_ASSERT(rootA->size() == size + 1);

    CPPUNIT_ASSERT(rootA->getChild("E")->size() == 1);
    CPPUNIT_ASSERT(rootA->getChild("E/F")->size() == 1);
    CPPUNIT_ASSERT(rootA->getChild("E/F/G")->size() == 0);

    CPPUNIT_ASSERT(rootA->getChild("E")->isLeaf() == false);
    CPPUNIT_ASSERT(rootA->getChild("E/F")->isLeaf() == false);
    CPPUNIT_ASSERT(rootA->getChild("E/F/G")->isLeaf() == true);

    CPPUNIT_ASSERT(rootA->getChild("E")->isRoot() == false);
    CPPUNIT_ASSERT(rootA->getChild("E/F")->isRoot() == false);
    CPPUNIT_ASSERT(rootA->getChild("E/F/G")->isRoot() == false);

    CPPUNIT_ASSERT(rootA->getChild("E")->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("E/F")->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("E/F/G")->getRoot() == rootA);

    CPPUNIT_ASSERT(rootA->getChild("E")->getParent() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("E/F")->getParent() == rootA->getChild("E"));
    CPPUNIT_ASSERT(rootA->getChild("E/F/G")->getParent() == rootA->getChild("E/F"));
}

void NamedTreeBaseTest::testAddChildPathPose()
{
    NamedTreeBase::size_type size = 0;
    
    rootA->addChild("E/F/G");
    //nadpisz
    rootA->addChild("E/F/H", 0);

    CPPUNIT_ASSERT(rootA->size() == 1);

    CPPUNIT_ASSERT(rootA->getChild("E/F")->size() == 1);
    CPPUNIT_ASSERT(rootA->getChild("E/F")->getChild(size) == rootA->getChild("E/F/H"));
    CPPUNIT_ASSERT(rootA->getChild("E/F")->getChild("H")->getParent() == rootA->getChild("E/F"));
    CPPUNIT_ASSERT(rootA->getChild("E/F")->getChild("H")->getRoot() == rootA);
}

void NamedTreeBaseTest::testAddChildPathExist()
{
    rootA->addChild("E/F/G");
    //wyjatek
    rootA->addChild("E/F/G");
}

void NamedTreeBaseTest::testAddChildPathBeyondRoot()
{
    //wyjatek
    rootA->addChild("../Up");
}

void NamedTreeBaseTest::testRemoveChild()
{
    rootA->addChild("A");
    NamedTreeBasePtr node = rootA->getChild(0);

    //mamy nowego roota
    CPPUNIT_ASSERT(node->getParent() == rootA);
    CPPUNIT_ASSERT(node->getRoot() == rootA);
    CPPUNIT_ASSERT(node->size() == 0);
    CPPUNIT_ASSERT(node->isLeaf() == true);
    CPPUNIT_ASSERT(node->isRoot() == false);

    NamedTreeBase::size_type size = rootA->size();

    rootA->removeChild(node);

    CPPUNIT_ASSERT(rootA->size() == size - 1);

    CPPUNIT_ASSERT(node->getParent() == nullptr);
    CPPUNIT_ASSERT(node->getRoot() == node);
    CPPUNIT_ASSERT(node->size() == 0);
    CPPUNIT_ASSERT(node->isLeaf() == true);
    CPPUNIT_ASSERT(node->isRoot() == true);
}

void NamedTreeBaseTest::testRemoveChildPose()
{
    rootA->addChild("A");
    NamedTreeBase::size_type size = rootA->size();
    NamedTreeBasePtr node = rootA->getChild(0);

    CPPUNIT_ASSERT(node->getParent() == rootA);
    CPPUNIT_ASSERT(node->getRoot() == rootA);
    CPPUNIT_ASSERT(node->size() == 0);
    CPPUNIT_ASSERT(node->isLeaf() == true);
    CPPUNIT_ASSERT(node->isRoot() == false);

    rootA->removeChild(0);

    CPPUNIT_ASSERT(rootA->size() == size - 1);
    CPPUNIT_ASSERT(node->getParent() == nullptr);
    CPPUNIT_ASSERT(node->getRoot() == node);
    CPPUNIT_ASSERT(node->size() == 0);
    CPPUNIT_ASSERT(node->isLeaf() == true);
    CPPUNIT_ASSERT(node->isRoot() == true);
}

void NamedTreeBaseTest::testRemoveChildNotExist()
{
    NamedTreeBasePtr node(new NamedTreeBase("someNode"));
    //wyjatek
    rootA->removeChild(node);
}

void NamedTreeBaseTest::testRemoveChildName()
{
    rootA->addChild("A");
    NamedTreeBasePtr node = rootA->getChild("A");
    NamedTreeBase::size_type size = rootA->size();

    CPPUNIT_ASSERT(node->getParent() == rootA);
    CPPUNIT_ASSERT(node->getRoot() == rootA);
    CPPUNIT_ASSERT(node->size() == 0);
    CPPUNIT_ASSERT(node->isLeaf() == true);
    CPPUNIT_ASSERT(node->isRoot() == false);

    rootA->removeChild("A");

    CPPUNIT_ASSERT(rootA->size() == size - 1);
    CPPUNIT_ASSERT(node->getParent() == nullptr);
    CPPUNIT_ASSERT(node->getRoot() == node);
    CPPUNIT_ASSERT(node->size() == 0);
    CPPUNIT_ASSERT(node->isLeaf() == true);
    CPPUNIT_ASSERT(node->isRoot() == true);
}

void NamedTreeBaseTest::testRemoveChildNameNotExist()
{
    //wyjatek
    rootA->removeChild("someChild");
}

void NamedTreeBaseTest::testRemoveChildPath()
{
    rootA->addChild("E/F/G");
    rootA->addChild("E/F/H");
    NamedTreeBasePtr node = rootA->getChild("E/F");
    NamedTreeBase::size_type size = rootA->getChild("E")->size();

    CPPUNIT_ASSERT(node->getParent() == rootA->getChild("E"));
    CPPUNIT_ASSERT(node->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("E")->isLeaf() == false);
    CPPUNIT_ASSERT(rootA->getChild("E")->isRoot() == false);
    CPPUNIT_ASSERT(rootA->getChild("E")->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("E")->getParent() == rootA);

    rootA->removeChild("E/F");

    CPPUNIT_ASSERT(rootA->getChild("E")->size() == size - 1);
    CPPUNIT_ASSERT(node->getParent() == nullptr);
    CPPUNIT_ASSERT(node->getRoot() == node);
    CPPUNIT_ASSERT(rootA->getChild("E")->isLeaf() == true);
    CPPUNIT_ASSERT(rootA->getChild("E")->isRoot() == false);
    CPPUNIT_ASSERT(rootA->getChild("E")->getRoot() == rootA);
    CPPUNIT_ASSERT(rootA->getChild("E")->getParent() == rootA);
}

void NamedTreeBaseTest::testGetChildPathNotExist()
{
    //wyjatek
    rootA->removeChild("W/W/W/W/W/W/W/W/W");
}

void NamedTreeBaseTest::testRemoveChildPathBeyondRoot()
{
    //wyjatek
    rootA->removeChild("../someNode");
}

