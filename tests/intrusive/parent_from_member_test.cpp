#include "catch.hpp"
#include <boost/intrusive/parent_from_member.hpp>

struct POD
{
	int   int_;
	float float_;
}pod;

struct Derived
	: public POD
{
	int   derived_int_;
	float derived_float_;
}derived;

struct Abstract
{
	int   abstract_int_;
	float abstract_float_;
	virtual void virtual_func1() = 0;
	virtual void virtual_func2() = 0;
	virtual ~Abstract() {}
};

struct DerivedPoly
	: public Abstract
{
	int   derivedpoly_int_;
	float derivedpoly_float_;
	virtual void virtual_func1() {}
	virtual void virtual_func2() {}
	Abstract* abstract() { return this; }
	Abstract const* abstract() const { return this; }
} derivedpoly;

struct MultiInheritance
	: public Derived, public DerivedPoly
{
	int   multiinheritance_int_;
	float multiinheritance_float_;
} multiinheritance;

struct Abstract2
{
	int   abstract2_int_;
	float abstract2_float_;
	virtual void virtual_func1() = 0;
	virtual void virtual_func2() = 0;
	virtual ~Abstract2() {}
};

struct DerivedPoly2
	: public Abstract2
{
	int   derivedpoly2_int_;
	float derivedpoly2_float_;
	virtual void virtual_func1() {}
	virtual void virtual_func2() {}
	Abstract2* abstract2() { return this; }
	Abstract2 const* abstract2() const { return this; }
	virtual ~DerivedPoly2() {}
} derivedpoly2;

struct MultiInheritance2
	: public DerivedPoly, public DerivedPoly2
{
	int   multiinheritance2_int_;
	float multiinheritance2_float_;
} multiinheritance2;

struct VirtualDerivedPoly
	: public virtual Derived
{
	int   virtualderivedpoly_int_;
	float virtualderivedpoly_float_;
	virtual void f1() {}
	virtual void f2() {}
	virtual ~VirtualDerivedPoly() {}
} virtualderivedpoly;

struct VirtualMultipleDerivedPoly
	: public virtual Derived, virtual public DerivedPoly
{
	int   virtualmultiplederivedpoly_int_;
	float virtualmultiplederivedpoly_float_;
	virtual void f1() {}
	virtual void f2() {}
	virtual ~VirtualMultipleDerivedPoly() {}
} virtualmultiplederivedpoly;

struct VirtualDerived
	: public virtual Derived
{
	int   virtualderived_int_;
	float virtualderived_float_;
	virtual void f1() {}
	virtual void f2() {}
	virtual ~VirtualDerived() {}
} virtualderived;

using namespace boost::intrusive;

TEST_CASE("parent_from_member_test", "parent_from_member_test") {
	//POD
	REQUIRE(&pod == get_parent_from_member(&pod.int_, &POD::int_));
	REQUIRE(&pod == get_parent_from_member(&pod.float_, &POD::float_));

	//Derived
	REQUIRE(&derived == get_parent_from_member(&derived.int_, &Derived::int_));
	REQUIRE(&derived == get_parent_from_member(&derived.float_, &Derived::float_));
	REQUIRE(&derived == get_parent_from_member(&derived.derived_int_, &Derived::derived_int_));
	REQUIRE(&derived == get_parent_from_member(&derived.derived_float_, &Derived::derived_float_));

	//Abstract
	REQUIRE(derivedpoly.abstract() == get_parent_from_member(&derivedpoly.abstract_int_, &Abstract::abstract_int_));
	REQUIRE(derivedpoly.abstract() == get_parent_from_member(&derivedpoly.abstract_float_, &Abstract::abstract_float_));

	//DerivedPoly
	REQUIRE(&derivedpoly == get_parent_from_member(&derivedpoly.abstract_int_, &DerivedPoly::abstract_int_));
	REQUIRE(&derivedpoly == get_parent_from_member(&derivedpoly.abstract_float_, &DerivedPoly::abstract_float_));
	REQUIRE(&derivedpoly == get_parent_from_member(&derivedpoly.derivedpoly_int_, &DerivedPoly::derivedpoly_int_));
	REQUIRE(&derivedpoly == get_parent_from_member(&derivedpoly.derivedpoly_float_, &DerivedPoly::derivedpoly_float_));

	//MultiInheritance
	REQUIRE(multiinheritance.abstract() == get_parent_from_member(&multiinheritance.abstract_int_, &MultiInheritance::abstract_int_));
	REQUIRE(multiinheritance.abstract() == get_parent_from_member(&multiinheritance.abstract_float_, &MultiInheritance::abstract_float_));
	REQUIRE(&multiinheritance == get_parent_from_member(&multiinheritance.derivedpoly_int_, &MultiInheritance::derivedpoly_int_));
	REQUIRE(&multiinheritance == get_parent_from_member(&multiinheritance.derivedpoly_float_, &MultiInheritance::derivedpoly_float_));
	REQUIRE(&multiinheritance == get_parent_from_member(&multiinheritance.int_, &MultiInheritance::int_));
	REQUIRE(&multiinheritance == get_parent_from_member(&multiinheritance.float_, &MultiInheritance::float_));
	REQUIRE(&multiinheritance == get_parent_from_member(&multiinheritance.derived_int_, &MultiInheritance::derived_int_));
	REQUIRE(&multiinheritance == get_parent_from_member(&multiinheritance.derived_float_, &MultiInheritance::derived_float_));

	REQUIRE(multiinheritance.abstract() == get_parent_from_member(&multiinheritance.abstract_int_, &MultiInheritance::abstract_int_));
	REQUIRE(multiinheritance.abstract() == get_parent_from_member(&multiinheritance.abstract_float_, &MultiInheritance::abstract_float_));
	REQUIRE(&multiinheritance == get_parent_from_member(&multiinheritance.derivedpoly_int_, &MultiInheritance::derivedpoly_int_));
	REQUIRE(&multiinheritance == get_parent_from_member(&multiinheritance.derivedpoly_float_, &MultiInheritance::derivedpoly_float_));
	REQUIRE(multiinheritance2.abstract2() == get_parent_from_member(&multiinheritance2.abstract2_int_, &MultiInheritance2::abstract2_int_));
	REQUIRE(multiinheritance2.abstract2() == get_parent_from_member(&multiinheritance2.abstract2_float_, &MultiInheritance2::abstract2_float_));
	REQUIRE(&multiinheritance2 == get_parent_from_member(&multiinheritance2.derivedpoly2_int_, &MultiInheritance2::derivedpoly2_int_));
	REQUIRE(&multiinheritance2 == get_parent_from_member(&multiinheritance2.derivedpoly2_float_, &MultiInheritance2::derivedpoly2_float_));

	//MSVC pointer to member data uses RTTI info even when not crossing virtual base boundaries
#ifndef BOOST_INTRUSIVE_MSVC_ABI_PTR_TO_MEMBER
	REQUIRE(&virtualderived == get_parent_from_member(&virtualderived.virtualderived_int_, &VirtualDerived::virtualderived_int_));
	REQUIRE(&virtualderived == get_parent_from_member(&virtualderived.virtualderived_float_, &VirtualDerived::virtualderived_float_));

	REQUIRE(&virtualderivedpoly == get_parent_from_member(&virtualderivedpoly.virtualderivedpoly_int_, &VirtualDerivedPoly::virtualderivedpoly_int_));
	REQUIRE(&virtualderivedpoly == get_parent_from_member(&virtualderivedpoly.virtualderivedpoly_float_, &VirtualDerivedPoly::virtualderivedpoly_float_));
	REQUIRE(&virtualmultiplederivedpoly == get_parent_from_member(&virtualmultiplederivedpoly.virtualmultiplederivedpoly_float_, &VirtualMultipleDerivedPoly::virtualmultiplederivedpoly_float_));
	REQUIRE(&virtualmultiplederivedpoly == get_parent_from_member(&virtualmultiplederivedpoly.virtualmultiplederivedpoly_int_, &VirtualMultipleDerivedPoly::virtualmultiplederivedpoly_int_));
	REQUIRE(&virtualmultiplederivedpoly == get_parent_from_member(&virtualmultiplederivedpoly.derivedpoly_float_, &VirtualMultipleDerivedPoly::derivedpoly_float_));
	REQUIRE(&virtualmultiplederivedpoly == get_parent_from_member(&virtualmultiplederivedpoly.derivedpoly_int_, &VirtualMultipleDerivedPoly::derivedpoly_int_));
#endif
}
