
module kbd_axis #(
	parameter AXIS_DATA_WIDTH = 8,
	parameter DEBOUNCE_COUNTER_SIZE = 8,
	parameter CLK_FREQ = 50000000
) (
	input  wire                ps2_clk_i,
	input  wire                ps2_data_i,
	input  wire                axis_aclk_i,
	input  wire                axis_aresetn_i,
	input wire m_axis_tready_i,
	output wire m_axis_tvalid_o,
	output reg [AXIS_DATA_WIDTH - 1:0] m_axis_tdata_o
);

	wire ps2_code_new;
	wire [7:0] ps2_code;

	ps2_keyboard #(
		.debounce_counter_size(DEBOUNCE_COUNTER_SIZE),
		.clk_freq(CLK_FREQ)
	) ps2_keyboard_inst (
		.clk(axis_aclk_i),
		.ps2_clk(ps2_clk_i),
		.ps2_data(ps2_data_i),
		.ps2_code_new(ps2_code_new),
		.ps2_code(ps2_code)
	);

	reg tvalid_reg;
	assign m_axis_tvalid_o = tvalid_reg;

	// Define estados como parâmetros
	localparam IDLE = 1'b0;
	localparam SEND = 1'b1;
	reg state;

	always @(posedge axis_aclk_i or negedge axis_aresetn_i) begin
		if (!axis_aresetn_i) begin
			tvalid_reg <= 1'b0;
			m_axis_tdata_o <= {AXIS_DATA_WIDTH{1'b0}};
			state <= IDLE;
		end else begin
			case (state)
				IDLE: begin
					tvalid_reg <= 1'b0;
					if (ps2_code_new) begin
						m_axis_tdata_o <= ps2_code;
						tvalid_reg <= 1'b1;
						state <= SEND;
					end
				end
				SEND: begin
					if (m_axis_tready_i) begin
						tvalid_reg <= 1'b0;
						state <= IDLE;
					end
				end
			endcase
		end
	end

endmodule
