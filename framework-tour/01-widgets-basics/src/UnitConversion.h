// SPDX-License-Identifier: MIT
#pragma once

#include <expected>
#include <string_view>
#include <vector>

namespace qttutorial::units {

enum class Quantity { Length, Mass, Temperature };

struct Unit {
    std::string_view symbol;
    std::string_view name;
    Quantity quantity;
    double toBaseFactor; // multiply by this to get the quantity's base unit
    double toBaseOffset; // added after scaling (needed for temperature)
};

enum class ConversionError { UnknownUnit, IncompatibleQuantities };

// The catalogue of convertible units. A base unit per quantity (metre, kilogram,
// kelvin) plus a handful of derived ones, enough to demonstrate a real, non-trivial
// domain model backing a Qt Widgets table view.
[[nodiscard]] const std::vector<Unit>& catalogue();

[[nodiscard]] std::expected<double, ConversionError> convert(double value, std::string_view fromSymbol,
                                                               std::string_view toSymbol);

[[nodiscard]] std::expected<const Unit*, ConversionError> findUnit(std::string_view symbol);

} // namespace qttutorial::units
