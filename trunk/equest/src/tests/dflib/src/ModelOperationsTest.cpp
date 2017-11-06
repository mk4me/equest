#include "ModelOperationsTest.h"
#include <dflib/Model.h>
#include <dflib/Connection.h>
#include "IntProcessor.h"
#include "IntSource.h"
#include "IntSink.h"
#include <dflib/DFModelRunner.h>
#include <boost/shared_ptr.hpp>
#include <cmath>
#include "dflib/STDFModelRunner.h"

//#include <threading/QtThreadFactory.h>
//#include <threading/ThreadPool.h>

typedef boost::shared_ptr<df::Model> ModelPtr;
typedef boost::shared_ptr<IntSource> SourcePtr;
typedef boost::shared_ptr<IntSink> SinkPtr;
typedef boost::shared_ptr<IntProcessor> ProcessorPtr;
typedef boost::shared_ptr<df::Connection> ConnectionPtr;

CPPUNIT_TEST_SUITE_REGISTRATION( ModelOperationsTest );

int addition(int a, int b)
{
	return a + b;
}

int subtraction(int a, int b)
{
	return a - b;
}

int multiplication(int a, int b)
{
	return a * b;
}

int division(int a, int b)
{
	return a / b;
}

ModelOperationsTest::ModelOperationsTest(void) : TestFixture()
{
}


ModelOperationsTest::~ModelOperationsTest(void)
{
}

void ModelOperationsTest::testBasicNodeAddition()
{
	ModelPtr model(new df::Model());
	SourcePtr source(new IntSource(IntSource::Data()));
	auto p = source->outputPin(0);
    model->addNode(source.get());

	SinkPtr sink(new IntSink());
	model->addNode(sink.get());

	ProcessorPtr proc(new IntProcessor(IntProcessor::Func(&addition)));
	model->addNode(proc.get());
   
    CPPUNIT_ASSERT( (source->model() == model.get()) && (sink->model() == model.get()) && (proc->model() == model.get()));
    CPPUNIT_ASSERT( model->nodesSize() == 3 );
    CPPUNIT_ASSERT( model->sourcesSize() == 1 );
	CPPUNIT_ASSERT( model->sinksSize() == 1 );
	CPPUNIT_ASSERT( model->processorsSize() == 1 );
    CPPUNIT_ASSERT( model->connectionsSize() == 0 );
}

void ModelOperationsTest::testSameNodeAddition()
{
	ModelPtr model(new df::Model());
	SourcePtr source(new IntSource(IntSource::Data()));
	model->addNode(source.get());
	model->addNode(source.get());
}

void ModelOperationsTest::testOtherModelNodeAddition()
{
	ModelPtr modelA(new df::Model());
	SourcePtr sourceA(new IntSource(IntSource::Data()));
	modelA->addNode(sourceA.get());
	ModelPtr modelB(new df::Model());
	SourcePtr sourceB(new IntSource(IntSource::Data()));
	modelB->addNode(sourceB.get());
	modelA->addNode(sourceB.get());
}

void ModelOperationsTest::testBasicNodeRemove()
{
	ModelPtr model(new df::Model());
	SourcePtr source(new IntSource(IntSource::Data()));
	model->addNode(source.get());

	SinkPtr sink(new IntSink());
	model->addNode(sink.get());

	ProcessorPtr proc(new IntProcessor(IntProcessor::Func(&addition)));
	model->addNode(proc.get());

	CPPUNIT_ASSERT( (source->model() == model.get()) && (sink->model() == model.get()) && (proc->model() == model.get()));
	CPPUNIT_ASSERT( model->nodesSize() == 3 );
	CPPUNIT_ASSERT( model->sourcesSize() == 1 );
	CPPUNIT_ASSERT( model->sinksSize() == 1 );
	CPPUNIT_ASSERT( model->processorsSize() == 1 );
	CPPUNIT_ASSERT( model->connectionsSize() == 0 );

	model->removeNode(proc.get());
	CPPUNIT_ASSERT( proc->model() == nullptr );
	CPPUNIT_ASSERT( model->nodesSize() == 2 );
	CPPUNIT_ASSERT( model->sourcesSize() == 1 );
	CPPUNIT_ASSERT( model->sinksSize() == 1 );
	CPPUNIT_ASSERT( model->processorsSize() == 0 );
	CPPUNIT_ASSERT( model->connectionsSize() == 0 );

	model->removeNode(sink.get());
	CPPUNIT_ASSERT( sink->model() == nullptr );
	CPPUNIT_ASSERT( model->nodesSize() == 1 );
	CPPUNIT_ASSERT( model->sourcesSize() == 1 );
	CPPUNIT_ASSERT( model->sinksSize() == 0 );
	CPPUNIT_ASSERT( model->processorsSize() == 0 );
	CPPUNIT_ASSERT( model->connectionsSize() == 0 );

	model->removeNode(source.get());
	CPPUNIT_ASSERT( source->model() == nullptr );
	CPPUNIT_ASSERT( model->nodesSize() == 0 );
	CPPUNIT_ASSERT( model->sourcesSize() == 0 );
	CPPUNIT_ASSERT( model->sinksSize() == 0 );
	CPPUNIT_ASSERT( model->processorsSize() == 0 );
	CPPUNIT_ASSERT( model->connectionsSize() == 0 );

}

void ModelOperationsTest::testOtherModelNodeRemove()
{
	ModelPtr modelA(new df::Model());
	SourcePtr sourceA(new IntSource(IntSource::Data()));
	modelA->addNode(sourceA.get());
	ModelPtr modelB(new df::Model());
	SourcePtr sourceB(new IntSource(IntSource::Data()));
	modelB->addNode(sourceB.get());
	modelA->removeNode(sourceB.get());
}

void ModelOperationsTest::testBasicConnecting()
{
	ModelPtr model(new df::Model());
	SourcePtr source(new IntSource(IntSource::Data()));
	model->addNode(source.get());

	ProcessorPtr proc(new IntProcessor(IntProcessor::Func(&addition)));
	model->addNode(proc.get());

	CPPUNIT_ASSERT( (source->model() == model.get()) && (proc->model() == model.get()));
	CPPUNIT_ASSERT( model->nodesSize() == 2 );
	CPPUNIT_ASSERT( model->sourcesSize() == 1 );
	CPPUNIT_ASSERT( model->sinksSize() == 0 );
	CPPUNIT_ASSERT( model->processorsSize() == 1 );
	CPPUNIT_ASSERT( model->connectionsSize() == 0 );

    CPPUNIT_ASSERT(model->canConnect(source->outputPin(0), proc->inputPin(0)) == true);
	ConnectionPtr conn(new df::Connection(source->outputPin(0), proc->inputPin(0)));
	model->addConnection(conn.get());

	CPPUNIT_ASSERT(model->connectionsSize() == 1);
	CPPUNIT_ASSERT(source->outputPin(0)->connectionsSize() == 1);
	CPPUNIT_ASSERT(proc->inputPin(0)->connectionsSize() == 1);
	CPPUNIT_ASSERT(source->connected() == true);
	CPPUNIT_ASSERT(proc->connected() == true);
	CPPUNIT_ASSERT(source->connectedPins() == 1);
	CPPUNIT_ASSERT(proc->connectedPins() == 1);
	CPPUNIT_ASSERT(source->outputsConnected() == 1);
	CPPUNIT_ASSERT(proc->inputsConnected() == 1);
}

void ModelOperationsTest::testDuplicatedConnecting()
{
	ModelPtr model(new df::Model());
	SourcePtr source(new IntSource(IntSource::Data()));
	model->addNode(source.get());

	ProcessorPtr proc(new IntProcessor(IntProcessor::Func(&addition)));
	model->addNode(proc.get());

	ConnectionPtr conn(new df::Connection(source->outputPin(0), proc->inputPin(0)));

	model->addConnection(conn.get());
	model->addConnection(conn.get());
}

void ModelOperationsTest::testDifferentModelsConnecting()
{
	ModelPtr modelA(new df::Model());
	SourcePtr source(new IntSource(IntSource::Data()));
	modelA->addNode(source.get());

	ModelPtr modelB(new df::Model());
	ProcessorPtr proc(new IntProcessor(IntProcessor::Func(&addition)));
	modelB->addNode(proc.get());

	ConnectionPtr conn(new df::Connection(source->outputPin(0), proc->inputPin(0)));
	modelA->addConnection(conn.get());
}

void ModelOperationsTest::testConnectionRemove()
{
	ModelPtr model(new df::Model());
	SourcePtr source(new IntSource(IntSource::Data()));
	model->addNode(source.get());

	ProcessorPtr proc(new IntProcessor(IntProcessor::Func(&addition)));
	model->addNode(proc.get());

	ConnectionPtr conn(new df::Connection(source->outputPin(0), proc->inputPin(0)));
	model->addConnection(conn.get());

	model->removeConnection(conn.get());

	CPPUNIT_ASSERT(model->connectionsSize() == 0);
	CPPUNIT_ASSERT(source->outputPin(0)->connectionsSize() == 0);
	CPPUNIT_ASSERT(proc->inputPin(0)->connectionsSize() == 0);
	CPPUNIT_ASSERT(source->connected() == false);
	CPPUNIT_ASSERT(proc->connected() == false);
	CPPUNIT_ASSERT(source->connectedPins() == 0);
	CPPUNIT_ASSERT(proc->connectedPins() == 0);
	CPPUNIT_ASSERT(source->outputsConnected() == 0);
	CPPUNIT_ASSERT(proc->inputsConnected() == 0);
}

void ModelOperationsTest::testDifferentModelConnectionRemove()
{
	ModelPtr modelA(new df::Model());
	SourcePtr source(new IntSource(IntSource::Data()));
	modelA->addNode(source.get());

	ProcessorPtr proc(new IntProcessor(IntProcessor::Func(&addition)));
	modelA->addNode(proc.get());

	ConnectionPtr conn(new df::Connection(source->outputPin(0), proc->inputPin(0)));
	modelA->removeConnection(conn.get());

}

void ModelOperationsTest::testSimplestModel()
{
	IntSource::Data dataA;
	dataA.push_back(123);

	ModelPtr model(new df::Model());
	// Ÿró³a
	SourcePtr sourceA(new IntSource(dataA, "A"));
	model->addNode(sourceA.get());

	// sinki
	SinkPtr sinkA(new IntSink("A"));
	model->addNode(sinkA.get());
		
	//³¹czymy
	//Ÿród³o A z mno¿eniem i dodawaniam
	ConnectionPtr connAMul(new df::Connection(sourceA->outputPin(0), sinkA->inputPin(0)));
	model->addConnection(connAMul.get());
	
	df::STDFModelRunner runner;
	
	//startujemy model
	runner.start(model.get(), nullptr);

	//porównyjemy wyniki - zawartoœæ sinków i Ÿróde³ powinna byæ taka sama
	bool equalA = dataA.size() == sinkA->data().size();

	if (equalA == true) {
		for (auto i = 0; i < dataA.size(); ++i) {
			if (dataA[i] != sinkA->data()[i]) {
				equalA = false;
				break;
			}
		}
	}
	CPPUNIT_ASSERT(equalA);

}

void ModelOperationsTest::testSingleThreadModelRunning()
{
	IntSource::Data dataA;
	IntSource::Data dataB;

	for (int i = 0; i < 1000; ++i) {
		dataA.push_back(1 + (rand() % 1000));
	}

	for (int i = 0; i < 1000; ++i) {
		dataB.push_back(1 + (rand() % 1000));
	}

	ModelPtr model(new df::Model());
	// Ÿró³a
	SourcePtr sourceA(new IntSource(dataA, "A"));
	model->addNode(sourceA.get());

	SourcePtr sourceB(new IntSource(dataB, "B"));
	model->addNode(sourceB.get());

	// elementy przetwarzaj¹ce
	ProcessorPtr procAdd(new IntProcessor(IntProcessor::Func(&addition), "Add"));
	model->addNode(procAdd.get());

	ProcessorPtr procSub(new IntProcessor(IntProcessor::Func(&subtraction), "Sub"));
	model->addNode(procSub.get());

	ProcessorPtr procMul(new IntProcessor(IntProcessor::Func(&multiplication), "Mul"));
	model->addNode(procMul.get());

	ProcessorPtr procDiv(new IntProcessor(IntProcessor::Func(&division), "Div"));
	model->addNode(procDiv.get());

	// sinki
	SinkPtr sinkA(new IntSink("A"));
	model->addNode(sinkA.get());

	SinkPtr sinkB(new IntSink("B"));
	model->addNode(sinkB.get());

	//³¹czymy
	//Ÿród³o A z mno¿eniem i dodawaniam
	ConnectionPtr connAMul(new df::Connection(sourceA->outputPin(0), procMul->inputPin(0)));
	ConnectionPtr connAAdd(new df::Connection(sourceA->outputPin(0), procAdd->inputPin(0)));

	model->addConnection(connAMul.get());
	model->addConnection(connAAdd.get());

	//Ÿród³o B z mno¿eniem i dodawaniam
	ConnectionPtr connBMul(new df::Connection(sourceB->outputPin(0), procMul->inputPin(1)));
	ConnectionPtr connBAdd(new df::Connection(sourceB->outputPin(0), procAdd->inputPin(1)));

	model->addConnection(connBMul.get());
	model->addConnection(connBAdd.get());

	//Ÿród³o B z dzieleniem
	ConnectionPtr connBDiv(new df::Connection(sourceB->outputPin(0), procDiv->inputPin(1)));
	model->addConnection(connBDiv.get());

	//Ÿród³o A z odejmowaniem
	ConnectionPtr connASub(new df::Connection(sourceA->outputPin(0), procSub->inputPin(1)));
	model->addConnection(connASub.get());

	//mnozenie z dzieleniem
	ConnectionPtr connMulDiv(new df::Connection(procMul->outputPin(0), procDiv->inputPin(0)));
	model->addConnection(connMulDiv.get());

	//dodawanie z odejmowaniem
	ConnectionPtr connAddSub(new df::Connection(procAdd->outputPin(0), procSub->inputPin(0)));
	model->addConnection(connAddSub.get());

	//podpinamy sinki do dzielenia i odejmowania
	ConnectionPtr connDivA(new df::Connection(procDiv->outputPin(0), sinkA->inputPin(0)));
	model->addConnection(connDivA.get());

	ConnectionPtr connSubB(new df::Connection(procSub->outputPin(0), sinkB->inputPin(0)));
	model->addConnection(connSubB.get());

	df::STDFModelRunner runner;



	//startujemy model
	runner.start(model.get(), nullptr);

	//porównyjemy wyniki - zawartoœæ sinków i Ÿróde³ powinna byæ taka sama

	bool equalA = dataA.size() == sinkA->data().size();

	if (equalA == true) {
		for (auto i = 0; i < dataA.size(); ++i) {
			if (dataA[i] != sinkA->data()[i]) {
				equalA = false;
				break;
			}
		}
	}

	bool equalB = dataB.size() == sinkB->data().size();

	if (equalB == true) {
		for (auto i = 0; i < dataB.size(); ++i) {
			if (dataB[i] != sinkB->data()[i]) {
				equalB = false;
				break;
			}
		}
	}

	CPPUNIT_ASSERT(equalA && equalB);

}

//void ModelOperationsTest::testModelRunning()
//{
//	utils::IThreadFactoryPtr tf(new utils::QtThreadFactory);
//	utils::IThreadPoolPtr tp(new utils::ThreadPool(tf));
//
//
//	IntSource::Data dataA;
//	IntSource::Data dataB;
//
//	for(int i = 0; i < 1000; ++i)
//	{
//		dataA.push_back(1 + (rand() % 1000));
//	}
//
//	for(int i = 0; i < 1000; ++i)
//	{
//		dataB.push_back(1 + (rand() % 1000));
//	}
//
//	ModelPtr model(new df::Model());
//	// Ÿró³a
//	SourcePtr sourceA(new IntSource(dataA, "A"));
//	model->addNode(sourceA.get());
//
//	SourcePtr sourceB(new IntSource(dataB, "B"));
//	model->addNode(sourceB.get());
//
//	// elementy przetwarzaj¹ce
//	ProcessorPtr procAdd(new IntProcessor(IntProcessor::Func(&addition), "Add"));
//	model->addNode(procAdd.get());
//
//	ProcessorPtr procSub(new IntProcessor(IntProcessor::Func(&subtraction), "Sub"));
//	model->addNode(procSub.get());
//
//	ProcessorPtr procMul(new IntProcessor(IntProcessor::Func(&multiplication), "Mul"));
//	model->addNode(procMul.get());
//
//	ProcessorPtr procDiv(new IntProcessor(IntProcessor::Func(&division), "Div"));
//	model->addNode(procDiv.get());
//
//	// sinki
//	SinkPtr sinkA(new IntSink("A"));
//	model->addNode(sinkA.get());
//
//	SinkPtr sinkB(new IntSink("B"));
//	model->addNode(sinkB.get());
//
//	//³¹czymy
//	//Ÿród³o A z mno¿eniem i dodawaniam
//	ConnectionPtr connAMul(new df::Connection(sourceA->outputPin(0), procMul->inputPin(0)));
//	ConnectionPtr connAAdd(new df::Connection(sourceA->outputPin(0), procAdd->inputPin(0)));
//
//	model->addConnection(connAMul.get());
//	model->addConnection(connAAdd.get());
//
//	//Ÿród³o B z mno¿eniem i dodawaniam
//	ConnectionPtr connBMul(new df::Connection(sourceB->outputPin(0), procMul->inputPin(1)));
//	ConnectionPtr connBAdd(new df::Connection(sourceB->outputPin(0), procAdd->inputPin(1)));
//
//	model->addConnection(connBMul.get());
//	model->addConnection(connBAdd.get());
//
//	//Ÿród³o B z dzieleniem
//	ConnectionPtr connBDiv(new df::Connection(sourceB->outputPin(0), procDiv->inputPin(1)));
//	model->addConnection(connBDiv.get());
//
//	//Ÿród³o A z odejmowaniem
//	ConnectionPtr connASub(new df::Connection(sourceA->outputPin(0), procSub->inputPin(1)));
//	model->addConnection(connASub.get());
//
//	//mnozenie z dzieleniem
//	ConnectionPtr connMulDiv(new df::Connection(procMul->outputPin(0), procDiv->inputPin(0)));
//	model->addConnection(connMulDiv.get());
//	
//	//dodawanie z odejmowaniem
//	ConnectionPtr connAddSub(new df::Connection(procAdd->outputPin(0), procSub->inputPin(0)));
//	model->addConnection(connAddSub.get());
//
//	//podpinamy sinki do dzielenia i odejmowania
//	ConnectionPtr connDivA(new df::Connection(procDiv->outputPin(0), sinkA->inputPin(0)));
//	model->addConnection(connDivA.get());
//
//	ConnectionPtr connSubB(new df::Connection(procSub->outputPin(0), sinkB->inputPin(0)));
//	model->addConnection(connSubB.get());
//
//	df::DFModelRunner runner;
//
//	
//
//	//startujemy model
//	runner.start(model.get(), nullptr, tp.get());
//	runner.join();
//
//	//porównyjemy wyniki - zawartoœæ sinków i Ÿróde³ powinna byæ taka sama
//
//	bool equalA = dataA.size() == sinkA->data().size();
//
//	if(equalA == true)
//	{
//		for(auto i = 0; i < dataA.size(); ++i)
//		{
//			if(dataA[i] != sinkA->data()[i])
//			{
//				equalA = false;
//				break;
//			}
//		}
//	}
//
//	bool equalB = dataB.size() == sinkB->data().size();
//
//	if(equalB == true)
//	{
//		for(auto i = 0; i < dataB.size(); ++i)
//		{
//			if(dataB[i] != sinkB->data()[i])
//			{
//				equalB = false;
//				break;
//			}
//		}
//	}
//
//	CPPUNIT_ASSERT( equalA && equalB );
//}
