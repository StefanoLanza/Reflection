#pragma once
#include "attribute.h"

namespace Typhoon::Reflection {

class FloatMin : public Attribute {
public:
	FloatMin(float minValue)
	    : Attribute(getTypeId<FloatMin>())
	    , min(minValue) {
	}

	float getMin() const {
		return min;
	}

private:
	float min;
};

class FloatMax : public Attribute {
public:
	FloatMax(float maxValue)
	    : Attribute(getTypeId<FloatMax>())
	    , max(maxValue) {
	}

	float getMax() const {
		return max;
	}

private:
	float max;
};

class FloatRange : public Attribute {
public:
	FloatRange(float minValue, float maxValue)
	    : Attribute(getTypeId<FloatRange>())
	    , min(minValue)
	    , max(maxValue) {
	}
	float getMin() const {
		return min;
	}
	float getMax() const {
		return max;
	}

private:
	float min;
	float max;
};

} // namespace Typhoon::Reflection
