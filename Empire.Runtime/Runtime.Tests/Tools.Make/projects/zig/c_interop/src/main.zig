const std = @import("std");
const c = @cImport({
    @cInclude("mathlib.h");
});

pub fn main() void {
    const sum = c.c_add(3, 4);
    const product = c.c_multiply(5, 6);
    std.debug.print("c_add(3,4)={d}\n", .{sum});
    std.debug.print("c_multiply(5,6)={d}\n", .{product});
}
