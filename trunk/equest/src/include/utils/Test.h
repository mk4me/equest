/********************************************************************
	created:	2015/01/22
	created:	14:10
	filename: 	Test.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__Test_H__
#define HEADER_GUARD_UTILS__Test_H__

//! Umożliwia testowanie pól prywatnych
#ifdef UTILS_ENABLE_PRIVATE_TESTS
#   define ENABLE_PRIVATE_TESTS                 \
        template<class _TestClass>              \
        static void _privateTestHook(_TestClass& x);    
#else
#   define ENABLE_PRIVATE_TESTS
#endif

#endif // HEADER_GUARD 
