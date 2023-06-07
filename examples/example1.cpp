// Example showing reflection and serialization of a builtin types, enums and bitmasks

#include <reflection/reflection.h>
#include <reflection/version.h>
#include <iostream>
#include <string>

#define XML  0
#define JSON 1
#if TY_REFLECTION_JSON && 1
#define ARCHIVE_TYPE JSON
#elif TY_REFLECTION_XML
#define ARCHIVE_TYPE XML
#else
#error "No supported archive types"
#endif

struct TestFlags : Typhoon::BitMask<uint16_t> {
	enum : StorageType {
		visible = 1,
		resizeable = 2,
		moveable = 4,
	};
};

enum class TestEnum {
	left,
	right,
	top,
	bottom,
};

struct Builtins {
	bool        b;
	int         i;
	float       f;
	double      d;
	int64_t     i64;
	uint64_t    ui64;
	std::string str;
	TestEnum    e;
	TestFlags   flags;
};

void        registerUserTypes();
Builtins    makeBuiltins();
std::string writeBuiltins(const Builtins& b, const char* XMLelement);
void        readBuiltins(Builtins& b, const std::string& xmlString, const char* XMLelement);

int main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "Reflection version: " << refl::getVersionString() << std::endl;

	refl::initReflection();
	registerUserTypes();

	const char* element = "builtins";
	Builtins    b = makeBuiltins();
	std::string archiveContent = writeBuiltins(b, element);
	Builtins    ob;
	readBuiltins(ob, archiveContent, element);

	refl::deinitReflection();
	return 0;
}

void registerUserTypes() {
	BEGIN_REFLECTION()

	BEGIN_BITMASK(TestFlags)
	BITMASK_VALUE(visible)
	BITMASK_VALUE(resizeable)
	BITMASK_VALUE(moveable)
	END_BITMASK();

	BEGIN_ENUM(TestEnum)
	ENUMERATOR(left)
	ENUMERATOR(right)
	ENUMERATOR(top)
	ENUMERATOR(bottom)
	END_ENUM();

	END_REFLECTION();
}

Builtins makeBuiltins() {
	Builtins b {};
	b.b = true;
	b.i = 10;
	b.f = -123.f;
	b.d = 3.14159265359;
	b.i64 = -123456789;
	b.ui64 = 123456789;
	b.str = "Stefano";
	b.e = TestEnum::left;
	b.flags.value = TestFlags::visible | TestFlags::resizeable;
	return b;
}

std::string writeBuiltins(const Builtins& obj, const char* element) {
	std::string content;
#if ARCHIVE_TYPE == XML
	refl::XMLOutputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONOutputArchive archive;
#endif
	if (archive.beginElement(element)) {
		if (archive.beginObject()) {
			archive.write("b", obj.b);
			archive.write("i", obj.i);
			archive.write("f", obj.f);
			archive.write("d", obj.d);
			archive.write("i64", obj.i64);
			archive.write("ui64", obj.ui64);
			archive.write("str", obj.str);
			archive.write("enum", obj.e);
			archive.write("flags", obj.flags);
			archive.endObject();
		}
		archive.endElement();
		archive.saveToString(content);
	}
	return content;
}

void readBuiltins(Builtins& obj, const std::string& content, const char* element) {
#if ARCHIVE_TYPE == XML
	refl::XMLInputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONInputArchive  archive;
#endif
	if (refl::ParseResult res = archive.initialize(content.data()); res) {
		if (archive.beginElement(element)) {
			archive.read("b", obj.b);
			archive.read("i", obj.i);
			archive.read("f", obj.f);
			archive.read("d", obj.d);
			archive.read("i64", obj.i64);
			archive.read("ui64", obj.ui64);
			archive.read("str", obj.str);
			archive.read("enum", obj.e);
			archive.read("flags", obj.flags);
			archive.endElement();
		}
	}
	else {
		std::cout << res.errorMessage << std::endl;
	}
}
