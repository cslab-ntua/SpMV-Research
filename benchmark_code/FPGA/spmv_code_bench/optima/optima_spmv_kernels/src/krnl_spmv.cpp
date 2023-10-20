#include <stdio.h>
#include <hls_stream.h>
#include "ap_int.h"

typedef double ValueType;

#define W 8

typedef ap_uint<32>  ColType;
typedef ap_uint<1> RowType;

typedef struct v_datatype { ValueType data[W]; } v_dt;
typedef struct v_datatypeint { ColType data[W]; } v_dti;
typedef struct v_datatyperow { RowType data[W]; } v_dtr;

void read_input_streams_ja_coef(v_dti *ja, hls::stream<v_dti> &jaStream, v_dt *coef, hls::stream<v_dt> &coefStream, int nterm) {
	unsigned int vSize = ((nterm - 1) / W) + 1;
	mem_rd_ja_coef:
	for (int i = 0; i < vSize; i++) {
    	#pragma HLS PIPELINE II=1
		jaStream << ja[i];
		coefStream << coef[i];
	}
}

void read_x(hls::stream<v_dti> &ja, ValueType *x, hls::stream<v_dt> &xstream, hls::stream<v_dtr> &iatStream, int nterm) {
	rd_x:
	ColType col;
	v_dti coltmp;
	v_dt xtmp;
	v_dtr rowtmp;

	LOOP_rd_x:
	for(int i = 0; i < nterm; i+=W) {
  		#pragma HLS PIPELINE
		coltmp = ja.read();
		LOOP_INNER_rd_x:
		for(int k = 0; k < W; k++) {
    		#pragma HLS UNROLL
			col = coltmp.data[k].range(0,30);
			rowtmp.data[k] = coltmp.data[k].range(31,31);
			xtmp.data[k] = x[col];
		}
		xstream << xtmp;
		iatStream << rowtmp;
	}
}

void spmv_csr(hls::stream<v_dt> &coef, hls::stream<v_dtr> &iat, hls::stream<v_dt> &xstream, hls::stream<ValueType> &b, int nterm) {
	execute:
	RowType m1;
	ValueType sum=0;
	ColType col;

	v_dt datatmp;
	v_dt xtmp;
	v_dtr rowtmp;

	ValueType temp;
	ValueType temp2;

	ValueType sum_p[W];
	#pragma HLS ARRAY_PARTITION variable=sum_p complete dim=1
	LOOP_init_sum_p: 
	for(int k = 0; k < W; k++) {
		#pragma HLS UNROLL
		sum_p[k] = 0;
	}

	ValueType tmp_red1, tmp_red2, tmp_red3, tmp_red4, tmp_red5, tmp_red6;	

	LOOP_execute:
	for(int i = 0; i < nterm; i+=W) {
		#pragma HLS PIPELINE II=8 rewind
		datatmp = coef.read();
		xtmp    = xstream.read();
		rowtmp   = iat.read();

		LOOP_INNER2_execute:
		for (int k = 0; k < W; k++){
			#pragma HLS UNROLL
			temp  = datatmp.data[k];
			temp2 = xtmp.data[k];
			// if(temp!=0){
			sum_p[k] += temp*temp2;
			// }
			m1 = rowtmp.data[k];
		}
		
		// LOOP_INNER_sum_p: 
		// for (int k = 0; k < W; k++) {
		// 	#pragma HLS UNROLL
		// 	sum += sum_p[k];
		// }
		tmp_red1 = sum_p[0]+sum_p[4];
		tmp_red2 = sum_p[1]+sum_p[5];
		tmp_red3 = sum_p[2]+sum_p[6];
		tmp_red4 = sum_p[3]+sum_p[7];

		tmp_red5 = tmp_red1+tmp_red3;
		tmp_red6 = tmp_red2+tmp_red4;

		sum = tmp_red5 + tmp_red6;

		if(m1==1){
			b << sum;
			// sum = 0;
			LOOP_INNER_init_sum_p: 
			for(int k = 0; k < W; k++) {
				#pragma HLS UNROLL
				sum_p[k] = 0;
			}
		}
	}
}

void write_b(ValueType *b, hls::stream< ValueType> &bStream, int nrows) {
	mem_wr:
	for (int i = 0; i < nrows; i++) {
	    #pragma HLS PIPELINE II=1
		b[i] = bStream.read();
	}
}

extern "C" {

void krnl_spmv(int num_runs, int nrows, int nterm, v_dti *ja, v_dt *coef, ValueType *x, ValueType *b) {
	#pragma HLS INTERFACE m_axi port = ja   offset = slave bundle = gmem1
	#pragma HLS INTERFACE m_axi port = coef offset = slave bundle = gmem1
	#pragma HLS INTERFACE m_axi port = x num_read_outstanding=64  depth=512 offset = slave bundle = gmem2
	#pragma HLS INTERFACE m_axi port = b num_write_outstanding=64 depth=512 offset = slave bundle = gmem0

	#pragma HLS INTERFACE s_axilite port = num_runs
	#pragma HLS INTERFACE s_axilite port = nrows
	#pragma HLS INTERFACE s_axilite port = nterm

	static hls::stream<v_dt>      coefStream;
	static hls::stream<ValueType> bStream;
	static hls::stream<v_dti>     jaStream;
	static hls::stream<v_dtr>     iatStream;
	static hls::stream<v_dt>      xStream;

	#pragma HLS STREAM variable = coefStream    depth = 512
	#pragma HLS STREAM variable = bStream       depth = 512
	#pragma HLS STREAM variable = jaStream      depth = 512
	#pragma HLS STREAM variable = iatStream     depth = 512
	#pragma HLS STREAM variable = xStream       depth = 512

	for(int i=0; i<num_runs; i++){
		#pragma HLS dataflow
		read_input_streams_ja_coef(ja, jaStream, coef, coefStream, nterm);
		read_x(jaStream, x, xStream, iatStream, nterm);
		spmv_csr(coefStream, iatStream, xStream, bStream, nterm);
		write_b(b, bStream, nrows);
		
	}
}

}
