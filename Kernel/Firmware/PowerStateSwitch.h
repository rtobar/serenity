/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2021, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefPtr.h>
#include <AK/Types.h>
#include <AK/Vector.h>
#include <Kernel/FileSystem/SysFS.h>
#include <Kernel/Firmware/SysFSFirmware.h>
#include <Kernel/KBuffer.h>
#include <Kernel/Memory/MappedROM.h>
#include <Kernel/Memory/Region.h>
#include <Kernel/PhysicalAddress.h>
#include <Kernel/VirtualAddress.h>

namespace Kernel {

class PowerStateSwitchNode final : public SysFSComponent {
public:
    static NonnullRefPtr<PowerStateSwitchNode> must_create(FirmwareSysFSDirectory&);
    virtual mode_t permissions() const override;
    virtual KResultOr<size_t> write_bytes(off_t, size_t, UserOrKernelBuffer const&, OpenFileDescription*) override;

private:
    PowerStateSwitchNode(FirmwareSysFSDirectory&);

    void reboot();
    void poweroff();
};

}
