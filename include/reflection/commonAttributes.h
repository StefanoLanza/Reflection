#pragma once
#include "attribute.h"

namespace Typhoon::Reflection {

class FloatMin final : public Attribute {
public:
	FloatMin(float minValue)
	    : Attribute { Typhoon::typeId_v<FloatMin>, AttributeTargets::Property | AttributeTargets::Field }
	    , min { minValue } {
	}

	float getMin() const {
		return min;
	}

private:
	float min;
};

class FloatMax final : public Attribute {
public:
	FloatMax(float maxValue)
	    : Attribute { Typhoon::typeId_v<FloatMax>, AttributeTargets::Property | AttributeTargets::Field }
	    , max { maxValue } {
	}

	float getMax() const {
		return max;
	}

private:
	float max;
};

} // namespace Typhoon::Reflection
