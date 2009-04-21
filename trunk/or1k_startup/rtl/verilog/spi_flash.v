//////////////////////////////////////////////////////////////////////
////                                                              ////
////  spi_shift.v                                                 ////
////                                                              ////
////  This file is part of the SPI IP core project                ////
////  http://www.opencores.org/projects/spi/                      ////
////                                                              ////
////  Author(s):                                                  ////
////      - Simon Srot (simons@opencores.org)                     ////
////                                                              ////
////  All additional information is avaliable in the Readme.txt   ////
////  file.                                                       ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2002 Authors                                   ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE.  See the GNU Lesser General Public License for more ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from http://www.opencores.org/lgpl.shtml                     ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
 `timescale 1ns / 10ps
module spi_flash_clgen (clk_in, rst, go, enable, last_clk, clk_out, pos_edge, neg_edge); 
   parameter divider_len = 2;
   parameter divider = 1;
   parameter Tp = 1;
   input                            clk_in;   
   input                            rst;      
   input                            enable;   
   input                            go;       
   input                            last_clk; 
   output                           clk_out;  
   output                           pos_edge; 
   output                           neg_edge; 
   reg                              clk_out;
   reg                              pos_edge;
   reg                              neg_edge;
   reg [divider_len-1:0] 	    cnt;      
   wire                             cnt_zero; 
   wire                             cnt_one;  
   assign cnt_zero = cnt == {divider_len{1'b0}};
   assign cnt_one  = cnt == {{divider_len-1{1'b0}}, 1'b1};
   always @(posedge clk_in or posedge rst)
     begin
	if(rst)
	  cnt <= #Tp {divider_len{1'b1}};
	else
	  begin
             if(!enable || cnt_zero)
               cnt <= #Tp divider;
             else
               cnt <= #Tp cnt - {{divider_len-1{1'b0}}, 1'b1};
	  end
     end
   always @(posedge clk_in or posedge rst)
     begin
	if(rst)
	  clk_out <= #Tp 1'b0;
	else
	  clk_out <= #Tp (enable && cnt_zero && (!last_clk || clk_out)) ? ~clk_out : clk_out;
     end
   always @(posedge clk_in or posedge rst)
     begin
	if(rst)
	  begin
             pos_edge  <= #Tp 1'b0;
             neg_edge  <= #Tp 1'b0;
	  end
	else
	  begin
             pos_edge  <= #Tp (enable && !clk_out && cnt_one) || (!(|divider) && clk_out) || (!(|divider) && go && !enable);
             neg_edge  <= #Tp (enable && clk_out && cnt_one) || (!(|divider) && !clk_out && enable);
	  end
     end
endmodule
 `timescale 1ns / 10ps
module spi_flash_shift 
  (
   clk, rst, latch, byte_sel, len, go,
   pos_edge, neg_edge,
   lsb, rx_negedge, tx_negedge,
   tip, last, 
   p_in, p_out, s_clk, s_in, s_out);
  parameter Tp = 1;
   input                          clk;          
   input                          rst;          
   input 			  latch;        
   input [3:0] 			  byte_sel;     
   input [6-1:0] len;          
   input                          lsb;          
   input 			  tx_negedge;
   input 			  rx_negedge;
   input                          go;           
   input                          pos_edge;     
   input                          neg_edge;     
   output                         tip;          
   output                         last;         
   input [31:0] 		  p_in;         
   output [32-1:0] 	  p_out;        
   input                          s_clk;        
   input                          s_in;         
   output                         s_out;        
   reg                            s_out;        
   reg                            tip;
   reg [6:0] 	  cnt;          
   reg [32-1:0] 	  data;         
   wire [6:0] 	  tx_bit_pos;   
   wire [6:0] 	  rx_bit_pos;   
   wire                           rx_clk;       
   wire                           tx_clk;       
   assign p_out = data;
   assign tx_bit_pos = lsb ? {!(|len), len} - cnt : cnt - {{6{1'b0}},1'b1};
   assign rx_bit_pos = lsb ? {!(|len), len} - (rx_negedge ? cnt + {{6{1'b0}},1'b1} : cnt) : (rx_negedge ? cnt : cnt - {{6{1'b0}},1'b1});
  assign last = !(|cnt);
  assign rx_clk = (rx_negedge ? neg_edge : pos_edge) && (!last || s_clk);
  assign tx_clk = (tx_negedge ? neg_edge : pos_edge) && !last;
  always @(posedge clk or posedge rst)
  begin
    if(rst)
      cnt <= #Tp {6+1{1'b0}};
    else
      begin
        if(tip)
          cnt <= #Tp pos_edge ? (cnt - {{6{1'b0}}, 1'b1}) : cnt;
        else
          cnt <= #Tp !(|len) ? {1'b1, {6{1'b0}}} : {1'b0, len};
      end
  end
  always @(posedge clk or posedge rst)
  begin
    if(rst)
      tip <= #Tp 1'b0;
  else if(go && ~tip)
    tip <= #Tp 1'b1;
  else if(tip && last && pos_edge)
    tip <= #Tp 1'b0;
  end
  always @(posedge clk or posedge rst)
  begin
    if (rst)
      s_out   <= #Tp 1'b0;
    else
      s_out <= #Tp (tx_clk || !tip) ? data[tx_bit_pos[6-1:0]] : s_out;
  end
   always @(posedge clk or posedge rst)
     if (rst)
       data   <= #Tp 32'h03000000;
     else
       if (latch & !tip)
	 begin
            if (byte_sel[0])
              data[7:0] <= #Tp p_in[7:0];
            if (byte_sel[1])
              data[15:8] <= #Tp p_in[15:8];
            if (byte_sel[2])
              data[23:16] <= #Tp p_in[23:16];
            if (byte_sel[3])
              data[31:24] <= #Tp p_in[31:24];
	 end
       else
	 data[rx_bit_pos[6-1:0]] <= #Tp rx_clk ? s_in : data[rx_bit_pos[6-1:0]];
endmodule
 `timescale 1ns / 10ps
module spi_flash_top
  (
   wb_clk_i, wb_rst_i, wb_adr_i, wb_dat_i, wb_dat_o, wb_sel_i,
   wb_we_i, wb_stb_i, wb_cyc_i, wb_ack_o, 
   ss_pad_o, sclk_pad_o, mosi_pad_o, miso_pad_i
   );
   parameter divider_len = 2;
   parameter divider = 0;
   parameter Tp = 1;
   input                            wb_clk_i;         
   input                            wb_rst_i;         
   input [4:2] 			    wb_adr_i;         
   input [31:0] 		    wb_dat_i;         
   output [31:0] 		    wb_dat_o;         
   input [3:0] 			    wb_sel_i;         
   input                            wb_we_i;          
   input                            wb_stb_i;         
   input                            wb_cyc_i;         
   output                           wb_ack_o;         
   output [2-1:0] 	    ss_pad_o;         
   output                           sclk_pad_o;       
   output                           mosi_pad_o;       
   input                            miso_pad_i;       
   reg [31:0] 			    wb_dat_o;
   reg                              wb_ack_o;
   wire [14-1:0] 	    ctrl;             
   reg [2-1:0] 	    ss;               
   wire [32-1:0] 	    rx;               
   wire [5:0] 			    char_len;
   reg 				    char_len_ctrl;    
   reg 				    go;               
   wire 			    spi_ctrl_sel;     
   wire 			    spi_tx_sel;       
   wire 			    spi_ss_sel;       
   wire                             tip;              
   wire                             pos_edge;         
   wire                             neg_edge;         
   wire                             last_bit;         
  wire                             rx_negedge;       
  wire                             tx_negedge;       
  wire                             lsb;              
  wire                             ass;              
   assign spi_ctrl_sel    = wb_cyc_i & wb_stb_i & (wb_adr_i[4:2] == 4);
   assign spi_tx_sel      = wb_cyc_i & wb_stb_i & (wb_adr_i[4:2] == 0);
   assign spi_ss_sel      = wb_cyc_i & wb_stb_i & (wb_adr_i[4:2] == 6);
   always @(posedge wb_clk_i or posedge wb_rst_i)
  begin
     if (wb_rst_i)
       wb_dat_o <= #Tp 32'b0;
     else
       case (wb_adr_i[4:2])
	 0:    wb_dat_o <= rx;	 
	 4:    wb_dat_o <= {18'd0, ctrl};
	 5:  wb_dat_o <= {{32-divider_len{1'b0}}, divider};
	 6:      wb_dat_o <= {{32-2{1'b0}}, ss};
	default:      wb_dat_o  <= rx;
       endcase
  end
   always @(posedge wb_clk_i or posedge wb_rst_i)
     begin
	if (wb_rst_i)
	  wb_ack_o <= #Tp 1'b0;
    else
      wb_ack_o <= #Tp wb_cyc_i & wb_stb_i & ~wb_ack_o;
     end
   always @(posedge wb_clk_i or posedge wb_rst_i)
     begin
	if (wb_rst_i)
	  {go,char_len_ctrl} <= #Tp 2'b01;
	else if(spi_ctrl_sel && wb_we_i && !tip)
	  begin
             if (wb_sel_i[0])
               char_len_ctrl <= #Tp wb_dat_i[5];
             if (wb_sel_i[1])
	       go <= #Tp wb_dat_i[8];
	  end
	else if(tip && last_bit && pos_edge)
	  go <= #Tp 1'b0;
     end
   assign char_len = char_len_ctrl ? 6'd32 : 6'd8;   
   assign ass = 1'b0;
   assign lsb = 1'b0;
   assign rx_negedge = 1'b0;
   assign tx_negedge = 1'b1;
   assign ctrl = {ass,1'b0,lsb,tx_negedge,rx_negedge,go,1'b0,1'b0,char_len};
   always @(posedge wb_clk_i or posedge wb_rst_i)
     if (wb_rst_i)
       ss <= #Tp {2{1'b0}};
     else if(spi_ss_sel && wb_we_i && !tip)
       if (wb_sel_i[0])
         ss <= #Tp wb_dat_i[2-1:0];
   assign ss_pad_o = ~((ss & {2{tip & ass}}) | (ss & {2{!ass}}));
   spi_flash_clgen
     #
     (
      .divider_len(divider_len),
      .divider(divider)
      )
     clgen 
       (
	.clk_in(wb_clk_i), 
	.rst(wb_rst_i), 
	.go(go), 
	.enable(tip), 
	.last_clk(last_bit),
	.clk_out(sclk_pad_o), 
	.pos_edge(pos_edge), 
        .neg_edge(neg_edge)
	);
   spi_flash_shift  shift 
     (
      .clk(wb_clk_i), 
      .rst(wb_rst_i), 
      .len(char_len[6-1:0]),
      .latch(spi_tx_sel & wb_we_i), 
      .byte_sel(wb_sel_i),
      .go(go), 
      .pos_edge(pos_edge), 
      .neg_edge(neg_edge),
      .lsb(lsb),
      .rx_negedge(rx_negedge), 
      .tx_negedge(tx_negedge), 
      .tip(tip), 
      .last(last_bit), 
      .p_in(wb_dat_i), 
      .p_out(rx), 
      .s_clk(sclk_pad_o), 
      .s_in(miso_pad_i), 
      .s_out(mosi_pad_o)
      );
endmodule
