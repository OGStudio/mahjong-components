
/*
This file is part of OGS Mahjong components:
  https://github.com/OGStudio/ogs-mahjong-components

Copyright (C) 2018 Opensource Game Studio

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef OGS_MAHJONG_COMPONENTS_MAHJONG_H
#define OGS_MAHJONG_COMPONENTS_MAHJONG_H

// OMC_MAHJONG_LOG Start
#include "log.h"
#include "format.h"
#define OMC_MAHJONG_LOG_PREFIX "mahjong %s"
#define OMC_MAHJONG_LOG(...) \
    log::logprintf( \
        OMC_MAHJONG_LOG_PREFIX, \
        format::printfString(__VA_ARGS__).c_str() \
    )
// OMC_MAHJONG_LOG End

namespace omc
{
namespace mahjong
{

// TilePosition Start
struct TilePosition
{
    TilePosition() :
        field(0),
        row(0),
        column(0)
    { }

    TilePosition(int field, int row, int column) :
        field(field),
        row(row),
        column(column)
    { }

    int field;
    int row;
    int column;
};
// TilePosition End

// Layout Start
//! Layout representation.
struct Layout
{
    typedef std::vector<TilePosition> Positions;

    int width;
    int height;
    int depth;
    Positions positions;
};
// Layout End
// KMahjonggLayout Start
//! KMahjongg layout representation to use only during parsing.
struct KMahjonggLayout : Layout
{
    std::string version;
};
// KMahjonggLayout End


// kmahjonggLayoutFieldsToPositions Start
typedef std::vector<std::string> Field;
typedef std::vector<Field> Fields;

KMahjonggLayout::Positions kmahjonggLayoutFieldsToPositions(
    const Fields &fields,
    int width,
    int height
) {
    KMahjonggLayout::Positions positions;
    for (int fieldId = 0; fieldId < fields.size(); ++fieldId)
    {
        auto field = fields[fieldId];
        for (int row = 0; row < height - 1; ++row)
        {
            for (int column = 0; column < width - 1; ++column)
            {
                // Detect tile.
                if (
                    (field[row][column] == '1') &&
                    (field[row][column + 1] == '2') &&
                    (field[row + 1][column] == '4') &&
                    (field[row + 1][column + 1] == '3')
                ) {
                    positions.push_back({fieldId, row, column});
                }
            }
        }
    }
    return positions;
}
// kmahjonggLayoutFieldsToPositions End
// linesToKMahjonggLayout Start
bool linesToKMahjonggLayout(
    const std::vector<std::string> &lines,
    KMahjonggLayout &layout
) {
    KMahjonggLayout layoutDraft;
    // Setup defaults.
    // KMahjongg layouts of version 1.0 have 32x16 field size.
    // So we simply set these values as defaults.
    layoutDraft.width = 32;
    layoutDraft.height = 16;
    // Depth is counted for version 1.0. So set it to zero.
    layoutDraft.depth = 0;

    // Prefixes.
    const std::string PREFIX_COMMENT = "#";
    const std::string PREFIX_VERSION = "kmahjongg-layout-v";
    const std::string PREFIX_DEPTH = "d";
    const std::string PREFIX_WIDTH = "w";
    const std::string PREFIX_HEIGHT = "h";

    Fields fields;
    Field field;
    // Count field lines to detect end-of-field.
    int fieldLineId = 0;

    for (auto ln : lines)
    {
        // Ignore comment.
        if (format::stringStartsWith(ln, PREFIX_COMMENT))
        {
            continue;
        }
        // Remove spaces.
        auto sln = format::trimmedString(ln);
        // Version.
        if (format::stringStartsWith(sln, PREFIX_VERSION))
        {
            layoutDraft.version = sln.substr(PREFIX_VERSION.length());
            // Explicitely support expected versions.
            if (
                layoutDraft.version != "1.0" &&
                layoutDraft.version != "1.1"
            ) {
                return false;
            }
        }
        // Depth.
        else if (format::stringStartsWith(sln, PREFIX_DEPTH))
        {
            auto depth = sln.substr(PREFIX_DEPTH.length());
            layoutDraft.depth = atoi(depth.c_str());
        }
        // Width.
        else if (format::stringStartsWith(sln, PREFIX_WIDTH))
        {
            auto width = sln.substr(PREFIX_WIDTH.length());
            layoutDraft.width = atoi(width.c_str());
        }
        // Height.
        else if (format::stringStartsWith(sln, PREFIX_HEIGHT))
        {
            auto height = sln.substr(PREFIX_HEIGHT.length());
            layoutDraft.height = atoi(height.c_str());
        }
        else
        {
            // Add line to current field.
            field.push_back(sln);
            ++fieldLineId;
            // Once we have collected enough lines, add current
            // field to the list of fields.
            if (fieldLineId >= layoutDraft.height)
            {
                fields.push_back(field);
                // Reset field buffer for the next field lines.
                field.clear();
                fieldLineId = 0;
            }
        }
    }
    // Make sure depth is equal to the number of fields
    // if depth was specified in layout.
    if (layoutDraft.depth)
    {
        if (fields.size() != layoutDraft.depth)
        {
            OMC_MAHJONG_LOG(
                "ERROR Specified layout depth (%d) is not equal to actual one (%d)",
                layoutDraft.depth,
                fields.size()
            );
            return false;
        }
    }
    // Provide layout depth if it was counted.
    else
    {
        layoutDraft.depth = fields.size();
    }
    // Positions.
    layoutDraft.positions =
        kmahjonggLayoutFieldsToPositions(
            fields,
            layoutDraft.width,
            layoutDraft.height
        );

    // Provide cooked layout.
    layout = layoutDraft;
    return true;
}
// linesToKMahjonggLayout End
// matchIds Start
typedef std::vector<int> MatchIds;

// Generate ids for 144 tiles according to Mahjong Solitaire rules:
// http://www.rubl.com/rules/mahjong-solitaire-rules.html
MatchIds matchIds()
{
    MatchIds ids;

    // There are 42 different match ids overall.
    const int IDS_COUNT = 42;
    // 35 of them are represented by 4 duplicates. The rest are single.
    const int GROUPED_IDS_COUNT = 35;

    // Generate 4 duplicates for each grouped id.
    // Generate single id for the rest of ids.
    for (int id = 0; id < IDS_COUNT; ++id)
    {
        bool isIdGrouped = (id - GROUPED_IDS_COUNT) < 1;
        int duplicatesCount = isIdGrouped ? 4 : 1;
        for (int duplicateId = 0; duplicateId < duplicatesCount; ++duplicateId)
        {
            ids.push_back(id);
        }
    }
    
    return ids;
}

// Generate ids for any number of tiles by reusing the same 144 ids.
MatchIds matchIds(int count)
{
    MatchIds limitedIds = matchIds();
    MatchIds ids;

    int id = 0;
    for (int i = 0; i < count; ++i)
    {
        int matchId = limitedIds[id];
        ids.push_back(matchId);

        // Reuse.
        if (++id >= limitedIds.size())
        {
            id = 0;
        }
    }

    return ids;
}
// matchIds End
// parseLayout Start
bool parseLayout(std::istream &in, Layout &layout)
{
    // Collect lines.
    std::vector<std::string> lines;
    std::string ln;
    while (std::getline(in, ln))
    {
        lines.push_back(ln);
    }

    // Parse them.
    KMahjonggLayout kmlayout;
    auto success = linesToKMahjonggLayout(lines, kmlayout);

    // Provide layout if parsing was successful.
    if (success)
    {
        layout = kmlayout;
    }
    
    return success;
}
// parseLayout End


} // namespace mahjong
} // namespace omc

#endif // OGS_MAHJONG_COMPONENTS_MAHJONG_H
