#include "utils.h"
#include <cassert>
#include <include/enumType.h>
#include <include/namespace.h>
#include <include/type.h>
#include <include/structType.h>
#include <include/visitor.h>
#include <iomanip>
#include <iostream>

namespace {

class Printer : public refl::Visitor {
public:
	void beginNamespace(const refl::Namespace& nameSpace) {
		indent();
		std::cout << "namespace " << nameSpace.getName() << " {" << std::endl;
		indentation += whitespace;
	}
	void endNamespace() {
		indentation -= whitespace;
		indent();
		std::cout << "}" << std::endl;
	}
	void visitType(const refl::Type& type) {
		indent();
		if (type.getSubClass() == refl::Type::Subclass::Enum) {
			std::cout << "enum ";
		}
		std::cout << type.getName();
		std::cout << std::endl;
		if (type.getSubClass() == refl::Type::Subclass::Enum) {
			printEnum(static_cast<const refl::EnumType&>(type));
		}
	}
	void visitField(const char* fieldName, const refl::Type& type) {
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
		std::cout << std::setfill(' ') << std::setw(indentation) << ' ';
	}

	void printEnum(const refl::EnumType& enumType) {
		indentation += whitespace;
		const auto underlyingType = enumType.getUnderlyingType();
		for (auto& enumerator : enumType.getEnumerators()) {
			indent();
			std::cout << enumerator.name << ": ";
			printEnumerator(enumerator, underlyingType);
			std::cout << std::endl;
		}
		indentation -= whitespace;
	}

	void printEnumerator(const refl::Enumerator& enumerator, const refl::Type& underlyingType) const {
		uint64_t uv = 0;
		int64_t  v = 0;
		// TODO Handle signed
		assert(sizeof(uv) >= underlyingType.getSize());
		std::memcpy(&uv, enumerator.value, underlyingType.getSize());
		std::cout << uv;
	}

private:
	std::streamsize indentation = 0;
	static constexpr std::streamsize whitespace = 4;
};

} // namespace

void printRegisteredType(const refl::Type& type) {
	refl::VisitOptions visitorOptions;
	// refl::visitType(type, visitor, visitorOptions);
}

void printNamespace(const refl::Namespace& nameSpace) {
	Printer            printer;
	refl::VisitOptions options;
	refl::visitNamespace(nameSpace, printer, options);
}
