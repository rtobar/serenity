/*
 * Copyright (c) 2020-2021, Itamar S. <itamar8910@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "AddressRanges.h"
#include "DwarfTypes.h"

namespace Debug::Dwarf {

AddressRanges::AddressRanges(ReadonlyBytes range_lists_data, size_t offset, CompilationUnit const& compilation_unit)
    : m_range_lists_stream(range_lists_data)
    , m_compilation_unit(compilation_unit)
{
    m_range_lists_stream.seek(offset);
}

void AddressRanges::for_each_range(Function<void(Range)> callback)
{
    // Dwarf version 5, section 2.17.3 "Non-Contiguous Address Ranges"

    Optional<FlatPtr> current_base_address;
    while (!m_range_lists_stream.eof() && !m_range_lists_stream.has_any_error()) {
        u8 entry_type;
        m_range_lists_stream >> entry_type;
        switch (static_cast<RangeListEntryType>(entry_type)) {
        case RangeListEntryType::BaseAddress: {
            FlatPtr base;
            m_range_lists_stream >> base;
            current_base_address = base;
            break;
        }
        case RangeListEntryType::OffsetPair: {
            Optional<FlatPtr> base_address = current_base_address;
            if (!base_address.has_value()) {
                base_address = m_compilation_unit.base_address();
            }

            if (!base_address.has_value()) {
                dbgln("expected base_address for rangelist");
                return;
            }

            size_t start_offset, end_offset;
            m_range_lists_stream.read_LEB128_unsigned(start_offset);
            m_range_lists_stream.read_LEB128_unsigned(end_offset);
            callback(Range { start_offset + *base_address, end_offset + *base_address });
            break;
        }
        case RangeListEntryType::StartLength: {
            FlatPtr start;
            m_range_lists_stream >> start;
            size_t length;
            m_range_lists_stream.read_LEB128_unsigned(length);
            callback(Range { start, start + length });
            break;
        }
        case RangeListEntryType::EndOfList:
            return;
        default:
            dbgln("unsupported range list entry type: 0x{:x}", (int)entry_type);
            return;
        }
    }
}

}
