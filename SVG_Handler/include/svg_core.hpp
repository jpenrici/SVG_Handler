#pragma once

#include <expected>
#include <string>
#include <vector>
#include <utility>

namespace svg_core {

    /**
     * @brief Represents the type of an SVG tag.
     */
    enum class TagType {
        Unknown = 0,
        Open,       /// < <tag>
        Close,      /// < </tag>
        SelfClose   /// < <tag />
    };

    /**
     * @brief The Status enum
     */
    enum class Status {
        Success = 0,
        EmptyInput,
        InvalidRoot,
        UnbalancedTags
    };

    /**
     * @brief Represents an attribute of an SVG tag (e.g. width="200").
     */
    using Attribute = std::pair<std::string, std::string>;

    /**
     * @brief Represents the list of attributes of a tag.
     */
    using Attributes = std::vector<Attribute>;

    /**
     * @brief Fundamental structure of a tokenized SVG tag.
     */
    using TagTuple = std::tuple<std::string, Attributes, TagType>;

    /**
     * @brief Auxiliary structures for integration with CSV.
     */
    using CsvRow = std::vector<std::string>;
    using CsvTable = std::vector<CsvRow>;

} // namespace svg_core
