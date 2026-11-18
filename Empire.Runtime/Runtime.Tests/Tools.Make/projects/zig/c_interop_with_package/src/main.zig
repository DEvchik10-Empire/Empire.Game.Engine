const std = @import("std");
const c = @cImport({
    @cInclude("zlib.h");
});

pub fn main() void {
    const version = c.zlibVersion();
    const ver_str: [*:0]const u8 = @ptrCast(version);
    std.debug.print("zlib version: {s}\n", .{ver_str});
}
