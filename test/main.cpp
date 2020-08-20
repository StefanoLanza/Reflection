#define CATCH_CONFIG_RUNNER
#include <Catch-master/single_include/catch2/catch.hpp>

#include "testClasses.h"
#include <include/reflection.h>
#include <string>

void registerUserTypes();

void compare(const GameObject& o0, const GameObject& o1) {
	CHECK(o0.getLives() == o1.getLives());
	CHECK(o0.getActionFlags() == o1.getActionFlags());
	CHECK(o0.getName() == o1.getName());
	CHECK(o0.getPosition() == o1.getPosition());
	CHECK(o0.getMaterial() == o1.getMaterial());
}

template <class T>
bool compareArrays(const T a[], const T b[], size_t n) {
	for (size_t i = 0; i < n; ++i) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}

int __cdecl main(int argc, char* argv[]) {
	refl::initReflection();
	registerUserTypes();
	const int res = Catch::Session().run(argc, argv);
	refl::deinitReflection();
	return res;
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
	using namespace refl;

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		REQUIRE(writeObject(c, "c", archive));
		REQUIRE(writeObject(uc, "uc", archive));
		REQUIRE(writeObject(i, "i", archive));
		REQUIRE(writeObject(ui, "ui", archive));
		REQUIRE(writeObject(l, "l", archive));
		REQUIRE(writeObject(ul, "ul", archive));
		REQUIRE(writeObject(f, "f", archive));
		REQUIRE(writeObject(d, "d", archive));
		REQUIRE(writeObject(b, "b", archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		REQUIRE(readObject(&c2, "c", archive));
		REQUIRE(readObject(&uc2, "uc", archive));
		REQUIRE(readObject(&i2, "i", archive));
		REQUIRE(readObject(&ui2, "ui", archive));
		REQUIRE(readObject(&l2, "l", archive));
		REQUIRE(readObject(&ul2, "ul", archive));
		REQUIRE(readObject(&f2, "f", archive));
		REQUIRE(readObject(&d2, "d", archive));
		REQUIRE(readObject(&b2, "b", archive));
		CHECK(c == c2);
		CHECK(uc == uc2);
		CHECK(i == i2);
		CHECK(ui == ui2);
		CHECK(l == l2);
		CHECK(ul == ul2);
		CHECK(f == f2);
		CHECK(d == d2);
		CHECK(b == b2);
	};

	SECTION("XML Serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON Serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
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
	using namespace refl;
	SeasonType  season = SeasonType::spring;
	const char* elementName = "season";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		std::string content;
		REQUIRE(writeObject(season, elementName, archive));
		archive.endRoot();
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		SeasonType readSeason;
		REQUIRE(readObject(&readSeason, elementName, archive));
		CHECK(readSeason == season);
	};

	SECTION("XML Serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON Serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		SeasonType cloned;
		REQUIRE(ErrorCode::ok == cloneObject(&cloned, season));
		REQUIRE(cloned == season);
	}
}

TEST_CASE("BitMask") {
	using namespace refl;
	ActionFlags flags;
	flags.value = ActionFlags::running | ActionFlags::smiling;
	const char* elementName = "flags";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		REQUIRE(writeObject(flags, elementName, archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		ActionFlags readFlags;
		REQUIRE(readObject(&readFlags, elementName, archive));
		CHECK(readFlags == flags);
	};

	SECTION("XML Serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON Serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		ActionFlags cloned;
		cloneObject(&cloned, flags);
		REQUIRE(cloned.value == flags.value);
	}
}

TEST_CASE("C array") {
	using namespace refl;

	constexpr size_t size = 64;
	using Array = int[size];
	Array array;
	int   i = 1;
	for (int& e : array) {
		e = (i * (i + 1)) / 2; // triangular number
		++i;
	}

	const char* elementName = "array";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		REQUIRE(writeObject(array, elementName, archive));
		std::string content;
		archive.endRoot();
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		Array in_array;
		REQUIRE(readObject(&in_array, elementName, archive));
		CHECK(compareArrays(in_array, array, size));
	};

	SECTION("XML serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		Array cloned;
		cloneObject(&cloned, array);
		CHECK(compareArrays(cloned, array, size));
	}
}

TEST_CASE("std::vector") {
	using namespace refl;
	using Vector = std::vector<std::string>;
	const Vector vec { "Stefano", "Claudio", "Cristiana", "Manlio", "Nicoletta", "Enrico", "Oriana", "Marco" };
	const char*  elementName = "vector";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		REQUIRE(writeObject(vec, elementName, archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		Vector in_vec;
		REQUIRE(readObject(&in_vec, elementName, archive));
		CHECK(in_vec == vec);
	};

	SECTION("XML serialization") {
		std::string      xmlContent;
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON serialization") {
		std::string       xmlContent;
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		Vector cloned;
		cloneObject(&cloned, vec);
		CHECK(vec == cloned);
	}
}

TEST_CASE("std::map") {
	using namespace refl;
	using Map = std::map<std::string, std::string>;
	const Map   map { { "Lanza", "Stefano" }, { "Biancardi", "Nicoletta" }, { "Moschella", "Enrico" } };
	const char* elementName = "map";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		REQUIRE(writeObject(map, elementName, archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		Map inMap;
		REQUIRE(readObject(&inMap, elementName, archive));
		CHECK(inMap == map);
	};

	SECTION("XML serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		Map cloned;
		cloneObject(&cloned, map);
		CHECK(map == cloned);
	}
}

TEST_CASE("std::array") {
	using namespace refl;
	using Array = std::array<int, 16>;
	Array array;
	for (auto& e : array) {
		e = static_cast<int>(reinterpret_cast<uintptr_t>(&e) & 0xFFFF);
	}

	const char* elementName = "array";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		REQUIRE(writeObject(array, elementName, archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		Array in_array;
		REQUIRE(readObject(&in_array, elementName, archive));
		CHECK(in_array == array);
	};

	SECTION("XML serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		Array cloned;
		cloneObject(&cloned, array);
		CHECK(cloned == array);
	}
}

TEST_CASE("std::pair") {
	using namespace refl;
	using Pair = std::pair<bool, std::string>;
	const Pair  pair { true, "Stefano" };
	const char* elementName = "pair";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		REQUIRE(writeObject(pair, elementName, archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		Pair inPair;
		REQUIRE(readObject(&inPair, elementName, archive));
		CHECK(inPair == pair);
	};

	SECTION("XML serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		Pair cloned;
		cloneObject(&cloned, pair);
		CHECK(cloned == pair);
	}
}

TEST_CASE("std::tuple") {
	using namespace refl;
	using Tuple = std::tuple<std::string, int, float, Coords>;
	const Tuple tuple { "Stefano", 40, 1.78f, { 10.f, 20.f, 30.f } };

	const char* elementName = "tuple";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		REQUIRE(writeObject(tuple, elementName, archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		Tuple inTuple;
		REQUIRE(readObject(&inTuple, elementName, archive));
		CHECK(inTuple == tuple);
	};

	SECTION("XML serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		Tuple cloned;
		cloneObject(&cloned, tuple);
		CHECK(cloned == tuple);
	}
}

TEST_CASE("std::unique_ptr") {
	using namespace refl;
	const auto  material = std::make_unique<Material>(Material { "material", Color { 255, 0, 0 } });
	const char* elementName = "material";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		REQUIRE(writeObject(material, elementName, archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		auto inMaterial = std::make_unique<Material>();
		REQUIRE(readObject(&inMaterial, elementName, archive));
		CHECK(*inMaterial == *material);
	};

	SECTION("XML serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		auto clonedMaterial = std::make_unique<Material>();
		cloneObject(&clonedMaterial, material);
		CHECK(*clonedMaterial == *material);
	}
}

TEST_CASE("std::shared_ptr") {
	using namespace refl;
	const auto  material = std::make_shared<Material>(Material { "material", Color { 255, 0, 0 } });
	const char* elementName = "material";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		REQUIRE(writeObject(material, elementName, archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		auto inMaterial = std::make_unique<Material>();
		REQUIRE(readObject(&inMaterial, elementName, archive));
		CHECK(*inMaterial == *material);
	};

	SECTION("XML serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		auto clonedMaterial = std::make_shared<Material>();
		cloneObject(&clonedMaterial, material);
		CHECK(*clonedMaterial == *material);
	}
}

TEST_CASE("Class") {
	using namespace refl;

	ActionFlags actionFlags;
	actionFlags.value = ActionFlags::running | ActionFlags::smiling;
	GameObject gameObject;
	gameObject.setLives(1000);
	gameObject.setName("William");
	gameObject.setActionFlags(actionFlags);
	gameObject.setPosition({ 0.f, 1.f, 2.f });
	gameObject.setMaterial(Material { "glossy", Color { 255, 255, 127 } });

	const char* elementName = "gameObject";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		CHECK(writeObject(gameObject, elementName, archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		GameObject inObject;
		REQUIRE(readObject(&inObject, elementName, archive));
		compare(inObject, gameObject);
	};

	SECTION("XML serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("Clone") {
		GameObject clonedObject;
		cloneObject(&clonedObject, gameObject);
		compare(clonedObject, gameObject);
	}
}

TEST_CASE("Struct") {
	using namespace refl;

	Fog fog;
	setDensity(fog, 10.f);
	setColor(fog, { 1.f, 0.5f, 0.5f });

	const char* elementName = "fog";

	auto write = [&](OutputArchive& archive) {
		archive.beginRoot();
		CHECK(writeObject(fog, elementName, archive));
		archive.endRoot();
		std::string content;
		REQUIRE(archive.saveToString(content));
		return content;
	};

	auto read = [&](InputArchive& archive) {
		Fog inFog;
		REQUIRE(readObject(&inFog, elementName, archive));
		REQUIRE(fog == inFog);
	};

	SECTION("XML serialization") {
		XMLOutputArchive outArchive;
		std::string      content = write(outArchive);
		XMLInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
	}

	SECTION("JSON serialization") {
		JSONOutputArchive outArchive;
		std::string       content = write(outArchive);
		JSONInputArchive  inArchive;
		REQUIRE(inArchive.initialize(content.data()));
		read(inArchive);
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
	using namespace refl;

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

void registerUserTypes() {
	BEGIN_REFLECTION()

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
	READER(customReadMaterial);
	WRITER(customSaveMaterial);
	END_STRUCT();

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
