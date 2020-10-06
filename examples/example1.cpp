// Example showing reflection and serialization of a builtin types, enums and bitmasks

#include "utils.h"
#include <include/reflection.h>
#include <include/version.h>
#include <iostream>
#include <string>

#define XML  0
#define JSON 1
#if TY_REFLECTION_JSON
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
	int         i;
	float       f;
	double      d;
	std::string str;
	TestEnum    e;
	TestFlags   flags;
};

void        registerUserTypes();
void        printRegisteredTypes();
Builtins    makeBuiltins();
std::string writeBuiltins(const Builtins& b, const char* XMLelement);
void        readBuiltins(Builtins& b, const std::string& xmlString, const char* XMLelement);

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "Reflection version: " << refl::getVersionString() << std::endl;

#if _DEBUG
	refl::initReflection();
#else
	char                     buffer[8192];
	Typhoon::HeapAllocator   heapAllocator;
	Typhoon::LinearAllocator linearAllocator(buffer, std::size(buffer), &heapAllocator);
	refl::initReflection(linearAllocator);
#endif
	registerUserTypes();
	printRegisteredTypes();

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

void printRegisteredTypes() {
	printRegisteredType<TestFlags>();
	printRegisteredType<TestEnum>();
}

Builtins makeBuiltins() {
	Builtins b {};
	b.i = 10;
	b.f = -123.f;
	b.d = 3.14159265359;
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
		archive.beginObject();
		writeObject(obj.i, "i", archive);
		writeObject(obj.f, "f", archive);
		writeObject(obj.d, "d", archive);
		writeObject(obj.str, "str", archive);
		writeObject(obj.e, "enum", archive);
		writeObject(obj.flags, "flags", archive);
		archive.endObject();
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
			readObject(&obj.i, "i", archive);
			readObject(&obj.f, "f", archive);
			readObject(&obj.d, "d", archive);
			readObject(&obj.str, "str", archive);
			readObject(&obj.e, "enum", archive);
			readObject(&obj.flags, "flags", archive);
			archive.endElement();
		}
	}
	else {
		std::cout << res.errorMessage << std::endl;
	}
}
