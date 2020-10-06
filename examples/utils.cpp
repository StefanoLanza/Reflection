#include "utils.h"
#include <include/enumType.h>
#include <include/type.h>
#include <include/visitor.h>
#include <iomanip>
#include <iostream>

namespace {

void printEnumerator(const refl::Enumerator& enumerator, const refl::Type& underlyingType) {
	uint64_t uv = 0;
	int64_t v = 0;
	// TODO Handle signed
	std::memcpy(&uv, enumerator.value, underlyingType.getSize());
	std::cout << uv;
}

void printEnum(const refl::EnumType& enumType, std::streamsize indent) {
	std::cout << std::endl;
	const auto underlyingType = enumType.getUnderlyingType();
	for (auto& enumerator : enumType.getEnumerators()) {
		std::cout << std::setfill(' ') << std::setw(indent + 4) << ' ';
		std::cout << enumerator.name << ": ";
		printEnumerator(enumerator, underlyingType);
		std::cout << std::endl;
	}
}

} // namespace

void printRegisteredType(Typhoon::TypeId typeId) {
	refl::Visitor visitor = [](const refl::Type& type, const refl::VisitContext& context) {
		std::streamsize indent = std::streamsize(context.level) * 4;
		std::cout << std::setfill(' ') << std::setw(indent) << ' ';
		if (type.getSubClass() == refl::Type::Subclass::Struct) {
			std::cout << "struct ";
		}
		else if (type.getSubClass() == refl::Type::Subclass::Enum) {
			std::cout << "enum ";
		}
		std::cout << type.getName();
		if (context.objectName) {
			std::cout << " " << context.objectName;
		}
		if (type.getSubClass() == refl::Type::Subclass::Enum) {
			printEnum(static_cast<const refl::EnumType&>(type), indent);
		}
		std::cout << std::endl;
	};
	refl::VisitOptions visitorOptions;
	refl::visitType(typeId, visitor, visitorOptions);
}
