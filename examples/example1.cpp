// Example showing reflection and serialization of a builtin types, enums and bitmasks

#include <include/reflection.h>
#include <include/version.h>
#include <iostream>
#include <string>

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

void        registerUserTypes(refl::TypeDB& typeDB);
Builtins    makeBuiltins();
std::string writeBuiltins(const Builtins& b, const char* XMLelement);
void        readBuiltins(Builtins& b, const std::string& xmlString, const char* XMLelement);

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "Reflection version: " << refl::getVersionString() << std::endl;

	refl::TypeDB& typeDB = refl::initReflection();
	registerUserTypes(typeDB);

	const char* xmlElement = "builtins";
	Builtins    b = makeBuiltins();
	std::string xmlContent = writeBuiltins(b, xmlElement);
	Builtins    ob;
	readBuiltins(ob, xmlContent, xmlElement);
	return 0;
}

void registerUserTypes(refl::TypeDB& typeDB) {
	BEGIN_REFLECTION(typeDB)

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
	b.i = 10;
	b.f = -123.f;
	b.d = 3.14159265359;
	b.str = "Stefano";
	b.e = TestEnum::left;
	b.flags.value = TestFlags::visible | TestFlags::resizeable;
	return b;
}

std::string writeBuiltins(const Builtins& obj, const char* XMLelement) {
	std::string            xmlContent;
	refl::XMLOutputArchive archive;
	if (archive.beginElement(XMLelement)) {
		writeObject(obj.i, "i", archive);
		writeObject(obj.f, "f", archive);
		writeObject(obj.d, "d", archive);
		writeObject(obj.str, "str", archive);
		writeObject(obj.e, "enum", archive);
		writeObject(obj.flags, "flags", archive);
		archive.endElement();
		archive.saveToString(xmlContent);
	}
	return xmlContent;
}

void readBuiltins(Builtins& obj, const std::string& xmlContent, const char* XMLelement) {
	refl::XMLInputArchive archive;
	if (archive.initialize(xmlContent.data())) {
		if (archive.beginElement(XMLelement)) {
			readObject(&obj.i, "i", archive);
			readObject(&obj.f, "f", archive);
			readObject(&obj.d, "d", archive);
			readObject(&obj.str, "str", archive);
			readObject(&obj.e, "enum", archive);
			readObject(&obj.flags, "flags", archive);
			archive.endElement();
		}
	}
}
