#include "utils.h"
#include <cassert>
#include <include/enumType.h>
#include <include/bitMaskType.h>
#include <include/namespace.h>
#include <include/type.h>
#include <include/structType.h>
#include <include/visitor.h>
#include <iomanip>
#include <iostream>

namespace {

class Printer : public refl::TypeVisitor {
public:
	void beginNamespace(const refl::Namespace& nameSpace) override {
		if (nameSpace.getName()) { // nullptr for global 
			indent();
			std::cout << "namespace " << nameSpace.getName() << " {" << std::endl;
			indentation += whitespace;
		}
	}
	void endNamespace(const refl::Namespace& nameSpace) override {
		if (nameSpace.getName()) { // nullptr for global 
			indentation -= whitespace;
			indent();
			std::cout << "}" << std::endl;
		}
	}

	void visitType(const refl::Type& type) override {
		if (type.getSubClass() == refl::Type::Subclass::Enum) {
			visitEnum(static_cast<const refl::EnumType&>(type));
			return;
		}
		else if (type.getSubClass() == refl::Type::Subclass::BitMask) {
			visitBitmask(static_cast<const refl::BitMaskType&>(type));
			return;
		}
		indent();
		std::cout << type.getName();
		std::cout << std::endl;
	}

	void visitEnum(const refl::EnumType& type) {
		indent();
		std::cout << "enum ";
		std::cout << type.getName();
		std::cout << " {" << std::endl;
		indentation += whitespace;
		const auto& underlyingType = type.getUnderlyingType();
		for (auto& enumerator : type.getEnumerators()) {
			indent();
			std::cout << enumerator.name << ": ";
			printIntegerValue(enumerator.value, underlyingType);
			std::cout << "," << std::endl;
		}
		std::cout << "}" << std::endl;
		indentation -= whitespace;
	}

	void visitBitmask(const refl::BitMaskType& type) {
		indent();
		std::cout << "bitmask ";
		std::cout << type.getName();
		std::cout << " {" << std::endl;
		indentation += whitespace;
		const auto& underlyingType = type.getUnderlyingType();
		for (auto& constant : type.getEnumerators()) {
			indent();
			std::cout << constant.name << " = ";
			printIntegerValue(&constant.mask, underlyingType);
			std::cout << "," << std::endl;
		}
		std::cout << "}" << std::endl;
		indentation -= whitespace;
	}

	void visitField(const char* fieldName, const refl::Type& type) override {
		indent();
		std::cout << type.getName() << " " << fieldName << ";" << std::endl;
	}

	void beginClass(const refl::StructType& type) override {
		indent();
		std::cout << "struct " << type.getName() << " {" <<std::endl;
		indentation += whitespace;
	}
	void endClass() override {
		indentation -= whitespace;
		indent();
		std::cout << "};" << std::endl;
	}

private:
	void indent() const {
		for (std::streamsize i = 0; i < indentation; ++i) {
			std::cout << ' ';
		}
	}

	void printIntegerValue(const void* value, const refl::Type& underlyingType) const {
		uint64_t uv = 0;
//		int64_t  v = 0;
		// TODO Handle signed
		assert(sizeof(uv) >= underlyingType.getSize());
		std::memcpy(&uv, value, underlyingType.getSize());
		std::cout << uv;
	}

private:
	std::streamsize indentation = 0;
	static constexpr std::streamsize whitespace = 4;
};

} // namespace

void printRegisteredType(const refl::Type& type) {
	Printer            printer;
	refl::VisitOptions visitorOptions;
	refl::visitType(type, printer, visitorOptions);
}

void printNamespace(const refl::Namespace& nameSpace) {
	Printer            printer;
	refl::VisitOptions options;
	refl::visitNamespace(nameSpace, printer, options);
}
