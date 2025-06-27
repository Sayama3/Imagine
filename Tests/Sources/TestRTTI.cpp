//
// Created by ianpo on 15/05/2025.
//

#include "GlobalUsefullTests.hpp"

class Something;
using RTTISomething = RTTI<Something>;

#define RTTISomething_DECLARATION(Self, Parent) RTTI_DECLARATION(::Something, Self, Parent)

class Something {
public:
	RTTI_ROOT_DECLARATION(Something);
	Something() {}
	virtual ~Something() {};
};

class SomeOtherThing : public Something {
public:
	RTTISomething_DECLARATION(SomeOtherThing, Something);
	SomeOtherThing() {}
	virtual ~SomeOtherThing() override {};
};

class AnotherThing : public Something {
public:
	RTTISomething_DECLARATION(AnotherThing, Something);
	AnotherThing() {}
	virtual ~AnotherThing() override {};
};

class AnotherSomeOtherThing : public SomeOtherThing {
public:
	RTTISomething_DECLARATION(AnotherSomeOtherThing, SomeOtherThing);
	AnotherSomeOtherThing() {}
	virtual ~AnotherSomeOtherThing() override {};
};

class SomeAnotherSomeOtherThing : public AnotherSomeOtherThing {
public:
	RTTISomething_DECLARATION(SomeAnotherSomeOtherThing, AnotherSomeOtherThing);
	SomeAnotherSomeOtherThing() {}
	virtual ~SomeAnotherSomeOtherThing() override {};
};


TEST(CoreRTTI, HeritageAssertions) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});

	ASSERT_TRUE(Something::rtti.IsRoot());
	ASSERT_EQ(Something::rtti.parent, nullptr);
	ASSERT_STREQ(Something::rtti.type.c_str(), "Something");
	ASSERT_TRUE(Something::rtti.IsA(Something::rtti));
	ASSERT_TRUE(Something::rtti.IsExactlyA(Something::rtti));
	Something _Something{};
	ASSERT_EQ(&(_Something).get_rtti(), &Something::rtti);
	;

	ASSERT_FALSE(SomeOtherThing::rtti.IsRoot());
	ASSERT_EQ(SomeOtherThing::rtti.parent, &Something::rtti);
	ASSERT_TRUE(SomeOtherThing::rtti.IsA(Something::rtti));
	ASSERT_FALSE(SomeOtherThing::rtti.IsExactlyA(Something::rtti));
	SomeOtherThing _SomeOtherThing{};
	ASSERT_EQ(&(_SomeOtherThing).get_rtti(), &SomeOtherThing::rtti);
	;

	ASSERT_FALSE(AnotherThing::rtti.IsRoot());
	ASSERT_EQ(AnotherThing::rtti.parent, &Something::rtti);
	ASSERT_TRUE(AnotherThing::rtti.IsA(Something::rtti));
	ASSERT_FALSE(AnotherThing::rtti.IsExactlyA(Something::rtti));
	ASSERT_FALSE(AnotherThing::rtti.IsA(SomeOtherThing::rtti));
	AnotherThing _AnotherThing{};
	ASSERT_EQ(&(_AnotherThing).get_rtti(), &AnotherThing::rtti);
	;

	ASSERT_FALSE(AnotherSomeOtherThing::rtti.IsRoot());
	ASSERT_EQ(AnotherSomeOtherThing::rtti.parent, &SomeOtherThing::rtti);
	ASSERT_TRUE(AnotherSomeOtherThing::rtti.IsA(Something::rtti));
	ASSERT_TRUE(AnotherSomeOtherThing::rtti.IsA(SomeOtherThing::rtti));
	ASSERT_FALSE(AnotherSomeOtherThing::rtti.IsExactlyA(Something::rtti));
	ASSERT_FALSE(AnotherSomeOtherThing::rtti.IsExactlyA(SomeOtherThing::rtti));
	ASSERT_FALSE(AnotherSomeOtherThing::rtti.IsA(AnotherThing::rtti));
	AnotherSomeOtherThing _AnotherSomeOtherThing{};
	ASSERT_EQ(&(_AnotherSomeOtherThing).get_rtti(), &AnotherSomeOtherThing::rtti);
	;

	ASSERT_FALSE(SomeAnotherSomeOtherThing::rtti.IsRoot());
	ASSERT_EQ(SomeAnotherSomeOtherThing::rtti.parent, &AnotherSomeOtherThing::rtti);
	ASSERT_TRUE(SomeAnotherSomeOtherThing::rtti.IsA(Something::rtti));
	ASSERT_TRUE(SomeAnotherSomeOtherThing::rtti.IsA(SomeOtherThing::rtti));
	ASSERT_TRUE(SomeAnotherSomeOtherThing::rtti.IsA(AnotherSomeOtherThing::rtti));
	ASSERT_FALSE(SomeAnotherSomeOtherThing::rtti.IsExactlyA(Something::rtti));
	ASSERT_FALSE(SomeAnotherSomeOtherThing::rtti.IsExactlyA(SomeOtherThing::rtti));
	ASSERT_FALSE(SomeAnotherSomeOtherThing::rtti.IsA(AnotherThing::rtti));
	SomeAnotherSomeOtherThing _SomeAnotherSomeOtherThing{};
	ASSERT_EQ(&(_SomeAnotherSomeOtherThing).get_rtti(), &SomeAnotherSomeOtherThing::rtti);
	;

	Log::Shutdown();
}


TEST(CoreRTTI, FactoryAssertions) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});

	Something *_Something = RTTISomething::FactoryCreate(Something::rtti.type);
	ASSERT_TRUE(_Something->get_rtti().IsExactlyA(Something::rtti));
	ASSERT_EQ(&_Something->get_rtti(), &Something::rtti);

	Something *__Something = RTTISomething::FactoryCreate(Something::rtti);
	ASSERT_TRUE(__Something->get_rtti().IsExactlyA(Something::rtti));
	ASSERT_EQ(&__Something->get_rtti(), &Something::rtti);

	Something *___Something = RTTISomething::FactoryCreate<Something>();
	ASSERT_TRUE(___Something->get_rtti().IsExactlyA(Something::rtti));
	ASSERT_EQ(&___Something->get_rtti(), &Something::rtti);


	Something *_SomeOtherThing = RTTISomething::FactoryCreate(SomeOtherThing::rtti.type);
	ASSERT_TRUE(_SomeOtherThing->get_rtti().IsExactlyA(SomeOtherThing::rtti));
	ASSERT_EQ(&_SomeOtherThing->get_rtti(), &SomeOtherThing::rtti);

	Something *__SomeOtherThing = RTTISomething::FactoryCreate(SomeOtherThing::rtti);
	ASSERT_TRUE(__SomeOtherThing->get_rtti().IsExactlyA(SomeOtherThing::rtti));
	ASSERT_EQ(&__SomeOtherThing->get_rtti(), &SomeOtherThing::rtti);

	Something *___SomeOtherThing = RTTISomething::FactoryCreate<SomeOtherThing>();
	ASSERT_TRUE(___SomeOtherThing->get_rtti().IsExactlyA(SomeOtherThing::rtti));
	ASSERT_EQ(&___SomeOtherThing->get_rtti(), &SomeOtherThing::rtti);


	Something *_AnotherThing = RTTISomething::FactoryCreate(AnotherThing::rtti.type);
	ASSERT_TRUE(_AnotherThing->get_rtti().IsExactlyA(AnotherThing::rtti));
	ASSERT_EQ(&_AnotherThing->get_rtti(), &AnotherThing::rtti);

	Something *__AnotherThing = RTTISomething::FactoryCreate(AnotherThing::rtti);
	ASSERT_TRUE(__AnotherThing->get_rtti().IsExactlyA(AnotherThing::rtti));
	ASSERT_EQ(&__AnotherThing->get_rtti(), &AnotherThing::rtti);

	Something *___AnotherThing = RTTISomething::FactoryCreate<AnotherThing>();
	ASSERT_TRUE(___AnotherThing->get_rtti().IsExactlyA(AnotherThing::rtti));
	ASSERT_EQ(&___AnotherThing->get_rtti(), &AnotherThing::rtti);


	Something *_AnotherSomeOtherThing = RTTISomething::FactoryCreate(AnotherSomeOtherThing::rtti.type);
	ASSERT_TRUE(_AnotherSomeOtherThing->get_rtti().IsExactlyA(AnotherSomeOtherThing::rtti));
	ASSERT_EQ(&_AnotherSomeOtherThing->get_rtti(), &AnotherSomeOtherThing::rtti);

	Something *__AnotherSomeOtherThing = RTTISomething::FactoryCreate(AnotherSomeOtherThing::rtti);
	ASSERT_TRUE(__AnotherSomeOtherThing->get_rtti().IsExactlyA(AnotherSomeOtherThing::rtti));
	ASSERT_EQ(&__AnotherSomeOtherThing->get_rtti(), &AnotherSomeOtherThing::rtti);

	Something *___AnotherSomeOtherThing = RTTISomething::FactoryCreate<AnotherSomeOtherThing>();
	ASSERT_TRUE(___AnotherSomeOtherThing->get_rtti().IsExactlyA(AnotherSomeOtherThing::rtti));
	ASSERT_EQ(&___AnotherSomeOtherThing->get_rtti(), &AnotherSomeOtherThing::rtti);


	Something *_SomeAnotherSomeOtherThing = RTTISomething::FactoryCreate(SomeAnotherSomeOtherThing::rtti.type);
	ASSERT_TRUE(_SomeAnotherSomeOtherThing->get_rtti().IsExactlyA(SomeAnotherSomeOtherThing::rtti));
	ASSERT_EQ(&_SomeAnotherSomeOtherThing->get_rtti(), &SomeAnotherSomeOtherThing::rtti);

	Something *__SomeAnotherSomeOtherThing = RTTISomething::FactoryCreate(SomeAnotherSomeOtherThing::rtti);
	ASSERT_TRUE(__SomeAnotherSomeOtherThing->get_rtti().IsExactlyA(SomeAnotherSomeOtherThing::rtti));
	ASSERT_EQ(&__SomeAnotherSomeOtherThing->get_rtti(), &SomeAnotherSomeOtherThing::rtti);

	Something *___SomeAnotherSomeOtherThing = RTTISomething::FactoryCreate<SomeAnotherSomeOtherThing>();
	ASSERT_TRUE(___SomeAnotherSomeOtherThing->get_rtti().IsExactlyA(SomeAnotherSomeOtherThing::rtti));
	ASSERT_EQ(&___SomeAnotherSomeOtherThing->get_rtti(), &SomeAnotherSomeOtherThing::rtti);

	Log::Shutdown();
}
