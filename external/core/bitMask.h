#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace Typhoon {

template <class EnumType>
class Bitmask final {
public:
	using Enum = EnumType;
	using ValueType = std::underlying_type<EnumType>::type;

	static_assert(std::is_enum_v<EnumType>, "EnumType is not a enum type");
	static_assert(std::is_unsigned_v<ValueType>, "EnumType underlying type must be unsigned");

	constexpr Bitmask()
	    : bits {} {};

	constexpr Bitmask(EnumType bit) noexcept
	    : bits { static_cast<ValueType>(bit) } {
	}

	constexpr Bitmask(EnumType bit, std::same_as<EnumType> auto... args) noexcept
	    : bits { static_cast<ValueType>(static_cast<ValueType>(bit) | (static_cast<ValueType>(args) | ...)) } {
	}

	[[nodiscard]] constexpr ValueType value() const;
	void                              set(EnumType bit);
	void                              unset(EnumType bit);
	[[nodiscard]] bool                isSet(EnumType bit) const;
	constexpr void                    flip(EnumType bit);

	constexpr Bitmask operator~() const noexcept {
		return Bitmask { std::true_type {}, ~bits };
	}

	constexpr Bitmask operator&(const Bitmask& r) const noexcept {
		return Bitmask { std::true_type {}, bits & r.bits };
	}

	constexpr Bitmask operator|(const Bitmask& r) const noexcept {
		return Bitmask { std::true_type {}, bits | r.bits };
	}

	constexpr Bitmask operator^(const Bitmask& r) const noexcept {
		return Bitmask { std::true_type {}, bits ^ r.bits };
	}

	Bitmask& operator|=(const Bitmask& r) noexcept {
		bits |= r.bits;
		return *this;
	}

	Bitmask& operator&=(const Bitmask& r) noexcept {
		bits &= r.bits;
		return *this;
	}

	Bitmask& operator^=(const Bitmask& r) noexcept {
		bits ^= r.bits;
		return *this;
	}

private:
	template <class U>
	constexpr Bitmask(std::true_type, U bits) noexcept
	    : bits { static_cast<ValueType>(bits) } {
	}

private:
	ValueType bits;
};

template <class EnumType>
inline constexpr Bitmask<EnumType>::ValueType Bitmask<EnumType>::value() const {
	return bits;
}

template <class EnumType>
inline void Bitmask<EnumType>::set(EnumType bit) {
	bits |= static_cast<ValueType>(bit);
}

template <class EnumType>
inline void Bitmask<EnumType>::unset(EnumType bit) {
	bits &= ~static_cast<ValueType>(bit);
}

template <class EnumType>
[[nodiscard]] inline bool Bitmask<EnumType>::isSet(EnumType bit) const {
	return bits & static_cast<ValueType>(bit);
}

template <class EnumType>
inline constexpr void Bitmask<EnumType>::flip(EnumType bit) {
	bits ^= static_cast<ValueType>(bit);
}

template <class EnumType>
inline constexpr bool operator==(Bitmask<EnumType> mask0, Bitmask<EnumType> mask1) noexcept {
	return mask0.value() == mask1.value();
}

template <class EnumType>
inline constexpr bool operator!=(Bitmask<EnumType> mask0, Bitmask<EnumType> mask1) noexcept {
	return ! (mask0 == mask1);
}

template <class T>
inline constexpr Bitmask<T> operator&(T l, const Bitmask<T>& r) noexcept {
	return r & Bitmask { l };
}

template <class T>
inline constexpr Bitmask<T> operator|(T l, Bitmask<T> r) noexcept {
	return r | Bitmask { l };
}

template <class T>
inline constexpr Bitmask<T> operator^(T l, const Bitmask<T>& r) noexcept {
	return r ^ Bitmask { l };
}

template <class T>
inline constexpr Bitmask<T> operator&(const Bitmask<T>& r, T l) noexcept {
	return r & Bitmask { l };
}

template <class T>
inline constexpr Bitmask<T> operator|(const Bitmask<T>& r, T l) noexcept {
	return r | Bitmask { l };
}

template <class T>
inline constexpr Bitmask<T> operator^(const Bitmask<T>& r, T l) noexcept {
	return r ^ Bitmask { l };
}

} // namespace Typhoon
