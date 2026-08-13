// SPDX-License-Identifier: MIT
#include "UnitConversion.h"

#include <algorithm>
#include <ranges>

namespace qttutorial::units {

const std::vector<Unit>& catalogue()
{
    static const std::vector<Unit> units{
        {"m", "metre", Quantity::Length, 1.0, 0.0},
        {"km", "kilometre", Quantity::Length, 1000.0, 0.0},
        {"mi", "mile", Quantity::Length, 1609.344, 0.0},
        {"ft", "foot", Quantity::Length, 0.3048, 0.0},
        {"kg", "kilogram", Quantity::Mass, 1.0, 0.0},
        {"lb", "pound", Quantity::Mass, 0.45359237, 0.0},
        {"g", "gram", Quantity::Mass, 0.001, 0.0},
        {"K", "kelvin", Quantity::Temperature, 1.0, 0.0},
        {"C", "celsius", Quantity::Temperature, 1.0, 273.15},
        {"F", "fahrenheit", Quantity::Temperature, 5.0 / 9.0, 459.67 * 5.0 / 9.0},
    };
    return units;
}

std::expected<const Unit*, ConversionError> findUnit(std::string_view symbol)
{
    const auto& units = catalogue();
    const auto it = std::ranges::find(units, symbol, &Unit::symbol);
    if (it == units.end()) {
        return std::unexpected(ConversionError::UnknownUnit);
    }
    return &(*it);
}

std::expected<double, ConversionError> convert(double value, std::string_view fromSymbol,
                                                 std::string_view toSymbol)
{
    const auto from = findUnit(fromSymbol);
    if (!from) {
        return std::unexpected(from.error());
    }
    const auto to = findUnit(toSymbol);
    if (!to) {
        return std::unexpected(to.error());
    }
    if ((*from)->quantity != (*to)->quantity) {
        return std::unexpected(ConversionError::IncompatibleQuantities);
    }

    const double baseValue = value * (*from)->toBaseFactor + (*from)->toBaseOffset;
    return (baseValue - (*to)->toBaseOffset) / (*to)->toBaseFactor;
}

} // namespace qttutorial::units
