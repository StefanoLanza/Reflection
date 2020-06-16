#define CATCH_CONFIG_RUNNER
#include <Catch-master/single_include/catch2/catch.hpp>

#include "testClasses.h"
#include <reflection/include/reflection.h>
#include <string>

using namespace Typhoon;

void    registerUserTypes(TypeDB& typeDB);
TypeDB* g_typeDB = nullptr;

void compare(const GameObject& o0, const GameObject& o1) {
	CHECK(o0.getLives() == o1.getLives());
	CHECK(o0.getActionFlags() == o1.getActionFlags());
	CHECK(o0.getName() == o1.getName());
	CHECK(o0.getPosition() == o1.getPosition());
	CHECK(o0.getMaterial() == o1.getMaterial());
}

int __cdecl main(int argc, char* argv[]) {
	TypeDB typeDB;
	g_typeDB = &typeDB;
	initReflection(typeDB);
	registerUserTypes(typeDB);
	return Catch::Session().run(argc, argv);
}

TEST_CASE("Primitives") {
	char          c = 120, c2 = 0;
	unsigned char uc = 250, uc2 = 0;
	int           i = 0, i2 = 0;
	unsigned      ui = 1, ui2 = 0;
	long          l = 2, l2 = 0;
	unsigned long ul = 3, ul2 = 0;
	float         f = 3.14f, f2 = 0.f;
	double        d = 4234234234., d2 = 0.;
	bool          b = false, b2 = true;

	SECTION("XML Serialization") {
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		{
			REQUIRE(outArchive.beginElement("root"));
			REQUIRE(writeObject(c, "c", outArchive));
			REQUIRE(writeObject(uc, "uc", outArchive));
			REQUIRE(writeObject(i, "i", outArchive));
			REQUIRE(writeObject(ui, "ui", outArchive));
			REQUIRE(writeObject(l, "l", outArchive));
			REQUIRE(writeObject(ul, "ul", outArchive));
			REQUIRE(writeObject(f, "f", outArchive));
			REQUIRE(writeObject(d, "d", outArchive));
			REQUIRE(writeObject(b, "b", outArchive));
			outArchive.endElement();
			REQUIRE(outArchive.saveToString(xmlContent));
		}
		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		{
			inArchive.beginElement("root");
			REQUIRE(readObject(&c2, "c", inArchive));
			REQUIRE(readObject(&uc2, "uc", inArchive));
			REQUIRE(readObject(&i2, "i", inArchive));
			REQUIRE(readObject(&ui2, "ui", inArchive));
			REQUIRE(readObject(&l2, "l", inArchive));
			REQUIRE(readObject(&ul2, "ul", inArchive));
			REQUIRE(readObject(&f2, "f", inArchive));
			REQUIRE(readObject(&d2, "d", inArchive));
			REQUIRE(readObject(&b2, "b", inArchive));
			CHECK(c == c2);
			CHECK(uc == uc2);
			CHECK(i == i2);
			CHECK(ui == ui2);
			CHECK(l == l2);
			CHECK(ul == ul2);
			CHECK(f == f2);
			CHECK(d == d2);
			CHECK(b == b2);
			inArchive.endElement();
		}
	}
	SECTION("Clone") {
		cloneObject(&c2, c);
		CHECK(c2 == c);
		cloneObject(&uc2, uc);
		CHECK(uc2 == uc);
		cloneObject(&i2, i);
		CHECK(i2 == i);
		cloneObject(&ui2, ui);
		CHECK(ui2 == ui);
		cloneObject(&l2, l);
		CHECK(l2 == l);
		cloneObject(&ul2, ul);
		CHECK(ul2 == ul);
		cloneObject(&f2, f);
		CHECK(f2 == f);
		cloneObject(&d2, d);
		CHECK(d2 == d);
		cloneObject(&b2, b);
		CHECK(b2 == b);
	}
}

TEST_CASE("Enum") {
	SeasonType season = SeasonType::spring;
	SECTION("XML Serialization") {
		const char*      XMLelement = "season";
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		{
			REQUIRE(writeObject(season, XMLelement, outArchive));
			REQUIRE(outArchive.saveToString(xmlContent));
		}
		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		{
			SeasonType readSeason;
			REQUIRE(readObject(&readSeason, XMLelement, inArchive));
			CHECK(readSeason == season);
		}
	}
	SECTION("Clone") {
		SeasonType cloned;
		REQUIRE(ErrorCode::ok == cloneObject(&cloned, season));
		REQUIRE(cloned == season);
	}
}

TEST_CASE("BitMask") {
	ActionFlags flags;
	flags.value = ActionFlags::running | ActionFlags::smiling;
	SECTION("XML Serialization") {
		const char*      XMLelement = "flags";
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		{
			REQUIRE(writeObject(flags, XMLelement, outArchive));
			REQUIRE(outArchive.saveToString(xmlContent));
		}
		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		{
			ActionFlags readFlags;
			REQUIRE(readObject(&readFlags, XMLelement, inArchive));
			CHECK(readFlags == flags);
		}
	}
	SECTION("Clone") {
		ActionFlags cloned;
		cloneObject(&cloned, flags);
		REQUIRE(cloned.value == flags.value);
	}
}

TEST_CASE("std::vector") {
	using Vector = std::vector<std::string>;
	const Vector vec { "Stefano", "Claudio", "Cristiana", "Manlio", "Nicoletta", "Enrico", "Oriana", "Marco" };
	SECTION("XML serialization") {
		const char*      XMLelement = "vector";
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		REQUIRE(writeObject(vec, XMLelement, outArchive));
		REQUIRE(outArchive.saveToString(xmlContent));

		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		Vector in_vec;
		REQUIRE(readObject(&in_vec, XMLelement, inArchive));
		CHECK(in_vec == vec);
	}
	SECTION("Clone") {
		Vector cloned;
		cloneObject(&cloned, vec);
		CHECK(vec == cloned);
	}
}

TEST_CASE("std::map") {
	using Map = std::map<std::string, std::string>;
	const Map map { { "Lanza", "Stefano" }, { "Biancardi", "Nicoletta" }, { "Moschella", "Enrico" } };
	SECTION("XML serialization") {
		const char*      XMLelement = "map";
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		REQUIRE(writeObject(map, XMLelement, outArchive));
		REQUIRE(outArchive.saveToString(xmlContent));

		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		Map in_map;
		REQUIRE(readObject(&in_map, XMLelement, inArchive));
		CHECK(in_map == map);
	}
	SECTION("Clone") {
		Map cloned;
		cloneObject(&cloned, map);
		CHECK(map == cloned);
	}
}

TEST_CASE("std::array") {
	using Array = std::array<int, 16>;
	Array array;
	for (auto& e : array) {
		e = static_cast<int>(reinterpret_cast<uintptr_t>(&e) & 0xFFFF);
	}

	SECTION("XML serialization") {
		const char*      XMLelement = "array";
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		REQUIRE(writeObject(array, XMLelement, outArchive));
		REQUIRE(outArchive.saveToString(xmlContent));

		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		Array in_array;
		REQUIRE(readObject(&in_array, XMLelement, inArchive));
		CHECK(in_array == array);
	}
	SECTION("Clone") {
		Array cloned;
		cloneObject(&cloned, array);
		CHECK(cloned == array);
	}
}

TEST_CASE("std::pair") {
	using Pair = std::pair<bool, std::string>;
	Pair pair { true, "Stefano" };

	SECTION("XML serialization") {
		const char*      XMLelement = "pair";
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		REQUIRE(writeObject(pair, XMLelement, outArchive));
		REQUIRE(outArchive.saveToString(xmlContent));

		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		Pair inPair;
		REQUIRE(readObject(&inPair, XMLelement, inArchive));
		CHECK(inPair == pair);
	}
	SECTION("Clone") {
		Pair cloned;
		cloneObject(&cloned, pair);
		CHECK(cloned == pair);
	}
}

TEST_CASE("std::tuple") {
	using Tuple = std::tuple<std::string, int, float, Coords>;
	const Tuple tuple { "Stefano", 40, 1.78f, { 10.f, 20.f, 30.f } };

	SECTION("XML serialization") {
		const char*      XMLelement = "tuple";
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		REQUIRE(writeObject(tuple, XMLelement, outArchive));
		REQUIRE(outArchive.saveToString(xmlContent));

		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		Tuple inTuple;
		REQUIRE(readObject(&inTuple, XMLelement, inArchive));
		CHECK(inTuple == tuple);
	}
	SECTION("Clone") {
		Tuple cloned;
		cloneObject(&cloned, tuple);
		CHECK(cloned == tuple);
	}
}

TEST_CASE("std::unique_ptr") {
	const auto material = std::make_unique<Material>(Material { "material", Color { 255, 0, 0 } });

	SECTION("XML serialization") {
		const char*      XMLelement = "material";
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		REQUIRE(writeObject(material, XMLelement, outArchive));
		REQUIRE(outArchive.saveToString(xmlContent));

		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		auto inMaterial = std::make_unique<Material>();
		REQUIRE(readObject(&inMaterial, XMLelement, inArchive));
		CHECK(*inMaterial == *material);
	}
	SECTION("Clone") {
		auto clonedMaterial = std::make_unique<Material>();
		cloneObject(&clonedMaterial, material);
		CHECK(*clonedMaterial == *material);
	}
}

TEST_CASE("std::shared_ptr") {
	const auto material = std::make_shared<Material>(Material { "material", Color { 255, 0, 0 } });

	SECTION("XML serialization") {
		const char*      XMLelement = "material";
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		REQUIRE(writeObject(material, XMLelement, outArchive));
		REQUIRE(outArchive.saveToString(xmlContent));

		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		auto inMaterial = std::make_shared<Material>();
		REQUIRE(readObject(&inMaterial, XMLelement, inArchive));
		CHECK(*inMaterial == *material);
	}
	SECTION("Clone") {
		auto clonedMaterial = std::make_shared<Material>();
		cloneObject(&clonedMaterial, material);
		CHECK(*clonedMaterial == *material);
	}
}

TEST_CASE("Class") {
	ActionFlags actionFlags;
	actionFlags.value = ActionFlags::running | ActionFlags::smiling;
	GameObject gameObject;
	gameObject.setLives(1000);
	gameObject.setName("William");
	gameObject.setActionFlags(actionFlags);
	gameObject.setPosition({ 0.f, 1.f, 2.f });
	gameObject.setMaterial(Material { "glossy", Color { 255, 255, 127 } });

	SECTION("XML Serialization") {
		const char*      XMLelement = "gameObject";
		std::string      xmlContent;
		XMLOutputArchive outArchive;

		CHECK(writeObject(gameObject, XMLelement, outArchive));
		outArchive.saveToString(xmlContent);

		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		GameObject inObject;
		REQUIRE(readObject(&inObject, XMLelement, inArchive));

		compare(inObject, gameObject);
	}
	SECTION("Clone") {
		GameObject clonedObject;
		cloneObject(&clonedObject, gameObject);
		compare(clonedObject, gameObject);
	}
}

TEST_CASE("Struct") {
	Fog fog;
	setDensity(fog, 10.f);
	setColor(fog, { 1.f, 0.5f, 0.5f });

	SECTION("XML Serialization") {
		const char*      XMLelement = "fog";
		std::string      xmlContent;
		XMLOutputArchive outArchive;

		CHECK(writeObject(fog, XMLelement, outArchive));
		outArchive.saveToString(xmlContent);

		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		Fog inFog;
		REQUIRE(readObject(&inFog, XMLelement, inArchive));
		REQUIRE(fog == inFog);
	}
	SECTION("Clone") {
		Fog clonedFog;
		cloneObject(&clonedFog, fog);
		REQUIRE(fog == clonedFog);
	}
}

#if 0
	TestDerivedClass derivedObject;
	CHECK(writeObject(derivedObject, "derivedObject", archive));

}
#endif

TEST_CASE("Variant") {
	using VariantArray = std::array<Variant, 2>;
	const VariantArray variants { Variant { 3.14, "double" }, Variant { 41, "int" } };
	VariantArray       otherVariants { Variant { 0., "double" }, Variant { 0, "int" } };
	VariantArray       clonedVariants { Variant { 0., "double" }, Variant { 0, "int" } };

	auto compare = [](const VariantArray& b0, const VariantArray& b1) { return b0[0] == b1[0] && b0[1] == b1[1]; };

	SECTION("XML Serialization") {
		const char*      XMLelement = "variants";
		std::string      xmlContent;
		XMLOutputArchive outArchive;

		CHECK(writeObject(variants, XMLelement, outArchive));
		outArchive.saveToString(xmlContent);

		XMLInputArchive inArchive;
		REQUIRE(inArchive.initialize(xmlContent.data()));
		REQUIRE(readObject(&otherVariants, XMLelement, inArchive));
		CHECK(compare(otherVariants, variants));
	}
	SECTION("Clone") {
		cloneObject(&clonedVariants, variants);
		CHECK(compare(clonedVariants, variants));
	}
}

void registerUserTypes(TypeDB& typeDB) {
	BEGIN_REFLECTION(typeDB)

	BEGIN_BITMASK(ActionFlags)
	BITMASK_VALUE(running)
	BITMASK_VALUE(shooting)
	BITMASK_VALUE(smiling)
	END_BITMASK();

	BEGIN_STRUCT(Color);
	FIELD(r);
	FIELD(g);
	FIELD(b);
	END_STRUCT();

	BEGIN_STRUCT(Coords);
	FIELD(x);
	FIELD(y);
	FIELD(z);
	END_STRUCT();

	BEGIN_STRUCT(Fog);
	C_PROPERTY("density", setDensity, getDensity);
	C_SETTER(color, setColor);
	END_STRUCT();

	BEGIN_ENUM(SeasonType)
	ENUMERATOR(spring)
	ENUMERATOR(summer)
	ENUMERATOR(autumn)
	ENUMERATOR(winter)
	END_ENUM();

	BEGIN_STRUCT(Material);
	FIELD(name);
	FIELD(color);
	END_STRUCT();
	typeDB.setCustomReader<Material>(customReadMaterial);
	typeDB.setCustomWriter<Material>(customSaveMaterial);

	BEGIN_CLASS(GameObject);
	PROPERTY("lives", getLives, setLives);
	PROPERTY("name", getName, setName);
	PROPERTY("position", getPosition, setPosition);
	PROPERTY_EX("action", getActionFlags, setActionFlags, Flags::all, Semantic::none);
	PROPERTY("material", getMaterial, setMaterial);
	END_CLASS();

	BEGIN_SUB_CLASS(DerivedGameObject, GameObject);
	PROPERTY("energy", getEnergy, setEnergy);
	END_CLASS();

	END_REFLECTION();
}
