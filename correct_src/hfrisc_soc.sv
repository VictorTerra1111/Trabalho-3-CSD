//================================================================================
// HF-RISC-V SoC - SystemVerilog Top Module (convertido do VHDL)
//================================================================================
module hfrisc_soc #(
    parameter int address_width = 15,
    parameter string memory_file = "code.txt"
) (
    input  wire clk_i,
    input  wire rst_i,
    inout  wire [15:0] gpioa,
    inout  wire [15:0] gpiob,
    output wire uart_tx,
    input  wire uart_rx,
    input  wire PS2_DATA,
    input  wire PS2_CLK,
    output wire vsync,
    output wire hsync,
    output wire [3:0] red,
    output wire [3:0] green,
    output wire [3:0] blue
);
    //========================================================================
    // Internal Signals
    //========================================================================
    logic clock, boot_enable, ram_enable_n, stall, ram_dly, rff1, reset;
    logic [31:0] address, data_read, data_write, data_read_boot, data_read_ram;
    logic [7:0] ext_irq;
    logic [3:0] data_we, data_w_n_ram;
    logic periph, periph_dly, periph_wr, periph_irq;
    logic [31:0] data_read_periph, data_read_periph_s, data_write_periph;
    logic [15:0] gpioa_in, gpioa_out, gpioa_ddr;
    logic [15:0] gpiob_in, gpiob_out, gpiob_ddr;
    logic ext_periph_axis, ext_periph_vga, ext_periph_axis_dly, ext_periph_vga_dly;
    logic reset_n;
    logic [31:0] data_read_axis, data_read_axis_s;
    logic m_axis_tready_s, m_axis_tvalid_s;
    logic [7:0] m_axis_tdata_s;
    logic [31:0] data_read_vga, data_read_vga_s;

    //========================================================================
    // Clock Divider (50MHz from 100MHz)
    //========================================================================
    always_ff @(posedge clk_i or posedge rst_i) begin
        if (rst_i) clock <= 1'b0;
        else clock <= ~clock;
    end

    //========================================================================
    // Reset Synchronizer
    //========================================================================
    always_ff @(posedge clock or posedge rst_i) begin
        if (rst_i) begin
            rff1 <= 1'b1;
            reset <= 1'b1;
        end else begin
            rff1 <= 1'b0;
            reset <= rff1;
        end
    end
    assign reset_n = ~reset;

    //========================================================================
    // GPIO A/B Configuration
    //========================================================================
    // Bidirectional assignments
    genvar i;
    generate
        for (i = 0; i < 16; i++) begin
            assign gpioa[i] = gpioa_ddr[i] ? gpioa_out[i] : 1'bz;
            assign gpioa_in[i] = gpioa_ddr[i] ? 1'b0 : gpioa[i];
            assign gpiob[i] = gpiob_ddr[i] ? gpiob_out[i] : 1'bz;
            assign gpiob_in[i] = gpiob_ddr[i] ? 1'b0 : gpiob[i];
        end
    endgenerate
    // UART connections
    // assign uart_tx = gpioa_out[2]; // Removido para evitar multi-driven net
    // assign gpioa_in[3] = uart_rx;  // Removido para evitar multi-driven net

    //========================================================================
    // Memory and Peripheral Address Decoding
    //========================================================================
    assign stall = 1'b0;
    assign boot_enable = (address[31:28] == 4'b0000);
    assign ram_enable_n = (address[31:28] == 4'b0100) ? 1'b0 : 1'b1;
    assign periph = (address[31:24] == 8'he1);

    // Pipeline Delay Registers
    always_ff @(posedge clock or posedge reset) begin
        if (reset) begin
            ram_dly <= 1'b0;
            periph_dly <= 1'b0;
            ext_periph_vga_dly <= 1'b0;
            ext_periph_axis_dly <= 1'b0;
        end else begin
            ram_dly <= ~ram_enable_n;
            periph_dly <= periph;
            ext_periph_vga_dly <= ext_periph_vga;
            ext_periph_axis_dly <= ext_periph_axis;
        end
    end

    // Data Read Multiplexer
    assign data_read = (ext_periph_axis | ext_periph_axis_dly) ? data_read_axis :
                       (ext_periph_vga | ext_periph_vga_dly) ? data_read_vga :
                       (periph | periph_dly) ? data_read_periph :
                       ((address[31:28] == 4'b0000) && !ram_dly) ? data_read_boot :
                       data_read_ram;
    assign data_w_n_ram = ~data_we;
    assign ext_irq = {7'b0, periph_irq};

    //========================================================================
    // Instanciação dos módulos
    //========================================================================
    processor processor_inst (
        .clk_i(clock),
        .rst_i(reset),
        .stall_i(stall),
        .addr_o(address),
        .data_i(data_read),
        .data_o(data_write),
        .data_w_o(data_we),
        .data_mode_o(),
        .extio_in(ext_irq),
        .extio_out()
    );

    peripherals peripherals_inst (
        .clk_i(clock),
        .rst_i(reset),
        .addr_i(address),
        .data_i(data_write_periph),
        .data_o(data_read_periph_s),
        .sel_i(periph),
        .wr_i(periph_wr),
        .irq_o(periph_irq),
        .gpioa_in(gpioa_in),
        .gpioa_out(gpioa_out),
        .gpioa_ddr(gpioa_ddr),
        .gpiob_in(gpiob_in),
        .gpiob_out(gpiob_out),
        .gpiob_ddr(gpiob_ddr)
    );

    assign data_read_periph = {data_read_periph_s[7:0], data_read_periph_s[15:8], data_read_periph_s[23:16], data_read_periph_s[31:24]};
    assign data_write_periph = {data_write[7:0], data_write[15:8], data_write[23:16], data_write[31:24]};
    assign periph_wr = (data_we != 4'b0000);

    // VGA
    assign data_read_vga = {data_read_vga_s[7:0], data_read_vga_s[15:8], data_read_vga_s[23:16], data_read_vga_s[31:24]};
    if_vga vga_core (
        .clk_i(clock),
        .rst_i(reset),
        .addr_i(address),
        .data_i(data_write_periph),
        .data_o(data_read_vga_s),
        .data_access_o(ext_periph_vga),
        .data_w_i(periph_wr),
        .board_clk_i(clk_i),
        .vsync(vsync),
        .hsync(hsync),
        .red(red),
        .green(green),
        .blue(blue)
    );

    // AXIS
    assign data_read_axis = {data_read_axis_s[7:0], data_read_axis_s[15:8], data_read_axis_s[23:16], data_read_axis_s[31:24]};
    if_axis if_axis_core1 (
        .addr_i(address),
        .data_o(data_read_axis_s),
        .data_access_o(ext_periph_axis),
        .data_w_i(periph_wr),
        .axis_aclk_i(clock),
        .axis_aresetn_i(reset_n),
        .s_axis_tready_o(m_axis_tready_s),
        .s_axis_tvalid_i(m_axis_tvalid_s),
        .s_axis_tdata_i(m_axis_tdata_s)
    );
    kbd_axis kbd_axis_inst (
        .ps2_clk_i(PS2_CLK),
        .ps2_data_i(PS2_DATA),
        .axis_aclk_i(clock),
        .axis_aresetn_i(reset_n),
        .m_axis_tready_i(m_axis_tready_s),
        .m_axis_tvalid_o(m_axis_tvalid_s),
        .m_axis_tdata_o(m_axis_tdata_s)
    );

    // Boot RAM
    ram #(.ADDR_WIDTH(32)) boot_ram (
        .clk(clock),
        .address(address[31:0]),
        .enable(boot_enable),
        .write_byte_enable(4'b0000),
        .data_write(32'h00000000),
        .data_read(data_read_boot)
    );

    // Main BRAM
    ram #(.ADDR_WIDTH(15)) bram_inst (
        .clk(clock),
        .address(address[14:0]),
        .enable(~ram_enable_n),
        .write_byte_enable(data_w_n_ram),
        .data_write(data_write),
        .data_read(data_read_ram)
    );

endmodule
