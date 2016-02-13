#ifndef HUSH_UNITTEST_H
#define HUSH_UNITTEST_H

#include <string>
#include <iostream>
#include "..\..\Common\BasicType.h"

namespace Hush
{
    namespace UnitTest
    {        
        using namespace std;
        class Assert;

        class ITestClass
        {
        public:
            virtual void RunTests() = 0;
        };

        struct TestClassInfo;
        class TestRunner
        {
        public:
            static void AddTestClass(TestClassInfo* info);
            static void RunTests();

        private:
            static TestClassInfo* headTestClassInfo;
            static TestClassInfo* tailTestClassInfo;

        };
                
        struct TestClassInfo
        {
            TestClassInfo()
                : next(0)
            {
                TestRunner::AddTestClass(this);
            }

            virtual ITestClass& GetTestClassInstance() = 0;
            virtual const Char* GetClassName() = 0;

            void RunTests()
            {
                wcout << L"+ Running tests in class [" <<GetClassName() <<"]" <<endl;
                GetTestClassInstance().RunTests();
            }

            TestClassInfo* next;
        };

        template <typename ClassType>
        struct TestClassRegistrator : TestClassInfo
        {
            virtual ITestClass& GetTestClassInstance()
            {
                return TestClassRegistrator::instance;
            }

            static ClassType instance;
        };

        template<typename ClassType>
        ClassType TestClassRegistrator<ClassType>::instance;

        template <typename T>
        class TestClassBase : public ITestClass
        {
        protected:
            template<typename ClassType>
            struct TestMethodInfo
            {
                TestMethodInfo() : next(0)
                {
                    ClassType::AddTest(this);
                }
                virtual void RunTest(ClassType* thisPointer) = 0;
                virtual const Char* GetMethodName() = 0;
                TestMethodInfo<ClassType>* next;
            };

        public:
            typedef T ClassType;

            static void AddTest(TestMethodInfo<ClassType>* info)
            {                
                if (!headTestMethodInfo)
                {
                    // This is the first node
                    ClassType::headTestMethodInfo = info;
                    ClassType::tailTestMethodInfo = info;
                }
                else
                {
                    // Add the new node to the tail
                    tailTestMethodInfo->next = info;
                    tailTestMethodInfo = info;
                }    
            }

        private:            
            static TestMethodInfo<ClassType>* headTestMethodInfo;
            static TestMethodInfo<ClassType>* tailTestMethodInfo;            

            static void RunTests(ClassType* thisPointer)
            {
                thisPointer->ClassInitialize();
                TestMethodInfo<ClassType>* info = ClassType::headTestMethodInfo;
                while (info)
                {
                    wcout << L"  - Running test [" << info->GetMethodName() << "]" <<endl;
                    thisPointer->TestInitialize();
                    info->RunTest(thisPointer);
                    thisPointer->TestCleanup();
                    info = info->next;
                }
                thisPointer->ClassCleanup();
            }

        public:
            virtual void RunTests()
            {
                RunTests((ClassType*)this);
            }

        protected:            
            virtual void ClassInitialize() {} 
            virtual void ClassCleanup() {}
            virtual void TestInitialize() {}
            virtual void TestCleanup() {}
        };
        
        template <typename T>
        typename TestClassBase<T>::TestMethodInfo<T>* TestClassBase<T>::headTestMethodInfo = 0;
        template <typename T>
        typename TestClassBase<T>::TestMethodInfo<T>* TestClassBase<T>::tailTestMethodInfo = 0;


#define TESTCLASS(ClassName) \
        class ClassName; \
        struct ClassName##Registrator : public TestClassRegistrator<ClassName> \
        {    virtual const Hush::Char* GetClassName() { return L#ClassName; } }; \
        ClassName##Registrator ClassName##Instance; \
        class ClassName : public TestClassBase<ClassName> \

#define TESTMETHOD(MethodName) \
        struct TestMethodInfo##MethodName : public TestMethodInfo<ClassType> \
        { virtual void RunTest(ClassType* thisPointer) {    thisPointer->MethodName(); } \
          const Hush::Char* GetMethodName() { return L#MethodName; } \
        };    \
        TestMethodInfo##MethodName MethodName##MethodInfo; \
        virtual void MethodName() \

#define EXPECTEXCEPTION_INTERNAL_CORE(ExceptionType,MethodPostfix) \
        { \
        Bool hasException = false;  \
        try { ExceptionWrapper##MethodPostfix(); } \
        catch (ExceptionType&) { hasException = true; } \
        Assert::IsTrue(hasException, wstring(L"Expected exception of type ") + wstring(L#ExceptionType)); \
        } \
        virtual void ExceptionWrapper##MethodPostfix()

#define EXPECTEXCEPTION_INTERNAL(ExceptionType,MethodPostfix)   EXPECTEXCEPTION_INTERNAL_CORE(ExceptionType, MethodPostfix) 

#define EXPECTEXCEPTION(ExceptionType)   EXPECTEXCEPTION_INTERNAL(ExceptionType, __LINE__)  

        class Assert
        {
        public:
            static void IsTrue(bool condition, const wstring& message = wstring());

            template <typename T>
            static void IsNotNull(const T* pointer,  const wstring& message = wstring())
            {
                if (!pointer)
                {
                    throw message;
                }
            }

            template <typename T>
            static void AreEqual(const T& expected, const T& actual, const wstring& message = wstring())
            {
                if (!(expected == actual))
                {
                    throw message;
                }
            }

            template <typename T>
            static void AreNotEqual(const T& notExpected, const T& actual, const wstring& message = wstring())
            {
                if (notExpected == actual)
                {
                    throw message;
                }
            }

            template <typename T>
            static void Fail(const wstring& message = wstring())
            {
                throw message;
            }
        };
    }
}



#endif