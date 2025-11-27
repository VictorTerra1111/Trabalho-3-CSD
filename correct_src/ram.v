module ram #(
    parameter ADDR_WIDTH = 15
) (
    input wire clk,
    input wire [ADDR_WIDTH-1:0] address,
    input wire enable,
    input wire [3:0] write_byte_enable,
    input wire [31:0] data_write,
    output reg [31:0] data_read
);
    reg [31:0] mem[0:(1<<ADDR_WIDTH)-1];
    always @(posedge clk) begin
        if (enable) begin
            if (write_byte_enable != 4'b0000) begin
                mem[address] <= data_write;
            end
            data_read <= mem[address];
        end
    end
endmodule